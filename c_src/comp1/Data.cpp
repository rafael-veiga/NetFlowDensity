#include "Data.h"
#include "List.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

Data::Data() : size_ev(0), ncol(0), col(nullptr), value(nullptr) {}

Data::~Data()
{
    delete[] col;
    delete[] value;
}

int Data::get_pos(int lin, int col)
{
    return lin * this->ncol + col;
}

void Data::loadFile(char *file)
{
    int nlin = 0;
    int res[2];
    std::string line;
    char aux[30];
    this->getSizeDataFile(res, file);
    size_ev = res[0];
    ncol = res[1];
    value = new float[size_ev * ncol];
    col = new std::string[ncol];

    std::ifstream saida(file);
    getline(saida, line);
    std::stringstream ss(line);
    for (int i = 0; i < ncol; i++)
    {
        std::getline(ss, col[i], ',');
    }

    while (getline(saida, line))
    {
        int tam = line.length();
        int pos = 0;
        int pcos = 0;
        for (int i = 0; i < tam; i++)
        {
            if (line[i] == ',')
            {
                aux[pos] = 0;
                value[this->get_pos(nlin, pcos)] = std::atof(aux);
                pcos++;
                pos = 0;
            }
            else
            {
                aux[pos] = line[i];
                pos++;
            }
        }
        aux[pos] = 0;
        value[this->get_pos(nlin, pcos)] = std::atof(aux);
        nlin++;
    }
    saida.close();
}

int Data::getSizeDataFile(int *res, char *file)
{
    std::ifstream saida(file);
    std::string line;
    int count = 1;
    if (!saida.is_open()) {
        std::cerr << "Erro: arquivo não encontrado!" << std::endl;
        return 1;
    }

    getline(saida, line);
    for (char c : line)
    {
        if (c == ',')
            count++;
    }
    res[1] = count;
    count = 0;
    while (getline(saida, line))
    {
        count++;
    }
    res[0] = count;
    saida.close();
    return 0;
}

int Data::save(char *file)
{
    std::ofstream out(file, std::ios::binary);
    if (!out)
    {
        std::cerr << "Erro ao abrir o arquivo para escrita." << std::endl;
        return -1;
    }

    out.write(reinterpret_cast<char *>(&size_ev), sizeof(size_ev));
    out.write(reinterpret_cast<char *>(&ncol), sizeof(ncol));

    out.write(reinterpret_cast<char *>(value), sizeof(float) * size_ev * ncol);

    for (int i = 0; i < ncol; i++)
    {
        out.write(col[i].c_str(), col[i].size() + 1);
    }

    out.close();
    return 0;
}

int Data::loadData(char *file)
{
    std::ifstream in(file, std::ios::binary);
    if (!in)
    {
        std::cerr << "Erro ao abrir o arquivo para leitura." << std::endl;
        return -1;
    }

    // Leitura dos inteiros size_ev e ncol
    in.read(reinterpret_cast<char *>(&size_ev), sizeof(size_ev));
    in.read(reinterpret_cast<char *>(&ncol), sizeof(ncol));

    // Aloca e lê o vetor de floats (tamanho = size_ev * ncol)
    value = new float[size_ev * ncol];
    in.read(reinterpret_cast<char *>(value), sizeof(float) * size_ev * ncol);

    // Aloca o vetor de strings com tamanho ncol
    col = new std::string[ncol];

    // Leitura de cada string armazenada (cada uma terminada com '\0')
    for (int i = 0; i < ncol; i++)
    {
        std::string temp;
        char ch;
        // Lê caractere a caractere até encontrar o terminador '\0'
        while (in.get(ch) && ch != '\0')
        {
            temp.push_back(ch);
        }
        col[i] = temp;
    }

    in.close();
    return 0;
}

void Data::getSample(std::string *files, int nfiles, int n_ev, int seed)
{
    srand(seed);
    int total_events = n_ev * nfiles;
    int sample_index = 0;

    for (int i = 0; i < nfiles; i++)
    {
        if (processFile(files[i], n_ev, sample_index, total_events))
            std::cerr << "problem with file " << files[i] << std::endl;
    }
}

int Data::processFile(const std::string &filepath, int n_ev, int &sample_index, int total_events)
{
    std::ifstream in(filepath, std::ios::binary);
    if (!in)
    {
        std::cerr << "Erro ao abrir o arquivo: " << filepath << std::endl;
        return -1;
    }

    int file_size_ev = 0, file_ncol = 0;
    in.read(reinterpret_cast<char *>(&file_size_ev), sizeof(file_size_ev));
    in.read(reinterpret_cast<char *>(&file_ncol), sizeof(file_ncol));
 
    if (this->size_ev == 0)
    {
        this->ncol = file_ncol;
        this->size_ev = total_events;
        this->value = new float[total_events * file_ncol];

        std::streampos col_offset = 2 * sizeof(int) + file_size_ev * file_ncol * sizeof(float);
        in.seekg(col_offset, std::ios::beg);
        this->col = new std::string[file_ncol];
        for (int i = 0; i < file_ncol; i++)
        {
            std::string temp;
            char ch;
            while (in.get(ch) && ch != '\0')
                temp.push_back(ch);
            this->col[i] = temp;
        }
    }
    else
    {
        if (file_ncol != this->ncol)
        {
            std::cerr << "Inconsistência: número de colunas diferente no arquivo " << filepath << std::endl;
            in.close();
            return 1;
        }
        std::streampos col_offset = 2 * sizeof(int) + file_size_ev * file_ncol * sizeof(float);
        in.seekg(col_offset, std::ios::beg);
        for (int i = 0; i < file_ncol; i++)
        {
            std::string temp;
            char ch;
            while (in.get(ch) && ch != '\0')
                temp.push_back(ch);
            if (temp != this->col[i])
            {
                std::cerr << "Inconsistência: coluna " << i << " diferente no arquivo " << filepath << std::endl;
                in.close();
                return 1;
            }
        }
    }

    List positions(n_ev);
    for (int i = 0; i < n_ev; i++)
    {
        int r = rand() % file_size_ev;
        while (positions.add(r))
        {
            r = rand() % file_size_ev;
        }
    }

    for (int i = 0; i < n_ev; i++)
    {
        std::streampos event_offset = 2 * sizeof(int) + positions.get(i) * file_ncol * sizeof(float);
        in.seekg(event_offset, std::ios::beg);
        in.read(reinterpret_cast<char *>(&this->value[sample_index * file_ncol]), file_ncol * sizeof(float));
        sample_index++;
    }
    in.close();
    return 0;
}