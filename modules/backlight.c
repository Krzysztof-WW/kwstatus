#include "../kwstatus.h"
#include <stdio.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BCFILEBUF 20

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
  char bcstr[BCFILEBUF] = {0};
  char out[BCFILEBUF];
  unsigned int backlight;
  short icon;
  struct inotify_event event;


  bc_read = open(mod->str, O_RDONLY);
  if(bc_read == 0) {
    warn("Backlight cannot open file");
    return;
  }

  bc_watch = inotify_init();
  inotify_add_watch(bc_watch, mod->str, IN_MODIFY);

  while(1) {
    /* read backlight file */
    lseek(bc_read, 0, SEEK_SET);
    bcstr[read(bc_read, bcstr, BCFILEBUF)] = 0;

    /* text */
    backlight = atoi(bcstr);
    backlight = backlight*100/mod->num;
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

    snprintf(out, BCFILEBUF, "%s %d%%", icons[icon], backlight);

    mod_update(mod, out);

    /* wait for modification */
    read(bc_watch, &event, sizeof(struct inotify_event));
  }
}
