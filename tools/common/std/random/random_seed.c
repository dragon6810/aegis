#include <std/random/random.h>

#include <time.h>
#include <stdlib.h>

void random_seed(void)
{
    srand(time(NULL));
}
