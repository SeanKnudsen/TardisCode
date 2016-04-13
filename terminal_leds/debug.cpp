#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>
#include <signal.h>
#include <chrono>

#include "../tardis/color.h"

#pragma clang diagnostic ignored "-Wc++11-extensions"

volatile sig_atomic_t interrupted = 0;
void interrupt_handler(int sig){
  interrupted = 1;
}

int left[] = {
     2,  1,  0,
     3,  4,  5,
     8,  7,  6,
     9, 10, 11,
};
int right[] = {
     0,  1,  2,
     5,  4,  3,
     6,  7,  8,
    11, 10,  9,
};

// colors = array of 12 RGB
void output_pane(RGB *colors, bool is_left=true) {
    int *x = is_left? &left[0] : &right[0];
    char leds[12][31];
    for(int i=0; i<12; i++) {
        sprintf(leds[i], "\e[48;2;%d;%d;%dm  \e[0m", colors[i].r, colors[i].g, colors[i].b);
    }
    printf("┌──┬──┬──┐\e[B\e[10D");
    printf("│%s│%s│%s│\e[B\e[10D", leds[x[0]], leds[x[1]], leds[x[2]]);
    printf("│%s│%s│%s│\e[B\e[10D", leds[x[3]], leds[x[4]], leds[x[5]]);
    printf("├──┼──┼──┤\e[B\e[10D");
    printf("│%s│%s│%s│\e[B\e[10D", leds[x[6]], leds[x[7]], leds[x[8]]);
    printf("│%s│%s│%s│\e[B\e[10D", leds[x[9]], leds[x[10]], leds[x[11]]);
    printf("└──┴──┴──┘\e[B\e[10D");
}

int main(int argc, char **argv) {
    const int loop_duration = 5000;
    HSVInterpolator interp(HSV(0.0, 1.0, 1.0), HSV(1.0, 1.0, 1.0), 0, loop_duration);
    HSV pix;
    RGB leds[96];
    setlocale(LC_ALL, "");
    printf("\n\n\n\n\n\n\n\n\e[A");
    printf("\e[?25l");
    signal(SIGINT, interrupt_handler);
    struct termios old_tty;
    struct termios new_tty;
    tcgetattr(STDIN_FILENO, &old_tty);
    new_tty = old_tty;
    new_tty.c_lflag &= ~ECHO;
    (void) tcsetattr(STDIN_FILENO, TCSANOW, &new_tty);

    pix.s = 1.0;
    pix.v = 1.0;
    auto t1 = std::chrono::high_resolution_clock::now();
    while(!interrupted) {
        auto t2 = std::chrono::high_resolution_clock::now();
        auto frame = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
        for (int i=0; i<96; i++) {
            pix = interp.interpolate((frame + ((loop_duration/8)*(i/6))) % (loop_duration + 1));
            leds[i] = pix.toRgb();
        }
        printf("\e[7A");
        output_pane(&leds[12], true);
        printf("\e[7A\e[12C");
        output_pane(&leds[0], false);

        printf("\e[7A\e[14C");
        output_pane(&leds[36], true);
        printf("\e[7A\e[12C");
        output_pane(&leds[24], false);

        printf("\e[7A\e[14C");
        output_pane(&leds[60], true);
        printf("\e[7A\e[12C");
        output_pane(&leds[48], false);

        printf("\e[7A\e[14C");
        output_pane(&leds[84], true);
        printf("\e[7A\e[12C");
        output_pane(&leds[72], false);
        usleep(12000);
        auto t3 = std::chrono::high_resolution_clock::now();

        printf("\r%lld FPS\r", 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(t3-t2).count());
    }
    (void) tcsetattr(STDIN_FILENO, TCSANOW, &old_tty);
    printf("\n\e[?25h");
    return 0;
}

