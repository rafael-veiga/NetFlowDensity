FROM compile1docker

WORKDIR /app

RUN apt-get update && apt-get install -y cmake make python3 python3-dev python3-pip pybind11-dev && pip3 install --break-system-packages pybind11 &&apt-get clean && rm -rf /var/lib/lists/*

CMD ["bash"]