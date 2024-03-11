#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 500

#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE_ALLOC 128
#define BARS 10
#define NUMBER_ARGS 4
#define FIFO_NAME "/tmp/cava_fifo"
#define TMP_TEMPLATE "/tmp/XXXXXX"
#define BINLOC "/bin/%s"
#define WIDTH 10
#define HEIGHT 25
#define MINSIZE 5
#define FRAMERATE 60

enum cava_actions {
  ACTION_CONTINUOUS,
  ACTION_PRINT,
};

int init(void);

char *colors[] = {"#35e856", "#a6cc2b", "#cc9c2d", "#c9652a",
                  "#cf3f32", "#cf3273", "#b62ebf", "#7d2fbd",
                  "#3430b8", "#2f8bb5", "#2aad93"};
char *ascii[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

struct signal_deamon {
  int graceful;
  pid_t pid;
};

typedef struct signal_deamon sdeamon;

static sdeamon dm = {.graceful = 1, .pid = -1};

void handler(int signum) {
  if (signum == SIGTERM) {
    if (dm.pid > 1) {
      kill(dm.pid, SIGKILL);
    }
    dm.graceful = 0;
  }
  if (signum == SIGUSR1) {
    if (dm.pid > 1) {
      kill(dm.pid, SIGKILL);
    }
    dm.pid = -1;
  }
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

void draw_status2d(float *bars_num, char *BUFFER) {

  int x = 0;
  char formatted[SIZE_ALLOC] = {0};
  for (int i = 0; i < BARS; i++) {
    int colorsize = (int)bars_num[i] / 10;
    sprintf(formatted, "^c%s^^r%d,0,%d,%d^", colors[colorsize], x + WIDTH * i,
            WIDTH, (int)(bars_num[i] * HEIGHT) + MINSIZE);
    strcat(BUFFER, formatted);
  }
  sprintf(formatted, "^f%d^", x + WIDTH * BARS);
  strcat(BUFFER, formatted);
  return;
}

void draw_ascii(float *bars_num, char *BUFFER) {
  char formatted[SIZE_ALLOC] = {0};
  for (int i = 0; i < BARS; i++) {
    int asciisize = bars_num[i] * BARS;
    sprintf(formatted, "%s ", ascii[asciisize]);
    strcat(BUFFER, formatted);
  }
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

void simple_print(int fifo_fd, char *BUFFER) {
  float bars_num[BARS];
  int num_read = frame_bar(fifo_fd, bars_num);

  // Fifo not empty
  if (num_read > 0) {
    draw_ascii(bars_num, BUFFER);
  }
}

void continuous_print(int fifo_fd, char *BUFFER) {
  int num_read = BARS;
  float bars_num[BARS];

  while (dm.graceful && num_read > 0) {
    num_read = frame_bar(fifo_fd, bars_num);
    draw_status2d(bars_num, BUFFER);
    fprintf(stdout, "\n");
    fflush(stdout);
  }
}

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
  }

  dm.pid = exec_cava;
  waitpid(exec_cava, NULL, 0);
  free_step2(tmp_fd, tmp_name, cava_cmd);
  return EXIT_SUCCESS;
}

int cava_dwm(int fifo_fd, enum cava_actions act, char *BUFFER) {

  switch (act) {
  case ACTION_CONTINUOUS:
    continuous_print(fifo_fd, BUFFER);
    break;
  case ACTION_PRINT:
    simple_print(fifo_fd, BUFFER);
    break;
  }

  return EXIT_SUCCESS;
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
    int status;
    status = init();
    if (status == EXIT_FAILURE) {
      _exit(EXIT_FAILURE);
    }
  }

  _exit(EXIT_SUCCESS);
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
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction: error receiving SIGTERM signal.");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int open_fifo_ready(void) {

  int fifo_fd = -1;
  unsigned char buffer[1];
  int num_read;

  do {
    if (fifo_fd > 1) {
      close(fifo_fd);
    }
    usleep(300000); // 300ms wait

    fifo_fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1) {
      fprintf(stderr, "could not open cava FIFO for reading");
      _exit(EXIT_FAILURE);
    }
  } while ((num_read = read(fifo_fd, buffer, 1 * sizeof(unsigned char))) <= 0);
  return fifo_fd;
}

int main(void) {

  init_signal();
  pid_t pid_daemon = start_daemon();
  int fifo_fd = open_fifo_ready();

  int status;
  char BUFFER[SIZE_ALLOC * BARS] = {0};
  while (waitpid(pid_daemon, &status, WNOHANG) == 0) {
    memset(BUFFER, 0, sizeof(char) * SIZE_ALLOC * BARS);
    usleep(1000000 / FRAMERATE);
    cava_dwm(fifo_fd, ACTION_PRINT, BUFFER);
    printf("%s\n", BUFFER);
  }

  close(fifo_fd);
}

// Link to slstatus (so can modify with config.def.h)
// Unlink tmp file
