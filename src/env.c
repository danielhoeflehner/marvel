#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "marvel.h"
#include "str.h"
#include "util.h"

int env_init(const char *filename) {
  if (!filename) {
    filename = ".env";
  }

  FILE *f = fopen(filename, "r");
  throw_v_(f == NULL, "Could not open %s file.", filename);

  char *line = NULL;
  size_t len = 0;
  ssize_t read = 0;
  int ret = 0;

  strlist *list = NULL;
  str *envval = str_create();
  throw_(envval == NULL, "Could not create envval string object.");

  str *env_key = NULL;
  str *env_value = NULL;

  // Read every line of the .env file
  // and set the env variable.
  while ((read = getline(&line, &len, f)) != -1) {
    str_put_into(envval, line, read);

    // get rid of the <newline> char
    str_strip_nl(envval);

    list = str_split(envval, "=");
    if (list == NULL) {
      continue;
    }

    env_key = strlist_at(list, 0);
    throw_(env_key == NULL, "Could not get key of env variable.");

    env_value = strlist_at(list, 1);
    throw_(env_value == NULL, "Could not get value of env variable.");

    ret = setenv(str_data(env_key), str_data(env_value), 1);
    throw_(ret == -1, "Could not set env variable.");

    strlist_destroy(list);
  }

  str_destroy(envval);
  free(line);
  fclose(f);
  return 0;

error:
  str_destroy(envval);
  if (line) free(line);
  if (f) fclose(f);
  return -1;
}