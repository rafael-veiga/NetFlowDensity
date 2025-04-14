#include "Data.h"
#include <iostream>
#include <string>

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
        std::string *files_in;
        int nfiles = 0;
        int nsample = std::atoi(argv[2]);
        files_in = new std::string[argc-4];
        nfiles=0;
        for (int i = 4; i < argc; i++)
        {
            files_in[nfiles] = argv[i];
            nfiles++;
        }
        data->getSample(files_in, nfiles, nsample,seed);
        if (data->save(argv[3]))
            return 1;
        delete[] files_in;
    }
}