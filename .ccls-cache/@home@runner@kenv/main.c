#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef x86_64
#define USE_NASM
#endif

#ifndef linux
int main() {
  printf("[kenv] as for now kenv only supports linux, but will eventually be supported on all systems");
  return -1;
}
#endif

#ifdef linux
#define ENV_PRINTF(format, vargs...)                                           \
  printf("[kenv] ");                                                           \
  printf(format, vargs)

#define CREATE_DIRECTORY(_fp)                                                  \
  dir_result = mkdir(_fp, 0700);                                               \
  switch (dir_result) {                                                        \
  case 0:                                                                      \
    ENV_PRINTF("created directory \"%s\"", _fp);                               \
    break;                                                                     \
  case -1: {                                                                   \
    ENV_PRINTF("directory already exists at %s", _fp);                         \
    exit(dir_result);                                                          \
  };                                                                           \
  }

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: kenv [project-name] [location]\n");
    return 0;
  }

  char *project_name = argv[1];
  char *path = argv[2];

  char *project_path = malloc(strlen(path) + strlen(project_name) + 0xFF);
  sprintf(project_path, "%s/%s", path, project_name);

  char *build_path = malloc(strlen(project_path) + 0xF);
  sprintf(build_path, "%s/%s", project_path, "build");

  char *src_path = malloc(strlen(project_path) + 0xF);
  sprintf(src_path, "%s/%s", project_path, "src");

  char *main_file_path = malloc(strlen(src_path) + 0xF);
  sprintf(main_file_path, "%s/%s", src_path, "main.ktro");

  int dir_result = mkdir(project_path, 0700);
  if (dir_result == 0) {
    printf("[kenv] project directory at %s\n\n", path);
    CREATE_DIRECTORY(build_path);

    CREATE_DIRECTORY(src_path);
    FILE *main_file = fopen(main_file_path, "w");
    fprintf(main_file, "@entry main;\n\n"
                       "fn main: i32 (argc: i32, argv: [str]) {\n"
                       "\tfmw(\"Hello, world!\\n\");\n"
                       "\treturn 0;\n"
                       "}");
    fclose(main_file);
  } else {
    switch (dir_result) {
    case -1:
      ENV_PRINTF("directory already exists at %s", project_path);
    }

    exit(dir_result);
  }

  return 0;
}

#endif