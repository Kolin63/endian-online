#include "command.h"

#include <assert.h>
#include <concord/application_command.h>
#include <concord/discord.h>
#include <concord/discord_codecs.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "bot.h"
#include "cJSON.h"
#include "command.h"
#include "fileio.h"
#include "function.h"
#include "json_iterator.h"
#include "json_macros.h"
#include "log.h"
#include "registry.h"
#include "regman.h"

// converts endian command options to concord command options
struct discord_application_command_options* command_options_end_to_conc(const struct command_options* end) {
  if (end == NULL) return NULL;

  struct discord_application_command_options* conc = malloc(sizeof(struct discord_application_command_options));
  conc->size = end->size;
  conc->array = malloc(sizeof(struct discord_application_command_option) * conc->size);
  for (int i = 0; i < end->size; i++) {
    const struct command_option* eopt = &(end->options[i]);
    struct discord_application_command_option* copt = &(conc->array[i]);
    copt->required = eopt->required;
    copt->autocomplete = eopt->autocomplete;
    copt->type = eopt->type;
    copt->name = eopt->name;
    copt->description = eopt->description;
    copt->choices = eopt->choices;
    copt->channel_types = eopt->channel_types;
    copt->min_value = eopt->min_value;
    copt->max_value = eopt->max_value;

    copt->options = command_options_end_to_conc(eopt->options);
  }
  return conc;
}

void discord_application_command_options_cleanup(struct discord_application_command_options* opts) {
  if (opts == NULL) return;
  for (int i = 0; i < opts->size; i++) {
    discord_application_command_options_cleanup(opts->array[i].options);
  }
  free(opts->array);
  free(opts);
}

// returns amount of errors, 0 if ok
// json should point to the root of the choices array
int command_option_choices_fillout(const char* mod_name, const char* file_name, const cJSON* json,
                                   struct discord_application_command_option_choices* choices) {
  int error = 0;

  END_JSON_CHECK_ROOT_ARRAY;

  struct json_iterator* iter = json_iterator_init(json);

  while (true) {
    iter = json_iterate(iter);
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    if (iter->json->type == cJSON_Object) {
      END_JSON_CHECK_OBJECT_AND_CHILDREN;
      choices->size++;
      choices->array = reallocarray(choices->array, choices->size, sizeof(struct discord_application_command_option_choice));
      struct discord_application_command_option_choice* i = &(choices->array[choices->size - 1]);
      i->name = NULL;
      i->value = NULL;
    } else if (iter->json->type == cJSON_String) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 100);
      if (choices->size == 0 || iter->parent == NULL || iter->parent->parent == NULL) {
        log_error("In command %s from mod %s, %s must be object", file_name, mod_name, json->string);
        error++;
        continue;
      }

      struct discord_application_command_option_choice* i = &(choices->array[choices->size - 1]);
      if (i->name != NULL || i->value != NULL) {
        log_error("In command %s from mod %s, choices is wrong size", file_name, mod_name, json->string);
        error++;
        continue;
      }

      if (strcmp(iter->json->string, "name") == 0) {
        const char* new = iter->json->valuestring;
        i->name = malloc(strlen(new) + 1);
        strcpy(i->name, new);
      } else if (strcmp(iter->json->string, "value") == 0) {
        const char* new = iter->json->valuestring;
        i->value = malloc(strlen(new) + 1);
        strcpy(i->value, new);
      } else {
        log_error("Command choice from command %s from mod %s has unknown object %s",
                  file_name, mod_name, iter->json->string);
        error++;
        continue;
      }
    } else {
      log_error("Command choice from command %s from mod %s has unknown object %s",
                file_name, mod_name, iter->json->string);
      if (iter->json->type == cJSON_Array)
        iter = json_iterator_skip_object(iter);
      error++;
      continue;
    }
  }

  json_iterator_cleanup(iter);

  return error;
}

