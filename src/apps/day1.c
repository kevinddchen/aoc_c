#include <stdio.h>
#include <stdlib.h>


static const char* FILENAME = "files/day1.txt";

int main()
{
    FILE* fp;

    // open file
    fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", FILENAME);
        return EXIT_FAILURE;
    }

    int zero_counts = 0;  // counts number of times dial is pointing at '0'
    int position = 50;    // current dial position

    // iterate over each line, executing the rotation
    char buff[64];
    while (fgets(buff, sizeof buff, fp) != NULL) {
        char direction = buff[0];
        int num_clicks = atoi(buff + 1);

        if (direction == 'R') {
            position += num_clicks;
        } else if (direction == 'L') {
            position -= num_clicks;
        } else {
            fprintf(stderr, "Unexpected direction: %c\n", direction);
            return EXIT_FAILURE;
        }

        position %= 100;

        if (position == 0) {
            ++zero_counts;
        }
    }
    fclose(fp);

    printf("Day 1\n");
    printf("Part 1: %d\n", zero_counts);

    return EXIT_SUCCESS;
}
