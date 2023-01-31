# kwstatus - asynchronous status monitor for dwm

Status monitor with instant updates based on internal events which should be
more reliable and portable than external signaling like
[dwmblocks](https://github.com/torrinfail/dwmblocks) do. 

# Usage

  `-d` enable dry mode which instead setting WM_NAME writes output to stdout

# Configuration

See *config.h* and *config.mk*. For each module description refer to description in
their source files in modules/ directory.

# Dependencies

  * xcb
  * libpulse (for pulseaudio support, optional)
  * libmpdclient (for mpd support, optional)

To enable or disable modules, see config.mk.

# Add module

First thing to know is that every module have its own thread. And if your
modules returns from its function it is not going to be run again. But previous
set text is still going to be displayed unchanged.

To create module: copy modules/template.c to modules/_module_name_.c and change
in this file function _name_ name to _module_name_ then add _module_name_ to
modules.h and add modules/_module_name_.c to Makefile. To update displayed
string on status bar use function mod_update.
