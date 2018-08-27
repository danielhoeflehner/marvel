#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "marvel.h"
#include "util.h"
#include "str.h"

int env_init()
{
    FILE *f = fopen(".env", "r");
    throw_(f == NULL, "Could not open .env file.");

    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    int ret = 0;

    strlist *list = NULL;
    str *envval = NULL;
    str *env_key = NULL;
    str *env_value = NULL;

    /*
     * Read every line of the .env file 
     * and set the env variable.
     */ 
    while ((read = getline(&line, &len, f)) != -1) {
        str_copy(envval, line, read);

        // get rid of the <newline> char
        str_strip_nl(envval);

        list = str_split(envval, '=');
        env_key = strlist_at(list, 0);
        env_value = strlist_at(list, 1);
        
        // set the env variable (has to be in the form "name=value")
        // ret = putenv(str_data(envval));
        ret = setenv(str_data(env_key), str_data(env_value));
        if (ret != 0) {
            perror("Could not put env variable. [Error]");
            exit(-1);
        }

        strlist_destroy(list);
    }


    fclose(f);
    return 0;

    error:
    str_destroy(envval);
    if (f) fclose(f);
    return -1;
}