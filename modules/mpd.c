#include "../kwstatus.h"
#include <mpd/client.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PAUSED ""
#define PLAYING ""

char*
pretty_name(char* file) {
  char* out;
  size_t n, end;

  for(n = strlen(file); n != 0 && file[n] != '/'; n--) {
    if(file[n] == '.')
      end = n;
  }

  out = emalloc(end-n+1);
  out[end-n] = 0;
  memcpy(out, file+n+1, end-n-1);

  return out;
}

char*
get_song_name(struct mpd_song* song) {
  char* tag_title;
  char* tag_artist;
  char* out;

  tag_title = (char*)mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
  if(tag_title == NULL) {
    char *tmp;
    tmp = (char*)mpd_song_get_uri(song);
    tag_title = pretty_name(tmp);
    free(tmp);
  }

  tag_artist = (char*)mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);

  if(tag_artist) {
    out = smprintf("%s - %s", tag_artist, tag_title);
    free(tag_artist);
  } else
    out = smprintf("%s", tag_title);

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
  enum mpd_idle idle_event = 1;
  struct mpd_song* song;
  char* song_name;
  char* state_icon;
  unsigned int elapsed_ms, elapsed_s, elapsed_min;
  unsigned int total_s, total_min;

  conn = mpd_connection_new(NULL, 0, 0);
  if(conn == NULL) {
    warn("Cannot allocate memory for mpd connection");
    return;
  }
  if(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    warn("cannot connect to mpd server");
    mpd_connection_free(conn);
    return;
  }
  mpd_connection_set_timeout(conn, 1000);

  while(1) {
    /* check for errors */
    if(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
      if(mpd_connection_get_error(conn) == MPD_ERROR_CLOSED ||
          mpd_connection_get_error(conn) == MPD_ERROR_TIMEOUT) {
        //warn("lost connection\n");
        do {
          mpd_connection_free(conn);
          if(!(conn = mpd_connection_new(NULL, 0, 0))) {
            warn("out of memory\n");
            return;
          }
        } while(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS);
        mpd_connection_set_timeout(conn, 1000);
      } else {
        warn("mpd error, exiting\n");
        return;
      }
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

      if(idle_event != 0) {
        total_s = mpd_status_get_total_time(status);
        total_min = total_s / 60;
        total_s %= 60;

        /* get status icon */
        if(state == MPD_STATE_PLAY)
          state_icon = PLAYING;
        else if(state == MPD_STATE_PAUSE)
          state_icon = PAUSED;

        if(song_name != NULL)
          free(song_name);
        /* get song */
        song = mpd_run_current_song(conn);
        song_name = get_song_name(song);
        //mpd_song_free(song);
      }

      /* update text */
      snprintf(out, MODSIZE-1, "%d:%02d/%d:%02d %s %s", elapsed_min, elapsed_s, total_min, total_s, state_icon, song_name);
      mod_update(mod, out);

    } else {
      state_icon = NULL;

      out[0] = 0; /* when stopped show nothing */
      mod_update(mod, out);
    }
    mpd_status_free(status);

    /* wait for next event or if playing to the next second */
    if(state_icon == NULL) {
      mpd_send_idle_mask(conn, MPD_IDLE_PLAYER);
      idle_event = mpd_recv_idle(conn, 1);
    } else {
      mpd_send_idle_mask(conn, MPD_IDLE_PLAYER | MPD_IDLE_OPTIONS);
      idle_event = mpd_recv_idle(conn, 0);
    }
  }
}
