#include "../kwstatus.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define FILEBUF 20
#define PATHBUF 100

#define LEVEL "/sys/class/power_supply/BAT0/capacity"
#define STATUS "/sys/class/power_supply/BAT0/status"

#define CHARGING "🔌"

static char* bat_icons[] = {
  "",
  "",
  "",
  "",
  "",
};

void
battery(void* self) {
  struct modules* mod = (struct modules*)self;
  int fd_level, fd_status;
  char level[FILEBUF+1], status[FILEBUF+1];
  int level_num;
  char* icon;
  char out[FILEBUF+1];

  /* open level file */
  fd_level = open(LEVEL, O_RDONLY);
  if(fd_level == -1) {
    warn("Battery cannot open level");
    return;
  }

  /* open status file */
  fd_status = open(STATUS, O_RDONLY);
  if(fd_status == -1) {
    warn("Battery cannot open status");
    return;
  }

  while(1) {
    /* read level file */
    lseek(fd_level, 0, SEEK_SET);
    read(fd_level, level, FILEBUF); //no zero
    level_num = atoi(level);

    /* read status file */
    lseek(fd_status, 0, SEEK_SET);
    read(fd_status, status, FILEBUF);

    if(status[0] == 'C')
      icon = CHARGING;
    else {
      if(level_num > 80)
        icon = bat_icons[0];
      else if(level_num > 60)
        icon = bat_icons[1];
      else if(level_num > 40)
        icon = bat_icons[2];
      else if(level_num > 20)
        icon = bat_icons[3];
      else
        icon = bat_icons[4];
    }

    snprintf(out, FILEBUF, "%s %d%%", icon, level_num);
    mod_update(mod, out);

    sleep(mod->num);
  }
}
