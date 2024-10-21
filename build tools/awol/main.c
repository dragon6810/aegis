#include "wadscript.h"

int main(int argc, char** argv)
{
	WS_Open();
	WS_Parse();
    WS_Run();
    
    return 0;
}
