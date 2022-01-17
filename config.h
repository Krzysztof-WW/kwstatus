#include "modules.h"

struct modules mdl[] = {
  {test, .num = 2, .str = "aa"},
  {test, .num = 3, .str = "bb"},
  {print, .str = ";;", .no_delim = 1},
  {clockm, .num = 1},
};

/* bar will not update more frequently than align_ms, align_ms also adds delay to bar */
const unsigned int align_ms = 5;

/* delimiter between modules */
const char delim[] = "  ";
