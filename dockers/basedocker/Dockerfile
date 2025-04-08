FROM alpine:latest

RUN apk update && apk add --no-cache build-base bash

WORKDIR /app

COPY scripts/process_file /app/process_file
RUN chmod +x /app/process_file

# Comando padrão que inicia o Bash
CMD ["/bin/bash"]
