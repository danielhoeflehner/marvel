#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "str.h"

str *str_create()
{
    str *s = malloc(sizeof(str));
    if (s == NULL) {
        perror("Cannot allocate memory for new string in str_create. [Error]");
        exit(1);
    }

    s->size = STR_INITIAL_SIZE;
    s->expand = STR_EXPAND;
    s->len = 0;
    s->data = calloc(s->size, sizeof(char));
    if (s->data == NULL) {
        perror("Cannot allocate memory for new string data in str_create. [Error]");
        exit(1);
    }

    return s;
}

void str_append(str *s, const char *append, size_t len)
{
    char *new = NULL;
    uint32_t new_len = s->len + len;
    if (new_len >= s->size) {
        new = realloc(s->data, new_len + s->expand);
        s->size += s->expand;
    } else {
        new = s->data;
    }

    memcpy(&new[s->len], append, len);
    
    new[new_len] = '\0';
    s->data = new;
    s->len = new_len;
}

void str_destroy(str *s)
{
    if (s->data) {
        free(s->data);
    }
}