#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <fstream>
#include <string>

namespace py = pybind11;

/*  Formato do arquivo
    int  num_rows
    int  num_cols
    float[num_rows * num_cols]            // matriz linha‑major
    char[]  col_name_0 '\0' … col_name_N '\0'
*/

// ---------- leitura ----------
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

    return py::make_tuple(std::move(np_array), std::move(col_names));
}

// ---------- gravação ----------
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
}

// ------------------- módulo --------------------
PYBIND11_MODULE(pyfileconverter, m)
{
    m.doc() = "Conversão binário <-> NumPy (versão focada em desempenho)";
    m.def("file_to_numpy", &file_to_numpy,
          py::arg("filename"),
          "Lê arquivo binário e devolve (ndarray float32, lista de nomes)");
    m.def("numpy_to_file", &numpy_to_file,
          py::arg("filename"), py::arg("np_array"), py::arg("col_names"),
          "Grava ndarray float32 + nomes de colunas em arquivo binário");
}
