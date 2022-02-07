/*  print module

    Prints static text on status bat

    Configuration:
      .str - text

*/
#include "../kwstatus.h"

void
print(void* self) {
  struct modules* mod = (struct modules*)self;
  mod_update(mod, mod->str);
}