// returns amount of errors, 0 if ok
// json should point to the root of the channel_types array
int command_option_channel_types_fillout(const char* mod_name,
                                         const char* file_name,
                                         const cJSON* json,
                                         struct integers* channel_types) {
  int error = 0;

  END_JSON_CHECK_ROOT_ARRAY;

  struct json_iterator* iter = json_iterator_init(json);

  while (true) {
    iter = json_iterate(iter);
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    END_JSON_CHECK_STRING;
    END_JSON_CHECK_STRING_LENGTH(1, 100);

    const char* val = iter->json->valuestring;
    int type;

    if (strcmp(val, "GUILD_TEXT") == 0) type = 0;
    else if (strcmp(val, "DM") == 0) type = 1;
    else if (strcmp(val, "GUILD_VOICE") == 0) type = 2;
    else if (strcmp(val, "GROUP_DM") == 0) type = 3;
    else if (strcmp(val, "GUILD_CATEGORY") == 0) type = 4;
    else if (strcmp(val, "GUILD_ANNOUNCEMENT") == 0) type = 5;
    else if (strcmp(val, "ANNOUNCEMENT_THREAD") == 0) type = 10;
    else if (strcmp(val, "PUBLIC_THREAD") == 0) type = 11;
    else if (strcmp(val, "PRIVATE_THREAD") == 0) type = 12;
    else if (strcmp(val, "GUILD_STAGE_VOICE") == 0) type = 13;
    else if (strcmp(val, "GUILD_DIRECTORY") == 0) type = 14;
    else if (strcmp(val, "GUILD_FORUM") == 0) type = 15;
    else if (strcmp(val, "GUILD_MEDIA") == 0) type = 16;
    else {
      log_error("In command %s from mod %s, unknown channel type %s", file_name, mod_name, val);
      error++;
      continue;
    }

    channel_types->size++;
    channel_types->array = reallocarray(channel_types->array, channel_types->size, sizeof(int));
    channel_types->array[channel_types->size - 1] = type;
  }

  return error;
}

// (forward declaration)
// returns amount of errors, 0 if ok
// json should point to the root of the options array
int command_options_fillout(const char* mod_name, const char* file_name,
                            const cJSON* json, struct command_options* opts);

// returns amount of errors, 0 if ok
// json should point to the root of the option
int command_option_fillout(const char* mod_name, const char* file_name,
                           const cJSON* json, struct command_option* opt) {
  int error = 0;

  struct json_iterator* iter = json_iterator_init(json);

  while (true) {
    iter = json_iterate(iter);
  loop_no_iterate:
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    const char* item_name = iter->json->string;

    if (strcmp(item_name, "type") == 0) {
      END_JSON_CHECK_STRING;
      const char* val = iter->json->valuestring;
      if (strcmp(val, "SUB_COMMAND") == 0) opt->type = DISCORD_APPLICATION_OPTION_SUB_COMMAND;
      else if (strcmp(val, "SUB_COMMAND_GROUP") == 0) opt->type = DISCORD_APPLICATION_OPTION_SUB_COMMAND_GROUP;
      else if (strcmp(val, "STRING") == 0) opt->type = DISCORD_APPLICATION_OPTION_STRING;
      else if (strcmp(val, "INTEGER") == 0) opt->type = DISCORD_APPLICATION_OPTION_INTEGER;
      else if (strcmp(val, "BOOLEAN") == 0) opt->type = DISCORD_APPLICATION_OPTION_BOOLEAN;
      else if (strcmp(val, "USER") == 0) opt->type = DISCORD_APPLICATION_OPTION_USER;
      else if (strcmp(val, "CHANNEL") == 0) opt->type = DISCORD_APPLICATION_OPTION_CHANNEL;
      else if (strcmp(val, "ROLE") == 0) opt->type = DISCORD_APPLICATION_OPTION_ROLE;
      else if (strcmp(val, "MENTIONABLE") == 0) opt->type = DISCORD_APPLICATION_OPTION_MENTIONABLE;
      else if (strcmp(val, "NUMBER") == 0) opt->type = DISCORD_APPLICATION_OPTION_NUMBER;
      else if (strcmp(val, "ATTACHMENT") == 0) opt->type = DISCORD_APPLICATION_OPTION_ATTACHMENT;
      else {
        log_error("In command %s from mod %s, unknown type %s", file_name, mod_name, val);
        error++;
        continue;
      }
    } else if (strcmp(item_name, "name") == 0) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 32);
      opt->name = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(opt->name, iter->json->valuestring);
    } else if (strcmp(item_name, "description") == 0) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 100);
      opt->description = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(opt->description, iter->json->valuestring);
    } else if (strcmp(item_name, "required") == 0) {
      END_JSON_CHECK_BOOL;
      opt->required = (iter->json->type == cJSON_True);
    } else if (strcmp(item_name, "min_value") == 0) {
      END_JSON_CHECK_STRING;
      opt->min_value = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(opt->min_value, iter->json->valuestring);
    } else if (strcmp(item_name, "max_value") == 0) {
      END_JSON_CHECK_STRING;
      opt->max_value = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(opt->max_value, iter->json->valuestring);
    } else if (strcmp(item_name, "autocomplete") == 0) {
      END_JSON_CHECK_BOOL;
      opt->autocomplete = (iter->json->type == cJSON_True);
    } else if (strcmp(item_name, "options") == 0) {
      END_JSON_CHECK_ARRAY_AND_CHILDREN;
      opt->options = malloc(sizeof(struct command_options));
      opt->options->options = NULL;
      opt->options->size = 0;
      command_options_fillout(mod_name, file_name, iter->json, opt->options);
      iter = json_iterator_skip_object(iter);
      goto loop_no_iterate;
    } else if (strcmp(item_name, "choices") == 0) {
      END_JSON_CHECK_ARRAY_AND_CHILDREN;
      opt->choices = malloc(sizeof(struct discord_application_command_option_choices));
      opt->choices->array = NULL;
      opt->choices->size = 0;
      error += command_option_choices_fillout(mod_name, file_name, iter->json, opt->choices);
      goto loop_no_iterate;
    } else if (strcmp(item_name, "channel_types") == 0) {
      END_JSON_CHECK_ARRAY_AND_CHILDREN;
      opt->channel_types = malloc(sizeof(struct integers));
      opt->channel_types->array = NULL;
      opt->channel_types->size = 0;
      error += command_option_channel_types_fillout(mod_name, file_name, iter->json, opt->channel_types);
      goto loop_no_iterate;
    } else {
      log_error("In command %s from mod %s, unknown object %s", file_name, mod_name, iter->json->string);
      if (iter->json->type == cJSON_Array || iter->json->type == cJSON_Object)
        iter = json_iterator_skip_object(iter);
      error++;
      continue;
    }
  }

  json_iterator_cleanup(iter);

  return error;
}

