#ifndef SHELL_H
#define SHELL_H

#include "io.h"

#define ENTER_KEY_CODE 0x1C

#define COMMAND_MAX_SIZE 1024

const unsigned int prompt_length = 10; // amount of characters in the prompt
char command[COMMAND_MAX_SIZE];

short int shift_down = 0;

void execute(const char *command) {
    
}

/** Convenience for newline then writing the prompt */
void np(void) {
    for (int i = 0; i < COMMAND_MAX_SIZE; i++) {
        command[i] = 0; // clear command
    }
    printa("<mustard> ", 0x0e);
}

/** Handles keyboard input, called from kernel.asm */
void keyboard_handler_main(void) {
    //keyboard_init();
    unsigned char status;

    /** Will be set to the keycode of the key which is pressed, if a key is pressed */
    char keycode;

    /** Sending end-of-interrup acknowledgement */
    write_port(0x20, 0x20);

    /** Read the status from the keyboard status port */
    status = read_port(KEYBOARD_STATUS_PORT);

    /** If the lowest bit of the status is 0, the buffer must be empty so there isn't any data,
     * so only continue with this if there is actually a key pressed
     */
    if (status & 0x01) {
        keycode = read_port(KEYBOARD_DATA_PORT);

        if (keycode < 0 || keyboard_map[keycode] == 0)
            return;

        if (keycode == ENTER_KEY_CODE) {
            // submit command
            nl();
            for (int i = 0; i < COMMAND_MAX_SIZE; i++) {
                if (command[i] != 0) {
                    printca(command[i], 0x0f);
                }
            }
            nl();
            np();
            update_cursor_graphic();
            return;
        }

        if (keycode == 14) { // Backspace
            // TODO: Make backspace pop the last character from the command array
            if (index2x(current_loc) > prompt_length) {
                current_loc -= 2;
                vidptr[current_loc] = ' ';
                vidptr[current_loc + 1] = 0x07;
            }
            update_cursor_graphic();
            return;
        }
        
        if (keycode == 42 || keycode == 54) {
            /**
             * Shift key pressed (right OR left)
             */
            shift_down = !shift_down;
            update_cursor_graphic();
            return;
        }

        if (keycode == 80) {
            /**
             * Down arrow
             */
            scroll(1);
            return;
        }

        /** If this code is executed, it means the keycode is >= 0, and
         * it's not one of the special keys which do something different.
         * In this case, simply write the character to the screen
         */
        if (shift_down) {
            vidptr[current_loc++] = keyboard_map_shift[(unsigned char) keycode];
            for (int i = 0; i < COMMAND_MAX_SIZE; i++) {
                if (command[i] == 0) {
                    if (i >= COMMAND_MAX_SIZE - 1) break;
                    command[i] = keyboard_map_shift[(unsigned char) keycode];
                    break;
                }
            }
        } else {
            vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
            for (int i = 0; i < COMMAND_MAX_SIZE; i++) {
                if (command[i] == 0) {
                    if (i >= COMMAND_MAX_SIZE - 1) break;
                    command[i] = keyboard_map[(unsigned char) keycode];
                    break;
                }
            }
        }
        vidptr[current_loc++] = 0x07;
    }

    update_cursor_graphic();
}

#endif