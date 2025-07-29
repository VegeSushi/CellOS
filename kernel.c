#include <stdint.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25

volatile char* video_memory = (char *)0xb8000;

static unsigned int next = 1;

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void print(char* message, int col, int row, unsigned char color) {
    int offset = (row * 80 + col) * 2;

    for (int i = 0; message[i] != '\0'; i++) {
        video_memory[offset + i * 2] = message[i];
        video_memory[offset + i * 2 + 1] = color;
    }
}

int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
    next = seed;
}

unsigned char get_time() {
    outb(0x70, 0x00);
    return inb(0x71);
}

char wait_for_keypress() {
    unsigned char scancode = 0;
    while (1) {
        scancode = inb(0x60);
        if (scancode < 0x80) {
            return scancode;
        }
    }
}

void clear() {
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x01;
    }
}

void delay(int count) {
    for (volatile int i = 0; i < count; i++) {
    }
}

// Automatas logic

// Conway

int count_neighbors(int current[HEIGHT][WIDTH], int row, int col) {
    int count = 0;
    for(int i = row - 1; i <= row + 1; i++) {
        for(int j = col - 1; j <= col + 1; j++) {
            if(i == row && j == col) continue; // skip self
            if(i >= 0 && i < HEIGHT && j >= 0 && j < WIDTH) {
                count += current[i][j];
            }
        }
    }
    return count;
}

void conway() {
    int current[HEIGHT][WIDTH] = {0};
    int next[HEIGHT][WIDTH] = {0};

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            current[y][x] = rand() % 2;
        }
    }

    while (1) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (current[y][x] == 1) {
                    print("#", x, y, 0x02);
                } else {
                    print("#", x, y, 0x04);
                }
            }
        }

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                int neighbors = count_neighbors(current, y, x);

                if (current[y][x] == 1) {
                    if (neighbors == 2 || neighbors == 3) {
                        next[y][x] = 1;
                    } else {
                        next[y][x] = 0;
                    }
                } else {
                    if (neighbors == 3) {
                        next[y][x] = 1;
                    } else {
                        next[y][x] = 0;
                    }
                }
            }
        }

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                current[y][x] = next[y][x];
            }
        }

        delay(10000000);
    }
}

// End Conway

// Langton's Ant

void langtons_ant() {
    int grid[HEIGHT][WIDTH] = {0};
    int dir = 0;

    int x = WIDTH / 2;
    int y = HEIGHT / 2;

    while (1) {
        for (int row = 0; row < HEIGHT; row++) {
            for (int col = 0; col < WIDTH; col++) {
                if (row == y && col == x) {
                    print("#", col, row, 0x14);
                } else if (grid[row][col] == 0) {
                    print("#", col, row, 0x38);
                } else {
                    print("#", col, row, 0x3F);
                }
            }
        }

        if (grid[y][x] == 0) {
            dir = (dir + 1) % 4;
            grid[y][x] = 1;
        } else {
            dir = (dir + 3) % 4;
            grid[y][x] = 0;
        }

        if (dir == 0) y = (y - 1 + HEIGHT) % HEIGHT;
        else if (dir == 1) x = (x + 1) % WIDTH;
        else if (dir == 2) y = (y + 1) % HEIGHT;
        else if (dir == 3) x = (x - 1 + WIDTH) % WIDTH;

        delay(10000000);
    }
}

// End Langton's Ant

// End automatas

// Kernel

void kernel_main() {
    clear();
    print("Select an automata:", 0, 0, 0x0F);
    print("1. Conway's Game Of Life", 0, 1, 0x07);
    print("2. Langton's Ant", 0, 2, 0x07);

    char key;

    while (1) {
        key = wait_for_keypress();

        if (key == 0x02 || key == 0x03) {
            break;
        }
    }

    int seed = get_time();
    srand(seed);
    clear();

    if (key == 0x02) {
        conway();
    } else if (key == 0x03) {
        langtons_ant();
    }
}
