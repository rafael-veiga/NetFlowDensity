#include "Data.h"
#include "List.h"
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

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
    std::cout << "a1\n";
    int nlin = 0;
    int res[2];
    std::string line;
    char aux[30];
    this->getSizeDataFile(res, file);
    std::cout << "a2\n";
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
    std::cout << "a3\n";
}

void Data::getSizeDataFile(int *res, char *file)
{
    std::ifstream saida(file);
    std::string line;
    int count = 1;

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

int Data::getSample(char *folder, int n_ev, int seed)
{
    // Inicializa o gerador de números aleatórios com a semente fornecida.
    srand(seed);

    int fileCount = 0;
    // Conta quantos arquivos .dat existem na pasta.
    for (const auto &entry : fs::directory_iterator(folder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".dat")
            fileCount++;
    }

    if (fileCount == 0)
    {
        std::cerr << "Nenhum arquivo .dat encontrado na pasta." << std::endl;
        return -1;
    }

    // Calcula o total de eventos a serem amostrados: n_ev para cada arquivo.
    int total_events = n_ev * fileCount;
    // Inicialmente, o objeto Data está vazio (size_ev == 0), ou seja, sem dados carregados.
    int sample_index = 0;

    // Itera sobre os arquivos .dat e processa cada um.
    for (const auto &entry : fs::directory_iterator(folder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".dat")
        {
            int ret = processFile(entry.path().string(), n_ev, sample_index, total_events);
            if (ret != 0)
                return ret; // Se ocorrer algum erro ou inconsistência.
        }
    }
    return 0;
}

// Método privado: processa cada arquivo .dat individualmente.
int Data::processFile(const std::string &filepath, int n_ev, int &sample_index, int total_events)
{
    std::ifstream in(filepath, std::ios::binary);
    if (!in)
    {
        std::cerr << "Erro ao abrir o arquivo: " << filepath << std::endl;
        return -1;
    }

    // Lê o cabeçalho: número de eventos do arquivo e número de colunas.
    int file_size_ev = 0, file_ncol = 0;
    in.read(reinterpret_cast<char *>(&file_size_ev), sizeof(file_size_ev));
    in.read(reinterpret_cast<char *>(&file_ncol), sizeof(file_ncol));

    // Se o objeto Data estiver vazio, inicializa os atributos com base no primeiro arquivo.
    if (this->size_ev == 0)
    {
        this->ncol = file_ncol;
        this->size_ev = total_events;
        // Aloca memória para armazenar todos os eventos amostrados.
        this->value = new float[total_events * file_ncol];

        // Lê o vetor de strings 'col' que está armazenado após os dados dos eventos.
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
        // Verifica se o número de colunas é consistente entre os arquivos.
        if (file_ncol != this->ncol)
        {
            std::cerr << "Inconsistência: número de colunas diferente no arquivo " << filepath << std::endl;
            in.close();
            return 1;
        }
        // Verifica se os nomes das colunas são iguais.
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

    // Gera uma lista com n_ev índices aleatórios (entre 0 e file_size_ev - 1).
    List positions(n_ev);
    for (int i = 0; i < n_ev; i++)
    {
        int r = rand() % file_size_ev;
        while (positions.add(r))
        {
            r = rand() % file_size_ev;
        }
    }
    //positions.print();
    // Ordena os índices para que as leituras sejam sequenciais e otimizadas.

    for (int i=0;i < n_ev; i++){
        std::streampos event_offset = 2 * sizeof(int) + positions.get(i) * file_ncol * sizeof(float);
        in.seekg(event_offset, std::ios::beg);
        // Lê os dados do evento (um bloco de file_ncol floats) e armazena no vetor.
        in.read(reinterpret_cast<char *>(&this->value[sample_index * file_ncol]), file_ncol * sizeof(float));
        sample_index++;
    }
    in.close();
    return 0;
}