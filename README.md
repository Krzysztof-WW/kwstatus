# kwstatus - asynchronous status monitor for dwm

Status monitor with instant updates based on internal events which should be
more reliable and portable than external signaling like
[dwmblocks](https://github.com/torrinfail/dwmblocks) do. 

# Usage

  `-d` enable dry mode which instead setting WM_NAME writes output to stdout

# Configuration

See config.h for options documentation and refer to each module description in
source files in modules/ directory.

In file kwstatus.h is macro MODSIZE which determines maximal length of one
module. When you see text for module is being cropped you can try increase
MODSIZE value.

# Add module

First thing to know is that every module have its own thread. And if your
modules returns from its function it is not going to be run again. But previous
set text is still going to be displayed unchanged.

To create module: copy modules/template.c to modules/_module_name_.c and change in this file function _name_ name to _module_name_ then add _module_name_ to modules.h and add modules/_module_name_.c to Makefile.
To update displayed string on status bar use function mod_update.
