FROM linuxcontainers/debian-slim:12.5

WORKDIR /app

RUN apt-get update && apt-get install -y python3 python3-pip python3-numpy && apt-get clean && rm -rf /var/lib/lists/* /var/cache/apt/archives/*

RUN pip3 install --no-cache-dir --break-system-packages pacmap scikit-learn

CMD ["bash"]