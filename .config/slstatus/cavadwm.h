#ifndef CAVADWM_H
#define CAVADWM_H

#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"

#define SIZE_ALLOC 128
#define NUMBER_ARGS 4
#define FIFO_NAME "/tmp/cava_fifo"

static const char *color_bg = "#1a1b26";
static const char *colors[] = {"#7daea3", "#a9b665", "#d4a356", "#d8a657",
                               "#e78a4e", "#de854c", "#e96c5f", "#ea6962"};
static const char *ascii[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

enum cava_actions {
  ACTION_CONTINUOUS,
  ACTION_PRINT,
};

struct signal_deamon {
  int graceful;
  pid_t pid;
  int fifo_fd;
};

typedef struct signal_deamon sdeamon;
static sdeamon dm = {.graceful = 1, .pid = -1, .fifo_fd = -1};

#endif /* CAVADWM_H */
