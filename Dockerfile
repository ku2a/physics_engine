# ========= build: SFML 3 + app =========
FROM ubuntu:24.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive

# Herramientas + deps (incluye git y pkg-config)
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates wget \
    build-essential g++ cmake git pkg-config \
    # X11 + GL + udev
    libx11-dev libxrandr-dev libudev-dev libgl1-mesa-dev \
    libxcursor-dev libxi-dev libxinerama-dev \
    # Texto / Audio
    libfreetype6-dev libharfbuzz-dev \
    libopenal-dev libsndfile1-dev \
    && update-ca-certificates && rm -rf /var/lib/apt/lists/*

# (Opcional) comprueba que git está realmente
RUN which git && git --version

ARG SFML_REF=master
WORKDIR /opt

# Descarga SFML 3.x (puedes cambiar SFML_REF a un tag/commit)
RUN wget -O sfml.tar.gz https://codeload.github.com/SFML/SFML/tar.gz/${SFML_REF} && \
    mkdir sfml && tar -xzf sfml.tar.gz --strip-components=1 -C sfml && rm sfml.tar.gz && \
    cmake -S sfml -B sfml/build -DCMAKE_BUILD_TYPE=Release \
          -DSFML_BUILD_EXAMPLES=OFF -DSFML_BUILD_DOC=OFF && \
    cmake --build sfml/build --target install -j && \
    ldconfig

# Para que pkg-config vea los .pc de /usr/local

ENV PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/lib64/pkgconfig

WORKDIR /app
COPY main.cpp .
COPY particle.h constants.h ./

# (Opcional) inspección para ver qué devuelve pkg-config
RUN pkg-config --modversion sfml-graphics && pkg-config --libs sfml-graphics

# Compila tu app (añadimos -L y rpath)
RUN g++ -std=gnu++17 -O2 -Wall -Wextra \
    main.cpp -o app \
    $(pkg-config --cflags sfml-graphics sfml-window sfml-system sfml-audio) \
    -L/usr/local/lib -Wl,-rpath,/usr/local/lib \
    $(pkg-config --libs   sfml-graphics sfml-window sfml-system sfml-audio)


# ========= runtime: mínimo =========
FROM ubuntu:24.04 AS runtime
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    # X11 + GL + udev (runtime)
    libx11-6 libxrandr2 libudev1 libgl1 \
    libxcursor1 libxi6 libxinerama1 \
    # Texto / Audio (runtime)
    libfreetype6 libharfbuzz0b \
    libopenal1 libsndfile1 \
    && update-ca-certificates && rm -rf /var/lib/apt/lists/*

# Copiamos lo instalado por SFML (libs en /usr/local)
WORKDIR /usr/local
COPY --from=builder /usr/local/ /usr/local/

WORKDIR /app
COPY --from=builder /app/app /app/app

CMD ["/app/app"]
