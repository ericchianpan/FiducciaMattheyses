#include <iostream>
#include "Floorplanner.h"
using namespace std;

int main(int argc, char *argv [])
{
    if(argc != 5)
    {
        cout << "Uesd ./[executable file name] [alpha value] [input.block name] [input.net name] [output file name]" << endl;
        exit(1);
    }

    Floorplanner floorplanner(argv[1]);
    floorplanner.outputfile = argv[4];
    floorplanner.parser(argv[2], argv[3]);
    floorplanner.simulated_annealing();
}
