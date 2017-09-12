/**
 * Veryyy basic kernel
 */

void kmain(void) {
    const char *str = "Hello, world!";
    char *vid = (char*) 0xb8000;
    unsigned int i = 0, j = 0;

    for (unsigned int vidpos = 0; vidpos < 80 * 25 * 2; vidpos += 2) {
        vid[vidpos] = ' ';
        vid[vidpos + 1] = 0x0f;
    }

    for (unsigned int strpos = 0, vidpos = 0; str[strpos] != 0; strpos++) {
        vid[vidpos] = str[strpos];
        vid[vidpos + 1] = 0x0f;
        vidpos += 2;
    }
    return;
}