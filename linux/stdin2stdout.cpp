/*
 * stdin2stdout.cpp
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 *
 * Scale the virtual terminal to double with of the
 * image being tested and same height.
 *
 * ./stdin2stdout < ../test_images/sample_1.gif
 */

#include <stdio.h> // printf()
#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h>

#include "../src/PipeGIF.h" // Will include VirtualDisplay & LZWdecod
#include "ansiTerminal.h"

int main(int argc, char *argv[]) {
    int ch;

    useconds_t delay = argc > 1 ? atoi(argv[1]) : 0;

    printf("Syntax:\n\t%s <delay microsec> < <giffile>\n", argv[0]);
    printf("Example:\n\t%s %u < interlace.gif\n", argv[0], delay);

    ansiTerminal ansiTerm(150, 42); // Text mode demo display
    PipeGIF gif( &ansiTerm ); // Decode GIF and send it to display

    while ((ch = fgetc(stdin)) != EOF) {
       switch ( gif.decode(ch) ) { // decode one byte GIF data
           case GIF_OK:
               // All OK
               break;
           case GIF_DELAY100:
	       fflush(stdout);
               usleep( 10000*gif.getDelay100() );
               break;
           case GIF_TRAILER:
               // End of image. Try pipe several images:
	       fflush(stdout);
	       usleep( 500000 ); // cat *.gif | ./stdin2stdout
               break;
           default:
	       fprintf(stderr,"\e[0m\nError GIF pasing character '0x%02x'\n", ch);
               exit(1);
       }
       usleep( delay );
    }

    return EXIT_SUCCESS;
}
