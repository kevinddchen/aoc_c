#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

/**
 * Read lines from a file and return as vector of strings, striping trailing '\n' character if any.
 * @param filename File to read.
 * @param count Maximum number of characters per line (including null character).
 * @param lines Output vector of strings. All strings need to be freed to avoid memory leak.
 */
void io_readlines(const char* filename, size_t count, Vector* lines) {
    vector_init(lines, sizeof(char*));

    FILE* fp = fopen(filename, "r");
    assert(fp != NULL);

    char* buff = calloc(count, sizeof(char));
    assert(buff != NULL);

    while (fgets(buff, count, fp) != NULL) {
        size_t length = strlen(buff);
        assert(length >= 1);
        assert(buff[length] == '\0');

        if (buff[length - 1] == '\n') {
            buff[length - 1] = '\0';
            length--;
        }

        // copy string plus null character
        char* line = malloc((length + 1) * sizeof(char));
        assert(line != NULL);

        memcpy(line, buff, (length + 1) * sizeof(char));
        vector_push_back(lines, &line);
    }

    free(buff);
    fclose(fp);
    fp = NULL;
}
