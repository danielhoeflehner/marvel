#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "str.h"
#include "util.h"

str *str_create()
{
    str *s = malloc(sizeof(str));
    throw_mem_(s);

    s->size = STR_INITIAL_SIZE;
    s->expand = STR_EXPAND;
    s->len = 0;

    s->data = calloc(s->size, sizeof(char));
    throw_mem_(s->data);

    return s;

    error:
    return NULL;
}

str *str_from(const char *string)
{
    size_t len = strlen(string);

    str *s = str_create();
    str_append(s, string, len);    

    return s;
}

void str_append(str *s, const char *append, size_t len)
{
    char *new = NULL;
    uint32_t new_len = s->len + len;
    if (new_len >= s->size) {
        new = realloc(s->data, new_len + s->expand);
        throw_mem_(new);
        s->size += s->expand;
    } else {
        new = s->data;
    }

    memcpy(&new[s->len], append, len);
    
    new[new_len] = '\0';
    s->data = new;
    s->len = new_len;

    error:
    return;
}

void str_destroy(str *s)
{
    if (s->data) {
        free(s->data);
    }

    free(s);
}