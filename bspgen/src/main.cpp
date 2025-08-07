#include <stdio.h>

#include <argparse/argparse.hpp>

#include <utilslib.h>

int main(int argc, char** argv)
{
    argparse::ArgumentParser argparser("bspgen", "0.1.0", argparse::default_arguments::help, false);
    std::string infile, outfile;

    argparser.add_argument("-v", "--verbose").flag();

    argparser.add_argument("mapfile").help("the map file to use as input").required();
    argparser.add_argument("-o", "--output").help("the bsp file to use as output");

    try
    {
        argparser.parse_args(argc, argv);
    }
    catch(const std::exception& err)
    {
        fprintf(stderr, "%s\n", err.what());
        std::cerr << argparser; // i hate doing this >:(
        return 1;
    }

    infile = argparser.get("mapfile");
    infile = Utilslib::DefaultExtension(infile.c_str(), "map");
    if(argparser.present("--output"))
    {
        outfile = argparser.get("--output");
        outfile = Utilslib::DefaultExtension(outfile.c_str(), "bsp");
    }
    else
    {
        outfile = Utilslib::StripExtension(infile.c_str());
        outfile = Utilslib::AddExtension(outfile.c_str(), "bsp");
    }

    puts(infile.c_str());
    puts(outfile.c_str());

    return 0;
}