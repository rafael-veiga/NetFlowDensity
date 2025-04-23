#!/bin/bash

# Se o argumento -pre for fornecido, não renomeia os arquivos e define o parâmetro para o Nextflow
if [ "$1" = "--pre" ]; then
    echo "Nenhum argumento '-pre' detectado: renomeando arquivos na pasta data."
    for file in data/*; do
        newfile=$(echo "$file" | sed 's/ /_/g')
        if [ "$file" != "$newfile" ]; then
            mv "$file" "$newfile"
        fi
    done
    NEXTFLOW_ARGS="--pre=TRUE"
else
    NEXTFLOW_ARGS="--pre=FALSE"
fi
ARGS_T=( "$@" )
FILTERED_ARGS=()
for a in "${ARGS_T[@]}"; do
  if [[ "$a" != "--pre" ]]; then
    FILTERED_ARGS+=( "$a" )
  fi
done
ARGS_T=( "${FILTERED_ARGS[@]}" )

FINAL_ARGS=( "$NEXTFLOW_ARGS" "${ARGS_T[@]}" )

# build docker images
docker build -f dockers/compile1docker/Dockerfile -t compile1docker .
docker run --rm -v "$(pwd)/c_src/comp1":/app compile1docker /bin/bash -c "g++ -std=c++17 -o3 read_csv.cpp Data.cpp List.cpp -o process_file"
mv "$(pwd)/c_src/comp1/process_file" "$(pwd)/scripts"
chmod +x "$(pwd)/scripts/process_file"

docker build -f dockers/compile2docker/Dockerfile -t compile2docker .
docker run --rm -v "$(pwd)/c_src/comp2":/app compile2docker bash -c 'c++ -O3 -Wall -Wextra -shared -std=c++17 -fPIC $(python3 -m pybind11 --includes) c_py.cpp -o pyfileconverter$(python3-config --extension-suffix)'
mv "$(pwd)"/c_src/comp2/pyfileconverter.* "$(pwd)/scripts"
chmod +x "$(pwd)"/scripts/pyfileconverter.*

docker build -f dockers/pre1docker/Dockerfile -t pre1docker .
docker build -f dockers/pydocker/Dockerfile -t pydocker .

nextflow run main.nf -resume ${FINAL_ARGS[@]} 

