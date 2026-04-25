#include "player.h"

#include "registry.h"
#include "regman.h"

#if 1
// this is so this is included first and clang format doesnt move it
#include <concord/discord.h>
#endif

#include <concord/discord-response.h>
#include <concord/user.h>
#include <stdlib.h>
#include <string.h>

#include "bot.h"
#include "log.h"

#define UUID_CHAR_MAX_LENGTH 128

enum player_init_status {
  PLAYER_INIT_STATUS_IDLE,
  PLAYER_INIT_STATUS_WORKING,
  PLAYER_INIT_STATUS_DONE,
  PLAYER_INIT_STATUS_FAIL,
};

static enum player_init_status player_init_status = PLAYER_INIT_STATUS_IDLE;

void player_init_done(struct discord* client, struct discord_response* resp,
                      const struct discord_user* ret) {
  player_init_status = PLAYER_INIT_STATUS_DONE;
}

void player_init_fail(struct discord* client, struct discord_response* resp) {
  player_init_status = PLAYER_INIT_STATUS_FAIL;
}

struct player* player_init(unsigned long uuid) {
  while (player_init_status != PLAYER_INIT_STATUS_IDLE);
  player_init_status = PLAYER_INIT_STATUS_WORKING;

  struct player* player = malloc(sizeof(struct player));
  player->uuid = uuid;
  if (registry_add(regman_get_player(), player) == NULL) {
    log_error("Could not initialize player %zi", uuid);
    free(player);
    return NULL;
  }

  // if (sync == NULL) {
  //   struct discord_ret_user disc = {.done = player_init_callback};
  //   discord_get_user(bot_get_global()->discord_bot, uuid, &disc);

  //   char* uuid_str = malloc(UUID_CHAR_MAX_LENGTH);
  //   snprintf(uuid_str, UUID_CHAR_MAX_LENGTH, "%zi", uuid);
  //   uuid_str = realloc(uuid_str, strlen(uuid_str));

  //   return player;

  struct discord_user sync = {};
  struct discord_ret_user ret = {
      .done = player_init_done, .fail = player_init_fail, .sync = &sync};
  discord_get_user(bot_get_global()->discord_bot, uuid, &ret);

  while (player_init_status == PLAYER_INIT_STATUS_WORKING);

  if (player_init_status == PLAYER_INIT_STATUS_FAIL) {
    log_error("Failed to initialize player %zi", uuid);
    free(player);
    return NULL;
  }

  if (player->uuid != sync.id) {
    log_error("Given UUID does not match returned UUID (%zi)", player->uuid);
    free(player);
    return NULL;
  }

  player->username = sync.username;
  player->avatar = sync.avatar;

  player_init_status = PLAYER_INIT_STATUS_IDLE;

  return player;
}

int player_cmp(const void* a, const void* b) {
  const struct player* x = a;
  const struct player* y = b;
  if (x > y)
    return 1;
  else if (x < y)
    return -1;
  return 0;
}

void player_cleanup(void* elem) {
  struct player* player = elem;
  free(player->username);
  free(player->avatar);
  free(player);
}
