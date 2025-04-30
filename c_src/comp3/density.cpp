#include <iostream>
#include "Data2.h"

// n, file_in, file_out
int main(int argc, char **argv)
{
    Data2 data;
    if (argc != 8)
    {
        std::cerr << "arguments missing\nTo execute use:\n\tdensity n file_in file_out x1 x2 y1 y2\n";
        return 1;
    }
    int n = std::atoi(argv[1]);
    float x1 = std::atof(argv[4]);
    float x2 = std::atof(argv[5]);
    float y1 = std::atof(argv[6]);
    float y2 = std::atof(argv[7]);
    data.convert(n, argv[2], argv[3], x1, x2, y1, y2);
}