kwstatus is asynchronous bar management program created for window managers
like DWM.
Each module in bar have its own thread and works independently from others. It
allow modules to wait until some event and then update.

Creating modules
Add module source file in modules/ add module name in modules.h file and add
module file name in Makefile.
Every module should include kwstatus.h and main function must be 
void name(void*)
Every module to update its text on bar just calls mod_update function from
kwstatus.h
