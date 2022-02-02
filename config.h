#include "modules.h"

struct modules mdl[] = {
  {battery, .num = 3},
  {pulse},
  {backlight, .str = "intel_backlight"},
  {run_command, .str = "printf '📅 '; date '+%a %m.%d'", .num = 60, .no_delim = 1},
  {print, .str = ";;", .no_delim = 1},
  {mpd},
  {clockm, .num = 1, .no_delim = 1},
};

/* bar will not update more frequently than align_ms, align_ms also adds delay to bar */
const unsigned int align_ms = 5;

/* delimiter between modules */
const char delim[] = "  ";
