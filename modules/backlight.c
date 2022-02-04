#include "../kwstatus.h"
#include <stdio.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define FILEBUF 20
#define PATHBUF 100

#define BRIGHTNES_DIR "/sys/class/backlight/"
#define BRIGHTNESS "/brightness"
#define MAX_BRIGHTNESS "/max_brightness"

static char* icons[] = {
  "🌑",
  "🌒",
  "🌓",
  "🌔",
  "🌕",
};

void
backlight(void* self) {
  struct modules* mod = (struct modules*)self;
  int bc_watch, bc_read;
  char file[PATHBUF+1] = BRIGHTNES_DIR;
  int file_end;
  char bcstr[FILEBUF+1] = {0};
  char out[FILEBUF+1];
  unsigned int backlight, max_brightness;
  short icon;
  struct inotify_event event;

  /* get path */
  strncat(file, mod->str, PATHBUF);
  file_end = strlen(file);

  /* read max_backlight */
  strncat(file, MAX_BRIGHTNESS, PATHBUF);
  bc_read = open(file, O_RDONLY);
  if(bc_read == -1) {
    fputs("backlight: cannot open max_brightness file\n", stderr);
    return;
  }
  read(bc_read, bcstr, FILEBUF);
  max_brightness = atoi(bcstr);
  close(bc_read);

  /* open brightness file */
  file[file_end] = 0;
  strncat(file, BRIGHTNESS, PATHBUF);
  bc_read = open(file, O_RDONLY);
  if(bc_read == 0) {
    fputs("backlight: cannot open file\n", stderr);
    return;
  }

  bc_watch = inotify_init();
  inotify_add_watch(bc_watch, file, IN_MODIFY);

  while(1) {
    /* read backlight file */
    lseek(bc_read, 0, SEEK_SET);
    bcstr[read(bc_read, bcstr, FILEBUF)] = 0;

    /* text */
    backlight = atoi(bcstr);
    backlight = backlight*100/max_brightness;
    if(backlight > 80)
      icon = 0;
    else if(backlight > 60)
      icon = 1;
    else if(backlight > 40)
      icon = 2;
    else if(backlight > 20)
      icon = 3;
    else
      icon = 4;

    snprintf(out, FILEBUF, "%s %d%%", icons[icon], backlight);

    mod_update(mod, out);

    /* wait for modification */
    read(bc_watch, &event, sizeof(struct inotify_event));
  }
}
