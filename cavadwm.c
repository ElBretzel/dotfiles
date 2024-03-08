#define _POSIX_C_SOURCE 199309L

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
#define FAKE_TMP_FILE "/tmp/cava_config.tmp"
#define SHELL_EXECUTABE "sh"
#define WIDTH 10
#define HEIGHT 25
#define MINSIZE 5

char *colors[] = {"#35e856", "#a6cc2b", "#cc9c2d", "#c9652a",
                  "#cf3f32", "#cf3273", "#b62ebf", "#7d2fbd",
                  "#3430b8", "#2f8bb5", "#2aad93"};
char *ascii[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

struct signal_deamon {
  int signal;
  int graceful;
};

typedef struct signal_deamon sdeamon;

static sdeamon dm = {.signal = 0, .graceful = 1};

void write_config(int temp_fd, int bars) {

  char buffer[1024] = {0};
  const char *filecontent = "[general]\n"
                            "bars = %d\n"
                            "[input]\n"
                            "method = pulse\n"
                            "source = auto\n"
                            "[output]\n"
                            "method = raw\n"
                            "data_format = binary\n"
                            "channels = mono\n"
                            "raw_target = %s\n"
                            "bit_format = 8bit\n";
  sprintf((char *)buffer, filecontent, bars, FIFO_NAME);
  write(temp_fd, buffer, strlen(buffer));
}

void draw_slstatus(float *bars_num) {

  int x = 0;
  for (int i = 0; i < BARS; i++) {
    int colorsize = (int)bars_num[i] / 10;
    printf("^c%s^^r%d,0,%d,%d^", colors[colorsize], x + WIDTH * i, WIDTH,
           (int)((bars_num[i] * HEIGHT) / 100) + MINSIZE);
  }
  printf("^f%d^", x + WIDTH * BARS);
  return;
}

void draw_ascii(float *bars_num) {
  for (int i = 0; i < BARS; i++) {
    int asciisize = (int)bars_num[i] / 10;
    printf("%s ", ascii[asciisize]);
  }
}

char **create_cava_cmd(int temp_fd, int bars) {
  write_config(temp_fd, bars);
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

  char *sh_bin = calloc(SIZE_ALLOC + 1, sizeof(char));
  const char *shell_bin = "/bin/%s";
  memcpy(sh_bin, shell_bin, strlen(shell_bin));
  sprintf(sh_bin, shell_bin, SHELL_EXECUTABE);

  memcpy(cava_cmd[0], sh_bin, strlen(sh_bin));
  memcpy(cava_cmd[1], SHELL_EXECUTABE, strlen(SHELL_EXECUTABE));

  const char *sh_param = "-c";
  memcpy(cava_cmd[2], sh_param, strlen(sh_param));

  const char *cmd_name = "cava -p %s";
  memcpy(cava_cmd[3], cmd_name, strlen(cmd_name));
  sprintf(cava_cmd[3], cmd_name, FAKE_TMP_FILE);
  cava_cmd[NUMBER_ARGS] = NULL;

  free(sh_bin);

  return cava_cmd;
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

void handler(int signum) {
  if (signum == SIGUSR1) {
    dm.graceful = 0;
  } else if (signum == SIGUSR2) {
    // Restart (move main)
  }
}

int main(int argc, char *argv[]) {
  int bars = BARS;
  if (argc > 1)
    bars = atoi(argv[1]);
  if (bars < 1) {
    fprintf(stderr, "error: bars is '%d', must be 1 or more\n", bars);
    exit(EXIT_FAILURE);
  }

  // You can use O_TMP_FILE but for portability reason, I prefer to create FAKE
  // temp file...
  // other possibilities:
  // mkstemp but require feature test macro
  // tmpnam but require c11
  remove(FAKE_TMP_FILE);
  int temp_fd = open(FAKE_TMP_FILE, O_RDWR | O_CREAT | O_EXCL, 0666);
  // int temp_fd = open(FAKE_TMP_FILE, O_WRONLY | O_TMPFILE | O_EXCL, 0666);
  if (temp_fd == -1) {
    perror("temp file could not be created");
    exit(EXIT_FAILURE);
  }

  unlink(FIFO_NAME);
  if (mkfifo(FIFO_NAME, 0666) == -1) {
    perror("could not create cava FIFO for writing");
    close(temp_fd);
    exit(EXIT_FAILURE);
  }

  char **cava_cmd = create_cava_cmd(temp_fd, bars);
  if (cava_cmd == NULL) {
    perror("could not allocate the cava command, should not happen under any "
           "circumstance");
    close(temp_fd);
    exit(EXIT_FAILURE);
  }

  int fds[2];
  if (pipe(fds) == -1) {
    perror("could not create pipe");
    close(temp_fd);
    free_cava_cmd(cava_cmd);
    exit(EXIT_FAILURE);
  }

  pid_t exec_cava;

  if ((exec_cava = fork()) == 0) {
    close(fds[0]);
    close(fds[1]);

    if (execvp(*cava_cmd, cava_cmd + 1)) {
      close(temp_fd);
      perror("could not execve the command");
      _exit(EXIT_FAILURE); // bye bye alloc
    }
    _exit(EXIT_SUCCESS);

  } else if (exec_cava == -1) {
    perror("could not fork");
    close(fds[0]);
    close(fds[1]);
    close(temp_fd);
    free_cava_cmd(cava_cmd);
    exit(EXIT_FAILURE);
  }

  pid_t deamon = fork();
  if (deamon == -1) {
    perror("could not fork");
    close(fds[0]);
    close(fds[1]);
    close(temp_fd);
    free_cava_cmd(cava_cmd);
    exit(EXIT_FAILURE);
  }

  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  if (deamon != 0) {
    close(fds[0]);
    close(fds[1]);
    close(temp_fd);
    exit(EXIT_SUCCESS);
  }

  if (sigemptyset(&sa.sa_mask) == -1) {
    perror("sigemptyset: could not fill mask.");
    return -1;
  }
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction: error receiving SIGUSR1 signal.");
    return -1;
  }
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    perror("sigaction: error receiving SIGUSR2 signal.");
    return -1;
  }

  close(fds[1]);
  if (dup2(fds[0], STDIN_FILENO) == -1) // Right end takes stdin
  {
    close(fds[0]);
    close(temp_fd);
    perror("could not dup2");
    _exit(EXIT_FAILURE); // bye bye alloc
  }
  close(fds[0]);

  FILE *fifo_file = fopen(FIFO_NAME, "rb");
  if (fifo_file == NULL) {
    fprintf(stderr, "could not open cava FIFO for reading");
    _exit(EXIT_FAILURE);
  }

  unsigned char bar_heights[bars];
  int num_read = fread(bar_heights, sizeof(unsigned char), bars, fifo_file);
  float bars_num[bars];

  while (dm.graceful && num_read >= bars) {
    fread(bar_heights, sizeof(unsigned char), bars, fifo_file);

    for (int i = 0; i < bars; i++) {
      float height = (bar_heights[i] / 255.f) * 100;
      bars_num[i] = height;
    }

    draw_slstatus(bars_num);
    fprintf(stdout, "\n");
    fflush(stdout);
  }

  fclose(fifo_file);
  free_cava_cmd(cava_cmd);
  close(temp_fd);
  remove(FAKE_TMP_FILE);

  int status;
  kill(exec_cava, SIGKILL);

  return WIFEXITED(status);
}
