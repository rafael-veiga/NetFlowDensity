#include "Data.h"
#include <iostream>

int main(int argc, char** argv){
char* infile = argv[1];
char* outfile = argv[2];

Data* data = new Data();
data->getSample(".",10,100);
data->save("sample3.dat");
}