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
    NEXTFLOW_ARGS=""
fi

# Constroi a imagem Docker
docker build -t pre1docher dockers/pre1docker

# Executa o Nextflow passando os argumentos definidos (se houver)
nextflow run main.nf -resume $NEXTFLOW_ARGS
