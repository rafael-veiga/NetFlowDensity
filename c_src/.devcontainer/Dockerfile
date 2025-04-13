FROM debian:bullseye-slim

# Atualiza repositórios e instala dependências necessárias, como o gdb e outras ferramentas
RUN apt-get update && apt-get install -y \
    gdb \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Copie seus arquivos e configure seu ambiente conforme necessário
#WORKDIR /workspace