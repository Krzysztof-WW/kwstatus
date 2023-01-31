/* Copyright (C) 2022 Krzysztof Wolski
 * See COPYING for license details */

/*  mpd module

    shows current playing song and its status
    Configuration:
      .str - mpd server hostname (do not set to use default)
      .num - port (do not set to use default)
      other macros (see below)
*/
/* icon to use if song is paused */
#define PAUSED ""
/* icon to use if song is playing */
#define PLAYING ""

/* icon to use when repeat mode is set */
#define REPEAT "🔁 "
/* icon to use when random mode is set */
#define RANDOM "🔀 "

/* time between attempts to reconnect when lost connection */
#define CONNECTION_RETRY_TIME 5
/* end of configuration */

#include "../kwstatus.h"
#include <stdio.h>
#include <mpd/client.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

static char*
pretty_name(const char* file) {
  char* out;
  size_t n, end, len;

  end = len = strlen(file);

  for(n = len; n != 0 && file[n] != '/'; n--) {
    if(file[n] == '.' && end == len)
      end = n;
  }

  out = emalloc(end-n);
  out[end-n-1] = 0;
  memcpy(out, file+n+1, end-n-1);

  return out;
}

static char*
get_song_name(struct mpd_song* song) {
  char* tag_title;
  char* tag_artist;
  char* out;
  int tag_title_generated = 0;

  tag_title = (char*)mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
  if(tag_title == NULL) {
    const char *tmp;
    tmp = mpd_song_get_uri(song);
    tag_title = pretty_name(tmp);

    tag_title_generated = 1;
  }

  tag_artist = (char*)mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);

  if(tag_artist) {
    out = smprintf("%s - %s", tag_artist, tag_title);
  } else
    out = smprintf("%s", tag_title);

  if(tag_title_generated)
    free(tag_title);

  return out;
}

void
mpd(void* self) {
  struct modules* mod = (struct modules*)self;
  char out[MODSIZE];
  struct mpd_connection* conn;
  struct mpd_status* status;
  enum mpd_state state;
  enum mpd_idle idle_event = MPD_IDLE_PLAYER | MPD_IDLE_OPTIONS;
  struct mpd_song* song;
  char* song_name = NULL;
  const char* state_icon = NULL, *random_icon = NULL, *repeat_icon = NULL;
  unsigned int elapsed_ms, elapsed_s, elapsed_min;
  unsigned int total_s = 0, total_min = 0;
  struct pollfd mpd_poll;
  mpd_poll.events = POLLIN;

  conn = mpd_connection_new(mod->str, mod->num, 0);
  if(conn == NULL) {
    fputs("mpd: Cannot allocate memory for mpd connection\n", stderr);
    return;
  }
  if(mpd_connection_get_error(conn) == MPD_ERROR_SUCCESS)
    mpd_poll.fd = mpd_connection_get_fd(conn);

  while(1) {
    /* check for errors */
    if(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
      out[0] = 0;
      mod_update(mod, out);
      do {
        mpd_connection_free(conn);
        sleep(CONNECTION_RETRY_TIME);
        if(!(conn = mpd_connection_new(mod->str, mod->num, 0))) {
          fputs("mpd: Cannot allocate memory for mpd connection\n", stderr);
          return;
        }
      } while(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS);
      mpd_poll.fd = mpd_connection_get_fd(conn);
    }

    /* get state */
    status = mpd_run_status(conn);
    state = mpd_status_get_state(status);
    if(state == MPD_STATE_PLAY || state == MPD_STATE_PAUSE) {
      /* get times */
      elapsed_ms = mpd_status_get_elapsed_ms(status);
      elapsed_s = (float)elapsed_ms/1000;
      elapsed_min = elapsed_s / 60;
      elapsed_s %= 60;

      if(idle_event & MPD_IDLE_PLAYER) {
        total_s = mpd_status_get_total_time(status);
        total_min = total_s / 60;
        total_s %= 60;

        /* get status icon */
        if(state == MPD_STATE_PLAY)
          state_icon = PLAYING;
        else
          state_icon = PAUSED;

        if(song_name != NULL)
          free(song_name);
        /* get song */
        song = mpd_run_current_song(conn);
        song_name = get_song_name(song);
        mpd_song_free(song);
      }

      if(idle_event & MPD_IDLE_OPTIONS) {
        repeat_icon = mpd_status_get_repeat(status) ? REPEAT : "";
        random_icon = mpd_status_get_random(status) ? RANDOM : "";
      }

      /* update text */
      snprintf(out, MODSIZE-1, "%d:%02d/%d:%02d %s%s%s %s",
          elapsed_min, elapsed_s, total_min, total_s,
          repeat_icon, random_icon, state_icon,
          song_name);
      mod_update(mod, out);

    } else {
      state_icon = NULL;

      out[0] = 0; /* when stopped show nothing */
      mod_update(mod, out);
    }
    mpd_status_free(status);

    /* wait for next event or if playing wait second */
    if(state_icon == NULL) {
      mpd_send_idle_mask(conn, MPD_IDLE_PLAYER);
      idle_event = mpd_recv_idle(conn, 1);
      if(idle_event) /* also update options because we do not know if any ware changed */
        idle_event = MPD_IDLE_PLAYER | MPD_IDLE_OPTIONS;
    } else {
      mpd_send_idle_mask(conn, MPD_IDLE_PLAYER | MPD_IDLE_OPTIONS);
      if(state == MPD_STATE_PAUSE)
        idle_event = mpd_recv_idle(conn, 1);
      else {
        if(poll(&mpd_poll, 1, 1000)) {
          idle_event = mpd_recv_idle(conn, 1);
        } else {
          idle_event = mpd_run_noidle(conn);
        }
      }
    }
  }
}
