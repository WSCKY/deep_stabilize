/*
 * main.c
 *
 *  Created on: Feb 25, 2019
 *      Author: kychu
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "terminal.h"
#include "SprinklerCtrl.h"

int main(int argc, char *argv[]) {
	int ch;
	char *baud = "115200";
	const char *dev = "/dev/ttyUSB0";

	terminal_config();

	printf("\e[0;31mTEST START\e[0m\n");
	while ((ch = getopt(argc, argv, "f:d:b:")) != -1) {
		switch (ch) {
			case 'b':
				baud = optarg;
			break;
			case 'd':
				dev = optarg;
			break;
			case '?':
				printf("Unknown option: %c\n", (char)optopt);
			break;
		}
	}

	if(SprinklerCtrl_start(dev, baud) != 0) {
      printf("\e[0;31mfailed to start Sprinkler Control.\e[0m\n");
      SprinklerCtrl_stop();
      goto exit;
	}

	SprinklerCtrl_stop();

exit:
	printf("\n\e[0;31mTEST DONE\e[0m\n");

	terminal_config_restore();

	return EXIT_SUCCESS;
}
