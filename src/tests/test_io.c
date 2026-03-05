#include "io.h"

static const char FILENAME[] = "src/tests/test_io.c";

void test_io_readlines()
{
    printf("test_io_readlines: ");
    // read this source file and check a few things about it.
    // this test may need to be altered when this source file is modified.
    Vector lines = {};
    io_readlines(FILENAME, 1024, &lines);

    assert(lines.count == 25);
    assert(strcmp(((char**)lines.items)[0], "#include \"io.h\"") == 0);
    assert(strcmp(((char**)lines.items)[4], "void test_io_readlines()") == 0);

    vector_free_arrays(&lines);
    printf("Pass!\n");
}

int main()
{
    test_io_readlines();
    return EXIT_SUCCESS;
}