// returns amount of errors, 0 if ok
// json should point to the root of the options array
int command_options_fillout(const char* mod_name, const char* file_name,
                            const cJSON* json, struct command_options* opts) {
  int error = 0;

  if (json->type != cJSON_Array) {
    log_error("In command %s from mod %s, %s must be array", file_name, mod_name, json->string);
    error++;
    return error;
  }
  if (json->child == NULL) {
    log_error("In command %s from mod %s, %s has no children", file_name, mod_name, json->string);
    error++;
    return error;
  }

  cJSON* i = json->child;

  while (i != NULL) {
    if (i->type != cJSON_Object) {
      log_error("In option %s from mod %s, %s must be object", file_name, mod_name, i->string);
      error++;
      continue;
    }
    if (i->child == NULL) {
      log_error("In option %s from mod %s, %s has no children", file_name, mod_name, i->string);
      error++;
      continue;
    }

    struct command_option opt = {};
    int this_error = command_option_fillout(mod_name, file_name, i, &opt);
    error += this_error;
    if (this_error == 0) {
      opts->size++;
      opts->options = reallocarray(opts->options, opts->size, sizeof(struct command_option));
      opts->options[opts->size - 1] = opt;
    }

    i = i->next;
  }

  return error;
}

// returns amount of errors, 0 if ok
int command_fillout(const char* mod_name, const char* file_name,
                    const cJSON* json, struct command* params) {
  int error = 0;

  struct json_iterator* iter = json_iterator_init(json);

  while (true) {
    iter = json_iterate(iter);
  loop_no_iterate:
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    const char* item_name = iter->json->string;

    if (strcmp(item_name, "type") == 0) {
      END_JSON_CHECK_STRING;
      const char* val = iter->json->valuestring;
      if (strcmp(val, "CHAT_INPUT") == 0) params->type = DISCORD_APPLICATION_CHAT_INPUT;
      else if (strcmp(val, "USER") == 0) params->type = DISCORD_APPLICATION_USER;
      else if (strcmp(val, "MESSAGE") == 0) params->type = DISCORD_APPLICATION_MESSAGE;
      else {
        log_error("In command %s from mod %s, unknown type %s", file_name, mod_name, val);
        error++;
        continue;
      }
    } else if (strcmp(item_name, "name") == 0) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 32);
      params->name = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(params->name, iter->json->valuestring);
    } else if (strcmp(item_name, "description") == 0) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 100);
      params->description = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(params->description, iter->json->valuestring);
    } else if (strcmp(item_name, "default_member_permissions") == 0) {
      END_JSON_CHECK_STRING;
      char* endptr = iter->json->valuestring;
      errno = 0;
      unsigned long perms = strtoul(iter->json->valuestring, &endptr, 10);
      if (errno != 0 || *endptr != '\0') {
        log_error("In command %s from mod %s, error reading default_member_permissions", file_name, mod_name);
        error++;
        continue;
      }
      params->default_member_permissions = perms;
    } else if (strcmp(item_name, "callback") == 0) {
      END_JSON_CHECK_STRING;
      char* func = iter->json->valuestring;
      if (registry_ktoi(regman_get_function(), &(struct function){.name = iter->json->valuestring}) == -1) {
        log_error("In command %s from mod %s, function %s not registered", file_name, mod_name, func);
        error++;
        continue;
      }
      params->callback = malloc(strlen(func) + 1);
      strcpy(params->callback, func);
    } else if (strcmp(item_name, "options") == 0) {
      END_JSON_CHECK_ARRAY_AND_CHILDREN;
      params->options = malloc(sizeof(struct command_options));
      params->options->options = NULL;
      params->options->size = 0;
      command_options_fillout(mod_name, file_name, iter->json, params->options);
      iter = json_iterator_skip_object(iter);
      goto loop_no_iterate;
    } else {
      log_error("Command %s from mod %s has unknown object %s", file_name, mod_name, iter->json->string);
      if (iter->json->type == cJSON_Array || iter->json->type == cJSON_Object)
        iter = json_iterator_skip_object(iter);
      error++;
      continue;
    }
  }

  json_iterator_cleanup(iter);
  return error;
}

