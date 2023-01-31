#include "modules.h"

/* refer to each module documentation in its source file to see what parameters it accepts. */
struct modules mdl[] = {
  /* function, .str | .num | .no_delim | .icon */
  /* when .no_delim is set to 1 then no delimeter is added between this and next module */
  /* icon adds icon before text from module */
  //{mpd}, // to use, enable MPD in config.mk
  {battery, .str = "BAT0", .num = 3},
  {pulse},
  {backlight, .str = "intel_backlight"},
  {disk, .str = "/", .num = 80, .icon = "💾 "},
  {run_command, .str = "date '+%a %m.%d'", .num = 240, .icon = "📅 "},
  {clockm, .str = "%i %02H:%02M:%02S", .num = 1},
  {interface, .str = "wlan0", .icon = " "},
};

/* bar will not update more frequently than align_ms, align_ms also adds delay to bar */
const unsigned int align_ms = 5;

/* delimiter between modules */
const char delim[] = "  ";
