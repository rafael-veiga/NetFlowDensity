#include "Data.h"
#include <iostream>

int main(int argc, char **argv)
{

    char *option = argv[1];
    int seed = 12345;

    if (argc < 3)
    {
        std::cout << "argments:\nr file_in file_out\ns number_sample_by_file file_out file1.dat file2.dat ...\n";
        return 1;
    }
    Data *data = new Data();
    if (argv[1][0] == 'r')
    {
        data->loadFile(argv[2]);
        if (data->save(argv[3]))
            return 1;
    }
    if (argv[1][0] == 's')
    {
        int nsample = std::atoi(argv[3]);
        if (data->getSample(argv[2], nsample, seed))
            return 1;
        if (data->save(argv[4]))
            return 1;
    }
}