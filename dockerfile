# Use an official Ubuntu as a base image for the build stage
FROM ubuntu:24.04 AS builder

ENV APP_VERSION=1.0

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install necessary build tools
RUN apt-get update && apt-get install -y --no-install-recommends build-essential git cmake autoconf libtool pkg-config libboost-all-dev libgtest-dev libgmock-dev net-tools netcat-openbsd ninja-build && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /equityExchange

# Copy the entire project into the container
COPY . .

#RUN cmake . && make

# create a build directory and configure + build
# use Ninja for faster builds
# Installs into /usr/local
RUN cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    && cmake --build build --parallel \
    && cmake --install build

# ---- Runtime stage smaller image
FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install only runtime libraries the binary needs
RUN apt-get update && apt-get install -y --no-install-recommends libboost-all-dev net-tools netcat-openbsd && rm -rf /var/lib/apt/lists/*

# Copy the installed artifacts from the builder
COPY --from=builder /usr/local /usr/local

ENV PATH="/usr/local/bin:${PATH}"

EXPOSE 1234

CMD ["myMatchingEngine", "inputFile.csv"] #name and case need to be the same as in add_executable in CMakeLists.txt
 
#COPY entrypoint.sh /equityExchange/entrypoint.sh
#COPY equityexchangetest /equityExchange/equityexchangetest

#RUN chmod +x /equityExchange/entrypoint.sh #/equityExchange/equityexchangetest

#CMD ["./entrypoint.sh"]


#docker build -t mymatchingengine .
