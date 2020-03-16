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

static pthread_t exit_thread;
static int _should_exit = 0;

static void exit_task(void);

int main(int argc, char *argv[]) {
	int ch;
	const char *dev = "/dev/ttyUSB0";

	terminal_config();

	printf("\e[0;31mTEST START\e[0m\n");
	while ((ch = getopt(argc, argv, "d:")) != -1) {
		switch (ch) {
			case 'd':
				dev = optarg;
			break;
			case '?':
				printf("Unknown option: %c\n", (char)optopt);
			break;
		}
	}

	if(pthread_create(&exit_thread, NULL, (void *)exit_task, NULL) != 0) {
		printf("\e[0;31mfailed to start exit_task.\e[0m\n");
		goto exit;
	}

	if(SprinklerCtrl_start(dev) != 0) {
      printf("\e[0;31mfailed to start Sprinkler Control.\e[0m\n");
      _should_exit = 1;
      SprinklerCtrl_stop();
      goto exit;
	}

	while(_should_exit != 1) {
		sleep(1);
		printf("pitch = %2.2f        \r", SprinklerCtrl_get_pitch());
		fflush(stdout);
	}

	SprinklerCtrl_stop();

exit:
	pthread_join(exit_thread, NULL);
	printf("\n\e[0;31mTEST DONE\e[0m\n");

	terminal_config_restore();

	return EXIT_SUCCESS;
}

static void exit_task(void)
{
	while(_should_exit != 1) {
		usleep(10000);
		int v = getchar();
		switch(v) {
			case 0x1d: // detect for 'CTRL+['
				_should_exit = 1;
			break;
			default:
				printf("\nkey:0x%x\n", v);
			break;
		}
	}
}
