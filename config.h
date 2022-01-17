#include "modules.h"

struct modules mdl[] = {
  {test, .num = 2, .str = "aa"},
  {test, .num = 3, .str = "bb"},
  {print, .str = "print test"},
};

/* bar will not update more frequently than align_ms, align_ms also adds delay to bar */
const unsigned int align_ms = 5;
