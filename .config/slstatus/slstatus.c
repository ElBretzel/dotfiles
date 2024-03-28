/* See LICENSE file for copyright and license details. */
#include <X11/Xlib.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "arg.h"
#include "cavadwm.h"
#include "config.def.h"
#include "config.h"
#include "util.h"

#define SIZE_ALLOC 128

char buf[1024];
static volatile sig_atomic_t done;
static Display *dpy;

void handler(int signum) {
  if (signum == SIGTERM || signum == SIGINT) {
    if (dm.pid > 1) {
      kill(dm.pid, SIGTERM);
      // printf("kill(%d, SIGINT);\n", dm.pid);
    }
    done = 1;
    dm.graceful = 0;
  }
  if (signum == SIGUSR1) {
    // printf("Enter: %d\n", dm.pid);
    //  Backend listen
    if (dm.pid > 1) {
      // printf("%d\n", dm.pid);
      kill(dm.pid, SIGTERM);
    }
    // Frontend listen
    else {
      // printf("Closing fifo\n");
      close(dm.fifo_fd);
      unlink(FIFO_NAME);
      mkfifo(FIFO_NAME, 0666);
      dm.fifo_fd = open_fifo_ready();
    }
    dm.pid = -1;
  }
}

int init_signal(void) {
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  if (sigemptyset(&sa.sa_mask) == -1) {
    perror("sigemptyset: could not fill mask.");
    return EXIT_FAILURE;
  }
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction: error receiving SIGUSR1 signal.");
    return EXIT_FAILURE;
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction: error receiving SIGINT signal.");
    return EXIT_FAILURE;
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction: error receiving SIGTERM signal.");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void difftimespec(struct timespec *res, struct timespec *a,
                         struct timespec *b) {
  res->tv_sec = a->tv_sec - b->tv_sec - (a->tv_nsec < b->tv_nsec);
  res->tv_nsec = a->tv_nsec - b->tv_nsec + (a->tv_nsec < b->tv_nsec) * 1E9;
}

static void usage(void) { die("usage: %s [-v] [-s] [-1]", argv0); }

static void print_status(char *BUFFER, char *cpy_buffer) {
  if (XStoreName(dpy, DefaultRootWindow(dpy), BUFFER) < 0) {
    close(dm.fifo_fd);
    free(BUFFER);
    dm.graceful = 0;
    free(cpy_buffer);

    die("XStoreName: Allocation failed");
  }
  XFlush(dpy);
}

int main(int argc, char *argv[]) {
  struct timespec start, current, diff, intspec, wait, cavawait, nwait;
  size_t i, len;
  int sflag, ret;
  const char *res;

  sflag = 0;
  ARGBEGIN {
  case 'v':
    die("slstatus-" VERSION);
  case '1':
    done = 1;
    /* FALLTHROUGH */
  case 's':
    sflag = 1;
    break;
  default:
    usage();
  }
  ARGEND

  if (argc)
    usage();

  // Init slstatus && cava slstatus SIGNAL handler
  init_signal();

  if (!sflag && !(dpy = XOpenDisplay(NULL)))
    die("XOpenDisplay: Failed to open display");

  // Start cava slstatus daemon
  pid_t pid_daemon = start_daemon();

  size_t iwanttodie = SIZE_ALLOC * BARS + MAXLEN + 1;

  int status;
  char *BUFFER = calloc(iwanttodie, sizeof(char));
  if (BUFFER == NULL) {
    fprintf(stderr, "Could not allocate buffer");
    dm.graceful = 0;
    return EXIT_FAILURE;
  }

  char *cpy_buffer = calloc(iwanttodie, sizeof(char));
  if (cpy_buffer == NULL) {
    fprintf(stderr, "Could not allocate buffer");
    free(BUFFER);
    dm.graceful = 0;
    return EXIT_FAILURE;
  }

  // Open fifo cava
  int fifo_fd = open_fifo_ready();
  dm.fifo_fd = fifo_fd;

  if (dm.fifo_fd == -1) {
    free(BUFFER);
    dm.graceful = 0;
    return EXIT_FAILURE;
  }

  do {
    memset(BUFFER, 0, sizeof(char) * (iwanttodie - 1));

    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
      close(dm.fifo_fd);
      free(BUFFER);
      free(cpy_buffer);
      dm.graceful = 0;
      die("clock_gettime:");
    }

    for (i = len = 0; i < LEN(args); i++) {
      if (cpy_buffer[0] != '\0' && i % 3 == 0) {
        fill_bar_buffer(fifo_fd, cpy_buffer, BUFFER);
        print_status(BUFFER, cpy_buffer);
      }
      if (!(res = args[i].func(args[i].args)))
        res = unknown_str;

      if ((ret = esnprintf(BUFFER + len, (iwanttodie - 1), args[i].fmt, res)) <
          0)
        break;

      len += ret;
    }

    strncpy(cpy_buffer, BUFFER, len);
    cpy_buffer[len] = '\0';

    if (sflag) {
      puts(BUFFER);
      fflush(stdout);
      if (ferror(stdout)) {
        close(dm.fifo_fd);
        dm.graceful = 0;
        free(BUFFER);
        free(cpy_buffer);
        die("puts:");
      }
    } else {
      if (!done) {
        if (clock_gettime(CLOCK_MONOTONIC, &current) < 0) {
          close(dm.fifo_fd);
          free(cpy_buffer);
          dm.graceful = 0;
          free(BUFFER);
          die("clock_gettime:");
        }
        difftimespec(&diff, &current, &start);

        intspec.tv_sec = INTERVAL / 1000;
        intspec.tv_nsec = (INTERVAL % 1000) * 1E6;
        difftimespec(&wait, &intspec, &diff);

        cavawait.tv_sec = wait.tv_sec / ((wait.tv_nsec * FRAMERATE) / 1E9);
        cavawait.tv_nsec = wait.tv_nsec / FRAMERATE;
        intspec.tv_sec = 0;
        intspec.tv_nsec = 0;

        difftimespec(&nwait, &cavawait, &intspec);
      }

      size_t n = (wait.tv_nsec * FRAMERATE) / 1E9;
      for (size_t i = 0; i < n; i++) {
        fill_bar_buffer(fifo_fd, cpy_buffer, BUFFER);
        // printf("%s\n", BUFFER);
        print_status(BUFFER, cpy_buffer);
        // printf("%s\n", BUFFER);
        nanosleep(&nwait, NULL);
      }
    }

  } while (!done && waitpid(pid_daemon, &status, WNOHANG) == 0);

  close(dm.fifo_fd);
  dm.graceful = 0;
  unlink(FIFO_NAME);
  free(BUFFER);
  free(cpy_buffer);
  if (!sflag) {
    XStoreName(dpy, DefaultRootWindow(dpy), NULL);
    if (XCloseDisplay(dpy) < 0)
      die("XCloseDisplay: Failed to close display");
  }

  return 0;
}
