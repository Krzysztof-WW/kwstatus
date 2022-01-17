#include "modules.h"

struct modules mdl[] = {
  {test, .num = 2, .str = "aa"},
  {test, .num = 3, .str = "bb"},
};

/* wait to align parallel modules with similar timings to avoid unnecessary updates */
const unsigned int align_ms = 5;
