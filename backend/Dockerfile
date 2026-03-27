FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    gdb \
    cmake \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    bison \
    flex \
    autoconf \
    automake \
    libtool \
    libcurl4-openssl-dev \
    libssl-dev \
    libpq-dev \
    libsodium-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh


RUN /opt/vcpkg/vcpkg install \
    asio \
    crow \
    curl \
    libenvpp \
    libpqxx


WORKDIR /backend

COPY . .

RUN cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -G Ninja

RUN cmake --build build

ENTRYPOINT ["./build/legal_chatbot"]
