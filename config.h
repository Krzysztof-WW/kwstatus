#include "modules.h"
#include "kwstatus.h"

struct modules mdl[] = {
  {backlight, .str = "/sys/class/backlight/intel_backlight/brightness", .num = 1200},
  {print, .str = ";;", .no_delim = 1},
  {clockm, .num = 1},
};

/* bar will not update more frequently than align_ms, align_ms also adds delay to bar */
const unsigned int align_ms = 5;

/* delimiter between modules */
const char delim[] = "  ";
