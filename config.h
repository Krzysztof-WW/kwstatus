#include "modules.h"

struct modules mdl[] = {
  {battery, .str = "BAT0", .num = 3},

  {pulse},

  {backlight, .str = "intel_backlight"},

  {print, .str = "📅 ", .no_delim = 1},
  {run_command, .str = "date '+%a %m.%d'", .num = 240, .no_delim = 1},

  {print, .str = ";;", .no_delim = 1},
  {mpd},
  {clockm, .str = "%i %02H:%02M:%02S", .num = 1, .no_delim = 1},
};

/* bar will not update more frequently than align_ms, align_ms also adds delay to bar */
const unsigned int align_ms = 5;

/* delimiter between modules */
const char delim[] = "  ";
