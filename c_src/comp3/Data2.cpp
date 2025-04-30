#include "Data2.h"
#include "fstream"
#include <iostream>

Data2::Data2()
{
    this->col_names = 0;
    this->ncol = 0;
    this->n = n;
    this->data = 0;
}

Data2::~Data2()
{
    if (this->data)
    {
        delete[] this->data;
        delete[] this->col_names;
    }
}

void Data2::convert(int n, char *file_in, char *file_out, float x1, float x2, float y1, float y2)
{
    char aux[20];
    int nevents;
    int i_aux;
    float f_aux;
    float *d_aux;
    float x_min, x_max, y_min, y_max;
    /*  file_in
    int  num_rows
    int  num_cols
    float[num_rows * num_cols]            // matrix
    char[]  col_name_0 '\0' … col_name_N '\0'
*/
    std::ifstream saida(file_in, std::ios::binary);
    if (!saida)
    {
        std::cout << "file not accessed\n";
        return;
    }
    this->n = n;
    saida.read(reinterpret_cast<char *>(&nevents), sizeof(int));
    saida.read(reinterpret_cast<char *>(&this->ncol), sizeof(int));
    d_aux = new float[nevents * this->ncol];
    saida.read(reinterpret_cast<char *>(d_aux), sizeof(float) * nevents * this->ncol);
    x_min = d_aux[0];
    x_max = d_aux[0];
    y_min = d_aux[1];
    y_max = d_aux[1];

    for (int i = this->ncol; i < nevents * this->ncol; i = i + this->ncol)
    {
        if (d_aux[i] < x_min)
        {
            x_min = d_aux[i];
        }
        else
        {
            if (d_aux[i] > x_max)
            {
                x_max = d_aux[i];
            }
        }
        if (d_aux[i + 1] < y_min)
        {
            y_min = d_aux[i + 1];
        }
        else
        {
            if (d_aux[i + 1] > y_max)
            {
                y_max = d_aux[i + 1];
            }
        }
    }

    this->ncol = this->ncol - 2;
    float *x = new float[n - 1];
    float *y = new float[n - 1];
    f_aux = (x2 - x1) / (n - 2);
    i_aux = n - 1;
    for (int i = 0; i < i_aux; i++)
    {
        x[i] = x1 + (f_aux * i);
    }
    f_aux = (y2 - y1) / (n - 2);
    for (int i = 0; i < i_aux; i++)
    {
        y[i] = y1 + (f_aux * i);
    }
    i_aux = n * n * (this->ncol + 1);
    this->data = new float[i_aux];
    for (int i = 0; i < i_aux; i++)
    {
        this->data[i] = 0.0;
    }
    for (int i = 0; i < nevents; i++)
    {
        int pos[2];
        i_aux = i * (this->ncol + 2);
        this->get_pos(pos, &d_aux[i_aux], x, y);
        //std::cout<<d_aux[i_aux]<<"|"<<d_aux[i_aux+1] << " = (" << pos[0] << " : " << pos[1] << ")\n";
        this->data[pos[0]*this->n+ pos[1]] = this->data[pos[0]*this->n+ pos[1]]+1.0;
        for(int j=0;j<this->ncol;j++){
            i_aux = this->n*this->n*(j+1);
            i_aux += pos[0]*this->n+ pos[1];
            this->data[i_aux]+= d_aux[i * (this->ncol + 2)+j+2];
        }
    }
    //* get mean mark
    for(int col=0;col<this->ncol;col++){
        int t_col = (col+1)*this->n*this->n;
        for(int i=0;i<this->n;i++){
            i_aux = i*this->n;
            for(int j=0;j<this->n;j++){
                if(this->data[i_aux+j]!=0.0){
                    this->data[t_col+i_aux+j] = this->data[t_col+i_aux+j]/this->data[i_aux+j];
                }
            }
        }
    }

    //! add colunas name
    delete[] x;
    delete[] y;
    delete[] d_aux;
    saida.close();
}

void Data2::get_pos(int *index, float* value, float *x, float *y)
{
    int i_aux = this->n -1;
    //* x
    index[0] = -1;
    for(int i=0;i<i_aux;i++){
        if(value[0]<x[i]){
            index[0] = i;
            break;
        }
    }
    if(index[0]==-1){
        index[0] = i_aux;
    }
    //* y
    index[1] = -1;
    for(int i=0;i<i_aux;i++){
        if(value[1]<x[i]){
            index[1] = i;
            break;
        }
    }
    if(index[1]==-1){
        index[1] = i_aux;
    }



}