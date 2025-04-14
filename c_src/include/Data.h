#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>

class Data
{
public:
    Data();
    ~Data();
    void loadFile(char* file);
    int save(char* file);
    int loadData(char* file);
    void getSample(std::string *files, int nfiles, int n_ev, int seed);

private:
    int size_ev;
    int ncol;
    std::string *col;
    float *value;

    int getSizeDataFile(int* res, char* file);
    int get_pos(int lin, int col);
    int processFile(const std::string& filepath, int n_ev, int& sample_index, int total_events);
};

#endif // DATA_H