#include <stdio.h>

#include <argparse/argparse.hpp>

int main(int argc, char** argv)
{
    argparse::ArgumentParser argparser("bspgen", "0.1.0", argparse::default_arguments::help, false);

    argparser.add_argument("-v", "-V", "--verbose").default_value(false).implicit_value(true);
    try
    {
        argparser.parse_args(argc, argv);
    }
    catch(const std::exception& err)
    {
        fprintf(stderr, "%s\n", err.what());
        return 1;
    }

    if(argparser["-v"] == true)
    {
        printf("verbose\n");
    }

    return 0;
}