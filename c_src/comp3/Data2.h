#ifndef DATA2_H
#define DATA2_H

class Data2
{
public:
    Data2();
    ~Data2();
    void convert(int n, char *file_in, char *file_out,float x1,float x2,float y1,float y2);

private:
    int n;
    int ncol;
    char *col_names;
    float *data;
    void get_pos(int* index,float* value, float* x,float* y);
};

#endif