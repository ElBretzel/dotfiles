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

int init(void);
int open_fifo_ready(void);

void handler(int signum) {
  if (signum == SIGTERM || signum == SIGINT) {
    printf("PID: %d\n", dm.pid);
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
    printf("I enter\n");
    printf("PID: %d\n", dm.pid);
    if (dm.pid > 1) {
      // printf("%d\n", dm.pid);
      kill(dm.pid, SIGTERM);
    }
    // Frontend listen
    else {
      printf("Closing fifo %s: %d\n", FIFO_NAME, dm.fifo_fd);
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

void write_config(int tmp_fd) {
  char buffer[1024] = {0};
  const char *config = "[general]\n"
                       "bars = %d\n"
                       "[input]\n"
                       "method = pulse\n"
                       "source = auto\n"
                       "[output]\n"
                       "method = raw\n"
                       "data_format = binary\n"
                       "channels = stereo\n"
                       "raw_target = %s\n"
                       "bit_format = 8bit\n";

  sprintf(buffer, config, BARS, FIFO_NAME);
  if (write(tmp_fd, buffer, strlen(buffer)) == -1) {
    perror("could not write into temporary file");
    exit(EXIT_FAILURE);
  }
  if (fsync(tmp_fd) == -1) {
    perror("could not sync the temporary file");
    exit(EXIT_FAILURE);
  }
}

void draw_status2d(float *bars_num, const char *format, char *BUFFER) {

  int x = 0;
  char formatted[SIZE_ALLOC] = {0};
  char *subbuffer = calloc(SIZE_ALLOC * BARS + 1, sizeof(char));
  if (subbuffer == NULL) {
    return;
  }
  for (int i = 0; i < BARS; i++) {
    int colorsize = (int)bars_num[i] / 10;
    sprintf(formatted, "^c%s^^r%d,0,%d,%d^^c%s^^r%d,%d,%d,%d^",
            colors[colorsize], x + WIDTH * i, WIDTH,
            (int)(bars_num[i] * HEIGHT) + MINSIZE, "#ffffff", x + WIDTH * i,
            (int)(bars_num[i] * HEIGHT) + MINSIZE, WIDTH, HEIGHT * 100);
    strcat(subbuffer, formatted);
  }
  sprintf(formatted, "^f%d^", x + WIDTH * BARS);
  strcat(subbuffer, formatted);
  int l = sprintf(BUFFER, format, subbuffer);
  BUFFER[l] = '\0';
  free(subbuffer);

  return;
}

void draw_ascii(float *bars_num, const char *format, char *BUFFER) {
  char formatted[SIZE_ALLOC] = {0};
  char *subbuffer = calloc(SIZE_ALLOC * BARS + 1, sizeof(char));
  if (subbuffer == NULL) {
    return;
  }
  for (int i = 0; i < BARS; i++) {
    int asciisize = bars_num[i] * BARS;
    sprintf(formatted, "%s ", ascii[asciisize]);
    strcat(subbuffer, formatted);
  }
  int l = sprintf(BUFFER, format, subbuffer);
  BUFFER[l] = '\0';
  free(subbuffer);
}

char **create_cava_cmd(int tmp_fd, char *tmp_name) {
  write_config(tmp_fd);
  char **cava_cmd = calloc(NUMBER_ARGS + 1, sizeof(char *));
  if (cava_cmd == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < NUMBER_ARGS; i++) {
    cava_cmd[i] = calloc(SIZE_ALLOC + 1, sizeof(char));
    if (*cava_cmd == NULL) {
      return NULL;
    }
  }

  sprintf(cava_cmd[0], BINLOC, "cava");
  memcpy(cava_cmd[1], "cava", strlen("cava"));
  memcpy(cava_cmd[2], "-p", strlen("-p"));
  sprintf(cava_cmd[3], "%s", tmp_name);
  cava_cmd[NUMBER_ARGS] = NULL;

  return cava_cmd;
}

int frame_bar(int fifo_fd, float *bars_num) {
  unsigned char bar_heights[BARS] = {0};
  unsigned char copy_bar_heights[BARS] = {0};
  int num_read;

  // Flush fifo and keep in memory the last one
  while ((num_read = read(fifo_fd, copy_bar_heights,
                          BARS * sizeof(unsigned char))) >= BARS) {
    memcpy(bar_heights, copy_bar_heights, sizeof(unsigned char) * BARS);
  }

  for (int i = 0; i < BARS; i++) {
    float height = copy_bar_heights[i] / 255.f;
    bars_num[i] = height;
  }
  return num_read != 0;
}

/*
void continuous_print(int fifo_fd, const char *format, char *BUFFER) {
  int num_read = BARS;
  float bars_num[BARS];

  while (dm.graceful && num_read > 0) {
    num_read = frame_bar(fifo_fd, bars_num);
    draw_status2d(bars_num, format, BUFFER);
    fprintf(stdout, "\n");
    fflush(stdout);
  }
}
*/

void free_cava_cmd(char **cava_cmd) {

  if (cava_cmd == NULL) {
    return;
  }
  for (size_t i = 0; i < NUMBER_ARGS; i++) {
    if (cava_cmd[i] == NULL) {
      continue;
    }
    free(cava_cmd[i]);
  }
  free(cava_cmd);
}

int free_step0(char *tmp_name) {
  if (tmp_name != NULL) {
    free(tmp_name);
  }
  return EXIT_FAILURE;
}
int free_step1(int tmp_fd, char *tmp_name) {
  close(tmp_fd);
  unlink(tmp_name);
  free_step0(tmp_name);
  return EXIT_FAILURE;
}

int free_step2(int tmp_fd, char *tmp_name, char **cava_cmd) {
  free_step1(tmp_fd, tmp_name);
  free_cava_cmd(cava_cmd);
  return EXIT_FAILURE;
}

int init(void) {
  char *tmp_name = calloc(SIZE_ALLOC + 1, sizeof(char));
  if (tmp_name == NULL) {
    return EXIT_FAILURE;
  }

  memcpy(tmp_name, TMP_TEMPLATE, strlen(TMP_TEMPLATE));
  int tmp_fd = mkstemp(tmp_name);
  if (tmp_fd == -1) {
    perror("could not created templated temporary file");
    return free_step0(tmp_name);
  }

  unlink(FIFO_NAME);
  if (mkfifo(FIFO_NAME, 0666) == -1) {
    perror("could not create cava FIFO for writing");
    return free_step1(tmp_fd, tmp_name);
  }

  char **cava_cmd = create_cava_cmd(tmp_fd, tmp_name);
  if (cava_cmd == NULL) {
    perror("could not allocate the cava command, should not happen under any "
           "circumstance");
    return free_step1(tmp_fd, tmp_name);
  }

  pid_t exec_cava = fork();

  if (exec_cava == -1) {
    perror("could not fork");
    return free_step2(tmp_fd, tmp_name, cava_cmd);
  }

  if (exec_cava == 0) {

    // OPTIONNAL: redirect stdout && stderr >> /dev/null
    // LESS ERROR PRONE FINAL OUTPUT
    // Because we are in a different process, stdout will not be modified in
    // parent
    int trash_file = open("/dev/null", O_WRONLY);
    if (trash_file != -1) {
      dup2(trash_file, STDOUT_FILENO);
      dup2(trash_file, STDERR_FILENO);
      close(trash_file);
    }

    if (execvp(*cava_cmd, cava_cmd + 1) == -1) {
      perror("could not execve the command");
      _exit(EXIT_FAILURE); // bye bye alloc
    }
    _exit(EXIT_SUCCESS);
  }

  dm.pid = exec_cava;

  int status = EXIT_FAILURE;
  while (!waitpid(exec_cava, &status, WNOHANG)) {
    usleep(100000);
  }

  free_step2(tmp_fd, tmp_name, cava_cmd);

  return status == EXIT_FAILURE;
}

void fill_bar_buffer(int fifo_fd, const char *format, char *BUFFER) {

  float bars_num[BARS];
  int num_read = frame_bar(fifo_fd, bars_num);

  // Fifo not empty
  if (num_read > 0) {
    draw_status2d(bars_num, format, BUFFER);
  }
}

pid_t start_daemon(void) {

  pid_t daemon = fork();
  if (daemon == -1) {
    perror("could not start cava dwm program as daemon...");
    exit(EXIT_FAILURE);
  }

  if (daemon != 0) {
    return daemon;
  }

  while (dm.graceful) {
    printf("graceful: %d\n", dm.graceful);
    printf("Before: %d\n", dm.pid);
    int status;
    status = init();
    printf("After: %d\n", dm.pid);
    if (status != EXIT_SUCCESS) {
      _exit(EXIT_FAILURE);
    }
  }
  printf("I exited graceful\n");

  _exit(EXIT_SUCCESS);
}

int open_fifo_ready(void) {

  int fifo_fd = -1;
  unsigned char buffer[1];
  int num_read;
  int max_try = 10;

  do {
    if (fifo_fd > 1) {
      close(fifo_fd);
    }
    usleep(300000); // 300ms wait

    fifo_fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK | O_CREAT);
    if (fifo_fd == -1) {
      fprintf(stderr, "could not open cava FIFO for reading");
      _exit(EXIT_FAILURE);
    }
    max_try--;
  } while ((num_read = read(fifo_fd, buffer, 1 * sizeof(unsigned char))) <= 0 &&
           max_try > 0);
  if (max_try == 0) {
    fprintf(stderr, "could not open fifo at all");
    return -1;
  }
  return fifo_fd;
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
  char *TMPBUFFER = calloc(iwanttodie, sizeof(char));
  if (TMPBUFFER == NULL) {
    fprintf(stderr, "Could not allocate temp buffer");
    dm.graceful = 0;
    return EXIT_FAILURE;
  }

  char *cpy_buffer = calloc(iwanttodie, sizeof(char));
  if (cpy_buffer == NULL) {
    fprintf(stderr, "Could not allocate buffer");
    free(BUFFER);
    free(TMPBUFFER);
    dm.graceful = 0;
    return EXIT_FAILURE;
  }

  // Open fifo cava
  int fifo_fd = open_fifo_ready();
  dm.fifo_fd = fifo_fd;

  if (dm.fifo_fd == -1) {
    free(BUFFER);
    free(TMPBUFFER);
    dm.graceful = 0;
    return EXIT_FAILURE;
  }

  do {
    memset(BUFFER, 0, sizeof(char) * (iwanttodie - 1));

    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
      close(dm.fifo_fd);
      free(BUFFER);
      free(TMPBUFFER);
      free(cpy_buffer);
      dm.graceful = 0;
      die("clock_gettime:");
    }

    memcpy(TMPBUFFER, BUFFER, sizeof(char) * (iwanttodie - 1));
    for (i = len = 0; i < LEN(args); i++) {
      if (cpy_buffer[0] != '\0' && i % 3 == 0) {
        fill_bar_buffer(fifo_fd, cpy_buffer, TMPBUFFER);
        print_status(TMPBUFFER, cpy_buffer);
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
        free(TMPBUFFER);
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
          free(TMPBUFFER);
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
  free(TMPBUFFER);
  free(cpy_buffer);
  if (!sflag) {
    XStoreName(dpy, DefaultRootWindow(dpy), NULL);
    if (XCloseDisplay(dpy) < 0)
      die("XCloseDisplay: Failed to close display");
  }

  return 0;
}
