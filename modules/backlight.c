/* Copyright (C) 2022 Krzysztof Wolski
 * See COPYING for license details */

/*  brightness module
    
    This module shows current brightness of display in laptop.

    Configuration:
      .str - backlight name

    Backlight names can be acquired with command `ls /sys/class/backlight`
*/

#include "../kwstatus.h"
#include <stdio.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define BRIGHTNES_ROOT "/sys/class/backlight/"
#define BRIGHTNESS "/brightness"
#define MAX_BRIGHTNESS "/max_brightness"

#define FILEBUF 20
#define PATHBUF 100

static const char* icons[] = {
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
  char file[PATHBUF+1] = BRIGHTNES_ROOT;
  int file_end;
  char bcstr[FILEBUF+1] = {0};
  char out[MODSIZE];
  unsigned int backlight, max_brightness;
  short icon;
  struct inotify_event event;

  /* get path */
  strncat(file, mod->str, PATHBUF);
  file_end = strlen(file);

  /* open max_backlight */
  strncat(file, MAX_BRIGHTNESS, PATHBUF);
  bc_read = open(file, O_RDONLY);
  if(bc_read == -1) {
    fputs("backlight: cannot open max_brightness file\n", stderr);
    return;
  }
  /* read max_backlight */
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

  /* setup inotify */
  bc_watch = inotify_init();
  inotify_add_watch(bc_watch, file, IN_MODIFY);

  while(1) {
    /* read backlight file */
    lseek(bc_read, 0, SEEK_SET);
    bcstr[read(bc_read, bcstr, FILEBUF)] = 0;

    /* generate text */
    backlight = atoi(bcstr);
    backlight = round(backlight*100./max_brightness);
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

    snprintf(out, MODSIZE - 1, "%s %d%%", icons[icon], backlight);
    mod_update(mod, out);

    /* wait for modifications */
    read(bc_watch, &event, sizeof(struct inotify_event));
  }
}
