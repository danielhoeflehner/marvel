#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "str.h"
#include "util.h"

/**
 *-------------------------------------------------------------------
 * String object
 *-------------------------------------------------------------------
 *
 * This section provides functions for creating and manipulating
 * string objects. String objects hold the raw string data
 * and some additional information about the string, like the 
 * current length, the size or the expand rate.
 * String objects are initialised with an initial size, which means
 * that at creation a specific amount of memory is allocated for
 * the raw data. Should the data inserted be greater, the data 
 * will be "expanded" by at least the amount which is needed.
 * 
 *-------------------------------------------------------------------
 */

/**
 * Creates / initialises an empty string object.
 * 
 * @return str | NULL   pointer to a string object
 */
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
    if (s) free(s);
    return NULL;
}

/**
 * Create a string object from a raw C char array.
 * The char array will be copied into the string object.
 * 
 * @param const char *string    pointer to a char array
 * 
 * @return str                  pointer to a string object
 */
str *str_from(const char *string)
{
    throw_(string == NULL, "String is NULL.");

    size_t len = strlen(string);

    str *s = str_create();

    // the raw string can just be appended
    str_append(s, string, len);    

    return s;

    error:
    return NULL;
}

/**
 * Duplicate a string object.
 * 
 * @param const str *s  pointer to string object
 * 
 * @return str          pointer to new string object
 */
str *str_duplicate(const str *s)
{
    if (s == NULL) {
        return NULL;
    }

    return str_from(str_data(s));
}

/**
 * Append a raw char string onto an existing string object. If the new
 * string would be too long, memory will be reallocated to hold the whole
 * string data.
 * 
 * @param str *s                pointer to a string object
 * @param const char *append    pointer to a char array
 * @param size_t len            string length of the char array
 * 
 * @return void
 */
void str_append(str *s, const char *append, size_t len)
{
    throw_(s == NULL, "String is NULL.");
    throw_(append == NULL, "String to append is NULL.");

    if (len <= 0) {
        goto error;
    }

    char *new = NULL;

    // calculate the needed size
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

/**
 * Split a string into parts by a delimiter char. Each part
 * will be stored in a string object which itself will be
 * held in a node of a string list.
 * 
 * @param str *s            pointer to a string object
 * @param char delimiter    char by which the string should be split
 * 
 * @return strlist | NULL   pointer to string list
 */
strlist *str_split(str *s, const char *delimiter)
{
    strlist *sl = strlist_create();
    throw_(sl == NULL, "Could not create string list.");

    // get the first token
    char *token = strtok(str_data(s), delimiter);
    throw_(token == NULL, "Could not find first token.");

    // if token is not NULL, the token will be pushed onto
    // the string list
    while (token != NULL) {
        strlist_push(sl, str_from(token));

        // get the next token
        token = strtok(NULL, delimiter);
    }

    return sl;

    error:
    return NULL;
}

/**
 * Put a raw string into an existing string object. Existing data
 * will be overwritten.
 * 
 * @param str *s            point to a string object
 * @param const char *put   pointer to data which will be put
 * @param size_t len        len of the raw string to copy into
 * 
 */
void str_put_into(str *s, const char *put, size_t len)
{
    // to copy / overwrite the existing data, we just set
    // the len to 0, so that the new string will be copied
    // to the beginning
    s->len = 0;
    str_append(s, put, len);
}

/**
 * Destroy (free) a string object.
 * 
 * @param str *s    pointer to a string object
 * 
 * @return void
 */
void str_destroy(str *s)
{
    if (s == NULL) {
        return;
    }

    if (s->data) {
        free(s->data);
        s->data = NULL;
    }

    free(s);
    s = NULL;
}

/**
 *-------------------------------------------------------------------
 * String list
 *-------------------------------------------------------------------
 *
 * This section provides functions for creating a double linked
 * list of str objects. For example the str_split function uses
 * a string list for storing each part of the splitted string
 * in a node.
 * 
 *-------------------------------------------------------------------
 */ 

/**
 * Creates / initialises an empty string list.
 * 
 * @return strlist | NULL   pointer to a string list
 */
strlist *strlist_create()
{
    strlist *sl = malloc(sizeof(strlist));
    throw_mem_(sl);

    sl->size = 0;
    sl->head = NULL;
    sl->tail = NULL;

    return sl;

    error:
    return NULL;
}

/**
 * Push / append a string object at the end of the string list.
 * 
 * @param strlist *sl   pointer to a string list
 * @param str *value    pointer to a string object
 * 
 * @return NULL
 */
void *strlist_push(strlist *sl, str *value)
{
    strlistnode *sln = malloc(sizeof(strlistnode));
    throw_mem_(sln);

    sln->value = value;
    sln->next = NULL;
    sln->prev = NULL;

    // if list is empty, set head and tail to first node
    if (sl->head == NULL) {
        sl->head = sln;
        sl->tail = sln;   
    } else {
        sl->tail->next = sln;
        sln->prev = sl->tail;
        sl->tail = sln;
    }

    sl->size++;

    error:
    return NULL;
}

/**
 * Get the last element of a string list.
 * 
 * @param strlist *sl   pointer to a string list
 * 
 * @return str          pointer to a string object
 */
str *strlist_pop(strlist *sl)
{
    strlistnode *sln = sl->tail;

    sl->tail = sln->prev;

    return sln->value;
}

/**
 * Get the string object of a specific index in the string list.
 * 
 * @param strlist *sl   pointer to a string list
 * @param size_t index  0-based index of the string object
 * 
 * @return str | NULL   pointer to a string object
 */
str *strlist_at(strlist *sl, size_t index)
{
    // return if index is bigger than the actual size of the list
    if (sl->size - 1 < index) {
        return NULL;
    }

    strlistnode *sln = sl->head;

    // go to the node at the index
    for (size_t i = 0; i < index; i++) {
        sln = sln->next;
    }

    return sln->value;
}

/**
 * Destroy (free) a string list.
 * 
 * @param strlist *sl   pointer to a string list
 * 
 * @return void
 */
void strlist_destroy(strlist *sl)
{
    if (sl == NULL) {
        return;
    }

    // if list is empty, just free list and return
    if (strlist_empty(sl)) {
        free(sl);
        sl = NULL;
        return;
    }

    strlistnode *prev = sl->head;
    // need a second reference to free the node but not lose the next node
    strlistnode *node = NULL;

    while (prev != NULL) {
        node = prev->next;

        if (prev->value) {
            str_destroy(prev->value);
        }

        free(prev);

        prev = node;
    }

    free(sl);
    sl = NULL;
}