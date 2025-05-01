#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <fstream>
#include <string>

namespace py = pybind11;

py::tuple file_to_numpy(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    int num_rows = 0, num_cols = 0;
    file.read(reinterpret_cast<char*>(&num_rows), sizeof(int));
    file.read(reinterpret_cast<char*>(&num_cols), sizeof(int));

    // aloca o bloco contíguo (vai ser “doado” ao NumPy, sem cópia)
    float* data = new float[num_rows * num_cols];
    file.read(reinterpret_cast<char*>(data),
              sizeof(float) * static_cast<size_t>(num_rows) * num_cols);

    /* Capsula que garante delete[] quando o array NumPy for destruído */
    py::capsule free_when_done(data, [](void* f) {
        delete[] static_cast<float*>(f);
    });

    /* Cria o np.ndarray 2‑D sem copiar:   shape = (num_rows, num_cols)
       strides = (num_cols*sizeof(float), sizeof(float))                */
    py::array np_array(
        {num_rows, num_cols},
        {static_cast<ssize_t>(num_cols * sizeof(float)),
         static_cast<ssize_t>(sizeof(float))},
        data,
        free_when_done);

    // ------------ nomes das colunas -------------
    py::list col_names;
    for (int c = 0; c < num_cols; ++c) {
        std::string name;
        char ch;
        while (file.get(ch) && ch != '\0') name.push_back(ch);
        col_names.append(std::move(name));
    }
    file.close();
    return py::make_tuple(std::move(np_array), std::move(col_names));   
}

void numpy_to_file(const std::string& filename,
                   py::array_t<float, py::array::c_style | py::array::forcecast> np_array,
                   py::list col_names)
{
    py::buffer_info buf = np_array.request();           // já é C‑contíguo
    int num_rows = static_cast<int>(buf.shape[0]);
    int num_cols = static_cast<int>(buf.shape[1]);

    std::ofstream file(filename, std::ios::binary);

    file.write(reinterpret_cast<char*>(&num_rows), sizeof(int));
    file.write(reinterpret_cast<char*>(&num_cols), sizeof(int));
    file.write(reinterpret_cast<char*>(buf.ptr),
               sizeof(float) * static_cast<size_t>(num_rows) * num_cols);

    for (py::size_t i = 0; i < col_names.size(); ++i) {
        const std::string& name = col_names[i].cast<std::string>();
        file.write(name.c_str(), name.size());
        file.put('\0');
    }
    file.close(); 
}

/* ----------------------------------------------------
 * Formato .dad (NOVO)
 *
 *   int   n            -> dimensão de cada matriz n×n
 *   int   ncol         -> número de “colunas” (grupos de matrizes)
 *   float data[...]    -> (ncol+1)*n*n valores 32-bit, linearizados
 *                         coluna0 (lin0 … linN-1), coluna1 (lin0 …) …
 *   char  colname[]    -> ncol strings, separadas por '\0'
 * ---------------------------------------------------- */

/* ---------- leitura ---------- */
py::tuple dad_to_numpy(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Não foi possível abrir " + filename);

    int n = 0, ncol = 0;
    file.read(reinterpret_cast<char*>(&n),    sizeof(int));
    file.read(reinterpret_cast<char*>(&ncol), sizeof(int));

    std::size_t nmat = static_cast<std::size_t>(ncol) + 1;   // ncol+1 matrizes n×n
    std::size_t total = nmat * n * n;

    float* data = new float[total];
    file.read(reinterpret_cast<char*>(data), total * sizeof(float));

    /* liberação automática do buffer quando o ndarray deixar de existir */
    py::capsule free_when_done(data, [](void* p){ delete[] static_cast<float*>(p); });

    /* shape (nmat, n, n)   ––   strides C-contíguos                         */
    py::array array3d(
        { static_cast<ssize_t>(nmat),
          static_cast<ssize_t>(n),
          static_cast<ssize_t>(n) },
        { static_cast<ssize_t>(n * n * sizeof(float)),
          static_cast<ssize_t>(n * sizeof(float)),
          static_cast<ssize_t>(sizeof(float)) },
        data,
        free_when_done);

    /* ----------- nomes das colunas ----------- */
    py::list col_names;
    for (int i = 0; i < ncol; ++i) {
        std::string name;
        char ch;
        while (file.get(ch) && ch != '\0')
            name.push_back(ch);
        col_names.append(std::move(name));
    }
file.close();
    /* A tupla exigida: (lista_de_colunas, ndarray_3d) */
    return py::make_tuple(std::move(col_names), std::move(array3d));
}

/* ---------- gravação ---------- */
void numpy_to_dad(const std::string& filename,
                  py::list col_names,
                  py::array_t<float, py::array::c_style | py::array::forcecast> array3d)
{
    /* validações básicas */
    if (array3d.ndim() != 3)
        throw std::runtime_error("array deve ser 3-D (nmat, n, n)");

    py::buffer_info buf = array3d.request();     // já está C-contíguo
    int nmat = static_cast<int>(buf.shape[0]);
    int n    = static_cast<int>(buf.shape[1]);

    if (buf.shape[1] != buf.shape[2])
        throw std::runtime_error("As duas últimas dimensões devem ser iguais (n×n)");

    int ncol = static_cast<int>(col_names.size());

    /* Opcional: exigir que nmat == ncol+1.  Comente se isso não se aplicar */
    if (nmat != ncol + 1) {
        throw std::runtime_error("Esperava nmat == ncol + 1, mas recebeu " +
                                 std::to_string(nmat) + " vs " + std::to_string(ncol));
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Não foi possível criar " + filename);

    /* cabeçalho */
    file.write(reinterpret_cast<char*>(&n),    sizeof(int));
    file.write(reinterpret_cast<char*>(&ncol), sizeof(int));

    /* dados (bloco contíguo) */
    std::size_t total = static_cast<std::size_t>(nmat) * n * n;
    file.write(reinterpret_cast<char*>(buf.ptr), total * sizeof(float));

    /* nomes de coluna, cada um separado por '\0' */
    for (py::size_t i = 0; i < col_names.size(); ++i) {
        std::string name = col_names[i].cast<std::string>();
        file.write(name.c_str(), name.size());
        file.put('\0');
    }
    file.close();
}

/* ------------------- módulo -------------------- */
PYBIND11_MODULE(pyfileconverter, m)
{
    m.doc() = "Conversão binário <-> NumPy ";

    m.def("file_to_numpy", &file_to_numpy,
          py::arg("filename"),
          "Lê arquivo binário e devolve (ndarray float32, lista de nomes)");
    m.def("numpy_to_file", &numpy_to_file,
          py::arg("filename"), py::arg("np_array"), py::arg("col_names"),
          "Grava ndarray float32 + nomes de colunas em arquivo binário");

    m.def("dad_to_numpy", &dad_to_numpy,
          py::arg("filename"),
          "Lê arquivo .dad e devolve (lista de nomes, ndarray float32 3-D)");

    m.def("numpy_to_dad", &numpy_to_dad,
          py::arg("filename"), py::arg("col_names"), py::arg("np_array"),
          "Grava (col_names, ndarray float32 3-D) no formato .dad");
}
