#include "keyboard_map.h"
#include "io.h"
#include "shell.h"

void boot(void) {
    // splash
    char *splash = "Mustard kernel v0.9";
    clear_screen();
	printa(splash, ATTR_ACC_REV);
	nl();
    np();

    // initialize IDT for responses to interrupts and exceptions
    idt_init();
    // initialize driver for keyboard
    keyboard_init();
    // draw cursor
	update_cursor_graphic();

    // event loop
	while(1);
}