void command_load(const struct discord_ready* event, const char* command_path,
                  const char* mod_name, const char* file_name) {
  if (strcmp(file_name, "template.json") == 0) return;

  char* fileio = NULL;
  FILE* file = fopen(command_path, "r");

  if (!file) {
    log_error("Could not open command file from %s at %s", mod_name, command_path);
    return;
  }

  fileio_read_all(&fileio, file);

  cJSON* json = cJSON_Parse(fileio);

  free(fileio);
  fclose(file);

  struct command params = {};
  if (command_fillout(mod_name, file_name, json, &params) != 0) {
    cJSON_Delete(json);
    return;
  }

  cJSON_Delete(json);

  if (registry_add(regman_get_command(), (void*)&params) == NULL) {
    log_error("Command %s already registered", file_name);
    return;
  };

  struct discord_application_command_options* discord_opts = command_options_end_to_conc(params.options);

  struct discord_create_global_application_command discord_params = {
      .type = params.type,
      .name = params.name,
      .description = params.description,
      .options = discord_opts,
      .default_member_permissions = params.default_member_permissions};

  discord_create_global_application_command(bot_get_global()->discord_bot, event->application->id,
                                            &discord_params, NULL);
  discord_application_command_options_cleanup(discord_opts);
  log_info("Loading command %s from mod %s", file_name, mod_name);
}

const struct command* command_get(char* name) {
  return registry_ktov(regman_get_command(), &(struct command){.name = name});
}

void command_options_cleanup(struct command_options* opts) {
  if (opts == NULL) return;
  for (int i = 0; i < opts->size; i++) {
    struct command_option* opt = &(opts->options[i]);
    free(opt->name);
    free(opt->description);
    free(opt->min_value);
    free(opt->max_value);

    if (opt->choices != NULL) {
      for (int j = 0; j < opt->choices->size; j++) {
        struct discord_application_command_option_choice* c = &(opt->choices->array[j]);
        free(c->name);
        free(c->value);
      }
      free(opt->choices->array);
      free(opt->choices);
    }

    if (opt->channel_types != NULL) {
      free(opt->channel_types->array);
      free(opt->channel_types);
    }

    if (opt->options != NULL) {
      command_options_cleanup(opt->options);
    }
  }

  free(opts->options);
  free(opts);
}

int command_cmp(const struct command* a, const struct command* b) {
  return registry_strcmp(a->name, b->name);
}

void command_cleanup(struct command* elem) {
  free(elem->name);
  free(elem->description);
  free(elem->callback);
  command_options_cleanup(elem->options);
}
