#include <std/textutils/textutils.h>

bool textutils_ischarcancelled(char* str, char* c)
{
    int i;

    bool canceled;

    i = 0;
    canceled = false;
    do
    {
        c--;

        if(*c != '\\' || i > 1)
            break;
    
        canceled = !canceled;
    
        i++;
    } while(c > str);
    
    return canceled;
}