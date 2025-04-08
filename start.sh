#!/bin/bash

# Se o argumento -pre for fornecido, não renomeia os arquivos e define o parâmetro para o Nextflow
if [ "$1" = "-pre" ]; then
    echo "Argumento '-pre' detectado: pulando a renomeação dos arquivos."
    NEXTFLOW_ARGS="--pre=FALSE"
else
    echo "Nenhum argumento '-pre' detectado: renomeando arquivos na pasta data."
    for file in data/*; do
        newfile=$(echo "$file" | sed 's/ /_/g')
        if [ "$file" != "$newfile" ]; then
            mv "$file" "$newfile"
        fi
    done
    NEXTFLOW_ARGS="--pre=TRUE"
fi

# Constroi a imagem Docker
docker build -t pre1docker dockers/pre1docker
docker build -f dockers/basedocker/Dockerfile -t basedocker .
docker run --rm \
  -v "$(pwd)/c_src:/project/c_src" \
  -v "$(pwd)/scripts:/project/scripts" \
  pre1docker \
  /bin/bash -c "cd /project/c_src && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make && cp bin/process_file ../../scripts/ && chmod +x ../../scripts/process_file"

nextflow run main.nf -resume $NEXTFLOW_ARGS
