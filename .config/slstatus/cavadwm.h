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

static const char *colors[] = {"#35e856", "#a6cc2b", "#cc9c2d", "#c9652a",
                               "#cf3f32", "#cf3273", "#b62ebf", "#7d2fbd",
                               "#3430b8", "#2f8bb5", "#2aad93"};
static const char *ascii[] = {"0", "1", "2", "3", "4", "5",
                              "6", "7", "8", "9", "10"};

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
