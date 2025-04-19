FROM linuxcontainers/debian-slim:12.5

WORKDIR /app

RUN apt-get update && apt-get install -y dos2unix && apt-get clean && rm -rf /var/lib/apt/lists/* /var/cache/apt/archives/*.deb

CMD ["bash"]