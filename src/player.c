#include "player.h"

#include "registry.h"
#include "regman.h"

#include <pthread.h>
#include <concord/discord.h>
#include <concord/discord-response.h>
#include <concord/user.h>
#include <stdlib.h>
#include <string.h>

#include "bot.h"
#include "log.h"

enum player_init_status {
  PLAYER_INIT_STATUS_IDLE,
  PLAYER_INIT_STATUS_WORKING,
  PLAYER_INIT_STATUS_DONE,
  PLAYER_INIT_STATUS_FAIL,
};

static enum player_init_status player_init_status = PLAYER_INIT_STATUS_IDLE;
static pthread_mutex_t player_init_lock = PTHREAD_MUTEX_INITIALIZER;

void player_init_done(struct discord* client, struct discord_response* resp, const struct discord_user* ret) {
  player_init_status = PLAYER_INIT_STATUS_DONE;
}

void player_init_fail(struct discord* client, struct discord_response* resp) {
  player_init_status = PLAYER_INIT_STATUS_FAIL;
}

struct player* player_init(unsigned long uuid) {
  pthread_mutex_lock(&player_init_lock);

  while (player_init_status != PLAYER_INIT_STATUS_IDLE);
  player_init_status = PLAYER_INIT_STATUS_WORKING;

  struct player* player = malloc(sizeof(struct player));
  player->uuid = uuid;
  if (registry_add(regman_get_player(), &player) == NULL) {
    log_error("Could not initialize player %zi", uuid);
    free(player);
    pthread_mutex_unlock(&player_init_lock);
    return NULL;
  }

  struct discord_user sync = {};
  struct discord_ret_user ret = {.done = player_init_done, .fail = player_init_fail, .sync = &sync};
  discord_get_user(bot_get_global()->discord_bot, uuid, &ret);

  while (player_init_status == PLAYER_INIT_STATUS_WORKING);

  if (player_init_status == PLAYER_INIT_STATUS_FAIL) {
    log_error("Failed to initialize player %zi", uuid);
    free(player);
    pthread_mutex_unlock(&player_init_lock);
    return NULL;
  }

  if (player->uuid != sync.id) {
    log_error("Given UUID does not match returned UUID (%zi)", player->uuid);
    free(player);
    pthread_mutex_unlock(&player_init_lock);
    return NULL;
  }

  player->username = sync.username;

  char* avatar = malloc(128);
  snprintf(avatar, 128, "https://cdn.discordapp.com/avatars/%zi/%s", uuid, sync.avatar);
  avatar = realloc(avatar, strlen(avatar) + 1);
  player->avatar = avatar;

  player_init_status = PLAYER_INIT_STATUS_IDLE;

  log_info("Initializing player %s (%zi)", player->username, player->uuid);
  pthread_mutex_unlock(&player_init_lock);
  return player;
}

struct player* player_get(unsigned long uuid) {
  struct player* key = &(struct player){.uuid = uuid};
  struct player** ret_ptr = registry_ktov(regman_get_player(), &key);
  if (ret_ptr == NULL) {
    return player_init(uuid);
  }
  return *ret_ptr;
}

int player_cmp(const void* a, const void* b) {
  const struct player** x = (void*)a;
  const struct player** y = (void*)b;

  if (*x == NULL && *y == NULL) return 0;
  else if (*x == NULL) return -1;
  else if (*y == NULL) return 1;

  if ((*x)->uuid > (*y)->uuid) return 1;
  else if ((*x)->uuid < (*y)->uuid) return -1;
  return 0;
}

void player_cleanup(void* elem) {
  struct player* player = *((struct player**)elem);
  free(player->username);
  free(player->avatar);
  free(player);
}
