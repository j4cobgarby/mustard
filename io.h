#ifndef IO_H
#define IO_H

#define LINES 25
#define COLUMNS 80
#define BYTES_PER_VID_ELEMENT 2
#define SCREENSIZE BYTES_PER_VID_ELEMENT * COLUMNS * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

// Attributes

#define ATTR_NORMAL 0x07 // Grey on black
#define ATTR_ACCENT 0x0e // Yellow on black
#define ATTR_REVERSE 0x70 // Reversed normal
#define ATTR_ACC_REV 0xe0 // Reversed accent
#define ATTR_BRIGHT 0x0f // White on black
#define ATTR_BRI_REV 0xf0 // Reversed bright

extern const unsigned char keyboard_map[128];
extern const unsigned char keyboard_map_shift[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

// Location of the cursor, to write as an offset of vidptr
unsigned int current_loc = 0;

// Video memory starts at this address
char *vidptr = (char*) 0xb8000;


struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

/*
 * Function declerations, all defined below
 */

void idt_init(void);
void keyboard_init(void);
void printa(const char *str, const unsigned short int attr);
void printca(const unsigned char ch, const unsigned short int attr);
void print(const char *str);
void printc(const unsigned char ch);
void nl(void);
void clear_screen(void);
int index2x(const unsigned int index);
void update_cursor_graphic();
//void scrollup(const unsigned short int lines);

/*
 * Function implementations
 */ 

void idt_init(void) {
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

    // IDT for keyboard
    keyboard_address = (unsigned long) keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	// ICW1 to begin initialization
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	// ICW2
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	// ICW3
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

    // ICW4, which is the environment info
    write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
    // Initialization finished

	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

/**
 * Initializes the keyboard so the user can type
 */
void keyboard_init(void) {
    // 0x21 is the keyboard thing in the IDT
    // 0xFD enables keyboard input only (which is IRQ1)
	write_port(0x21 , 0xfd);
}

/** Prints at the cursor location with a particular attribute */
void printa(const char *str, const unsigned short int attr) {
    /** Attribute colours:
     * 0 - Black, 1 - Blue, 2 - Green, 3 - Cyan, 4 - Red,
     * 5 - Magenta, 6 - Brown, 7 - Light Grey, 8 - Dark Grey,
     * 9 - Light Blue, 10/a - Light Green, 11/b - Light Cyan,
     * 12/c - Light Red, 13/d - Light Magenta, 14/e - Light Brown,
     * 15/f – White.
     *
     * Notes:
     * Light brown is literally just yellow
     */

    unsigned int i = 0;

    /**
     * Stop when a null character is found, since strings are null-terminated
     */
	while (str[i] != '\0') {
        /** Add the character to the video memory */
        vidptr[current_loc++] = str[i++];
        /** Set the attributes of that character */
		vidptr[current_loc++] = attr;
	}
}

void printca(const unsigned char ch, const unsigned short int attr) {
    vidptr[current_loc++] = ch;
    vidptr[current_loc++] = attr;
}

/** Prints something at the current cursor location */
void print(const char *str) {
    printa(str, ATTR_NORMAL);
}

void printc(const unsigned char ch) {
    printca(ch, ATTR_NORMAL);
}

/** Moves the cursor to the next line and shows a prompt */
void nl(void) {
	unsigned int line_size = BYTES_PER_VID_ELEMENT * COLUMNS;
    current_loc = current_loc + (line_size - current_loc % (line_size));
}

/** Basically sets everything in the visible video memory to a space */
void clear_screen(void) {
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = ATTR_NORMAL;
	}
}

/** Get the x-coordinate of the cursor based on its position in the video memory */
int index2x(const unsigned int index) {
    return index/2 % COLUMNS;
}

/**
 * Call this every time the cursor moves to update the visible cursor
 */
void update_cursor_graphic() {
    /**
     * Use i to iterate the characters of the screen, and their attributes
     */
	unsigned int i = 1;
	while (i < SCREENSIZE) {
		if (vidptr[i] == ATTR_REVERSE) {
			vidptr[i] = ATTR_NORMAL;
		}
		i += 2;
	}
	vidptr[current_loc + 1] = ATTR_REVERSE;
}

void scroll(int lines) {
    /**
     * If the amount of lines to scroll is 25, everything will be scrolled out of view, so
     * it may as well just clear the screen
     */
    if (lines == 25) {
        clear_screen();
        return;
    }

    /**
     * Calculate the offset which will be how far characters and the cursor are shifted back
     */
    int offset = lines * COLUMNS * BYTES_PER_VID_ELEMENT;

    /**
     * For every character (and its attribute) on the screen ..
     */
    for (int va = 0; va < SCREENSIZE; va++) {
        /** 
         * If the current address + the offset is on the screen, that means that a character is
         * being moved, as opposed to new blank lines being created at the bottom
         */
        if (va + offset < SCREENSIZE) {
            char temp = vidptr[va + offset];
            vidptr[va] = temp;
        } else {
            /**
             * Otherwise create a blank line at the bottom
             */
            vidptr[va] = (va % 2 == 0 ? ' ' : ATTR_NORMAL);
        }
    }

    /**
     * Move cursor up by the offset and update it visually
     */
    current_loc -= (current_loc - offset >= 0 ? offset : current_loc);
    update_cursor_graphic();
}

#endif