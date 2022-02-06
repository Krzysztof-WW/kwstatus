/*  battery module

    Monitor battery and do actions on events

    Configuration:
      .str - battery name
      .num - refresh time (s)
      See other macros below

    Default macros are set to send notifications on battery level events and
    delete notifications when they are not needed anymore.
*/

/* icon when battery is charging */
#define CHARGING_ICON "🔌"
/* icon when battery is full */
#define FULL_ICON ""

/* command to execute when battery starts charging */
#define CHARGING_CMD "dunstify -C 122; dunstify -C 123"
/* command to execute when battery starts discharging */
#define DISCHARGING_CMD "dunstify -C 121"

/* battery level in % this option is for batteries that stop charging and do
 * not indicate they are full, otherwise set it to 100 */
#define FULL_CHARGE 90
/* command to execute when battery indicate is full or reach FULL_CHARGE charge */
#define FULL_CHARGE_CMD "dunstify -r 121 'Battery is full'"

/* first step in low charge */
#define LOW_CHARGE 20
#define LOW_CHARGE_CMD "dunstify -r 122 -u critical 'Battery is low'"

/* last warning in low charge */
#define CRITICAL_CHARGE 15
#define CRITICAL_CHARGE_CMD "dunstify -r 123 -u critical 'Battery on critical level!'"

/* end of configuration */

#include "../kwstatus.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FILEBUF 100
#define BATTERY_ROOT "/sys/class/power_supply/"
#define LEVEL "/capacity"
#define STATUS "/status"

static const char* bat_icons[] = {
  "",
  "",
  "",
  "",
  "",
};

void
battery(void* self) {
  struct modules* mod = (struct modules*)self;
  FILE* flevel, *fstatus;
  char level[MODSIZE+1], status[MODSIZE+1];
  int level_num, old_level = -1, old_status = -1;
  int low_set = 0, critical_set = 0, full_set = 0,
      charging_set = 0, discharging_set = 0;
  const char* icon;
  char out[MODSIZE+1];
  char file[FILEBUF+1] = BATTERY_ROOT;
  size_t file_end;

  /* get path */
  strncat(file, mod->str, FILEBUF);
  file_end = strlen(file);

  /* open level file */
  strncpy(file+file_end, LEVEL, FILEBUF-file_end);
  flevel = fopen(file, "r");
  if(flevel == NULL) {
    fputs("battery: cannot open level\n", stderr);
    return;
  }
  file[file_end] = 0;

  /* open status file */
  strncat(file+file_end, STATUS, FILEBUF-file_end);
  fstatus = fopen(file, "r");
  if(fstatus == NULL) {
    fputs("battery: cannot open status\n", stderr);
    return;
  }

  while(1) {
    /* read level file */
    fseek(flevel, 0, SEEK_SET);
    fread(level, 1, MODSIZE, flevel); //no zero
    level_num = atoi(level);

    /* read status file */
    fseek(fstatus, 0, SEEK_SET);
    fread(status, 1, MODSIZE, fstatus);

    if(old_status != status[0] || old_level != level_num) {
      old_status = status[0]; old_level = level_num;

      if(status[0] == 'F' || level_num >= FULL_CHARGE) {
        icon = FULL_ICON;
        discharging_set = 0;

        if(!full_set) {
          full_set = 1;
          system(FULL_CHARGE_CMD);
        }
      } else if(status[0] == 'C') {
        icon = CHARGING_ICON;
        discharging_set = 0;
        critical_set = 0;
        low_set = 0;

        if(!charging_set) {
          charging_set = 1;
          system(CHARGING_CMD);
        }
      } else {
        full_set = 0;
        charging_set = 0;

        /* run cmds */
        if(!discharging_set) {
          discharging_set = 1;
          system(DISCHARGING_CMD);
        }
        if(level_num <= CRITICAL_CHARGE && !critical_set) {
          critical_set = 1;
          system(CRITICAL_CHARGE_CMD);
        } else if(level_num <= LOW_CHARGE && !low_set) {
          low_set = 1;
          system(LOW_CHARGE_CMD);
        }

        /* set icon */
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

      snprintf(out, MODSIZE, "%s %d%%", icon, level_num);
      mod_update(mod, out);
    }

    sleep(mod->num);
  }
}
