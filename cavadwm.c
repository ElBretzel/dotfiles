#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE_ALLOC 128
#define BARS 10
#define NUMBER_ARGS 3
#define FIFO_NAME "/tmp/cava_fifo"
#define FAKE_TMP_FILE "/tmp/cava_config.tmp"

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

void draw_slstatus(float height) {
  fprintf(stdout, "%3f ", height);
  return;
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

  const char *cmd_name = "cava";
  const char *cmd_param = "-p";

  memcpy(cava_cmd[0], cmd_name, strlen(cmd_name));
  memcpy(cava_cmd[1], cmd_param, strlen(cmd_param));
  memcpy(cava_cmd[2], FAKE_TMP_FILE, strlen(FAKE_TMP_FILE));
  cava_cmd[NUMBER_ARGS] = NULL;

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
  // int temp_fd = open(FAKE_TMP_FILE, O_WRONLY | O_TMP_FILE | O_EXCL, 0666);
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

  int fifo_fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK, 0666);
  if (fifo_fd == -1) {
    free_cava_cmd(cava_cmd);
    fprintf(stderr, "could not open cava FIFO for reading");
    close(temp_fd);
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  int status;

  if (pid == -1) {
    perror("could not fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) // Child
  {
    if (execvp("cava", cava_cmd + 1) == -1) {
      perror("could not execve the command");
      _exit(EXIT_FAILURE); // bye bye alloc
    }
    _exit(EXIT_SUCCESS);
  }

  unsigned char bar_heights[bars];
  while (!waitpid(pid, &status, WNOHANG)) {
    int num_read = read(fifo_fd, bar_heights, sizeof(unsigned char) * bars);

    if (num_read < bars) {
      continue;
    }

    for (int i = 0; i < bars; i++) {
      float height = (bar_heights[i] / 255.f) * 100;
      draw_slstatus(height);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
  }

  free_cava_cmd(cava_cmd);
  close(temp_fd);
  close(fifo_fd);
  return WIFEXITED(status);
}
