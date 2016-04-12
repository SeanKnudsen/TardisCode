#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>
#include <signal.h>

#include "../tardis/color.h"

volatile sig_atomic_t interrupted = 0;
void interrupt_handler(int sig){
  interrupted = 1;
}

// colors = array of 12 RGB
void output_pane(RGB *colors) {
    char leds[12][31];
    for(int i=0; i<12; i++) {
        sprintf(leds[i], "\e[48;2;%d;%d;%dm  \e[0m", colors[i].r, colors[i].g, colors[i].b);
    }
    wprintf(L"┌──┬──┬──┐\e[B\e[10D");
    wprintf(L"│%s│%s│%s│\e[B\e[10D", leds[ 0], leds[ 1], leds[ 2]);
    wprintf(L"│%s│%s│%s│\e[B\e[10D", leds[ 3], leds[ 4], leds[ 5]);
    wprintf(L"├──┼──┼──┤\e[B\e[10D");
    wprintf(L"│%s│%s│%s│\e[B\e[10D", leds[ 6], leds[ 7], leds[ 8]);
    wprintf(L"│%s│%s│%s│\e[B\e[10D", leds[ 9], leds[10], leds[11]);
    wprintf(L"└──┴──┴──┘\e[B\e[10D");
}

int main(int argc, char **argv) {
    HSVInterpolator interp(HSV(0.0, 1.0, 1.0), HSV(1.0, 1.0, 1.0), 0, 50000);
    HSV pix;
    RGB leds[96];
    int frame=0;
    setlocale(LC_ALL, "");
    printf("\n\n\n\n\n\n\n\n\e[A");
    printf("\e[?25l");
    signal(SIGINT, interrupt_handler);
    pix.s = 1.0;
    pix.v = 1.0;
    while(!interrupted) {
        pix = interp.interpolate((frame++) % 50001);
        for (int i=0; i<12; i++) {
            leds[i] = pix.toRgb();
        }
        printf("\e[7A");
        output_pane(&leds[0]);
        usleep(16);
    }
    printf("\n\e[?25h");
    return 0;
}

