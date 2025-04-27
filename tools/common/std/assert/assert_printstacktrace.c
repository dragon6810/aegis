#ifdef DEBUG

#include <std/assert/assert.h>

#include <execinfo.h>
#include <unistd.h>
#include <limits.h>

#ifdef APPLE
    #include <mach-o/dyld.h>
#endif

void assert_printstacktrace(void)
{
    int i;

    void **buff;
    char **symbols;
    unsigned int size;
    int stacksize;

    size = 16;
    buff = 0;
    while (1) 
    {
        if(buff)
            buff = realloc(buff, size * sizeof(void*));
        else
            buff = malloc(size * sizeof(void*));

        stacksize = backtrace(buff, size);
        if (stacksize < size)
            break;

        size <<= 2;
    }

    symbols = backtrace_symbols(buff, stacksize);
    fprintf(stderr, "Stack trace:\n");
    /* start at 1 so you dont print this function in the call stack */
    for (i=1; i<stacksize; i++)
        fprintf(stderr, "%s\n", symbols[i]);
    
    free(symbols);
    free(buff);
}

#endif