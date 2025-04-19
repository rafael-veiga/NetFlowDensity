FROM linuxcontainers/debian-slim:12.5

WORKDIR /app

RUN apt-get update && apt-get install -y g++ && apt-get clean && rm -rf /var/lib/lists/* /var/cache/apt/archives/*

CMD ["bash"]