# Stage 1: Build environment
FROM openjdk:21-slim as builder

# 1. Install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    gcc \
    make \
    libcjson-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 2. Copy native code
COPY libanalisis /app/libanalisis
COPY cJSON-1.7.15/cJSON.c cJSON-1.7.15/cJSON.h /app/libanalisis/

# 3. Set Java include paths
ENV JAVA_HOME=/usr/local/openjdk-21
ENV CPATH=$JAVA_HOME/include:$JAVA_HOME/include/linux

# 4. Compile native library
RUN cd /app/libanalisis && \
    gcc -shared -fPIC -I. -I$JAVA_HOME/include -I$JAVA_HOME/include/linux \
    analisisNumerico.c tinyexpr.c cJSON.c -o libanalisisnumerico.so && \
    cp libanalisisnumerico.so /usr/lib/ && \
    chmod 755 /usr/lib/libanalisisnumerico.so

# 5. Copy Java application
COPY Aplicacion /app/Aplicacion

# 6. Compile Java classes
RUN javac /app/Aplicacion/analisisNumerico.java

# Stage 2: Runtime image
FROM openjdk:21-slim

# 1. Copy native library from builder
COPY --from=builder /usr/lib/libanalisisnumerico.so /usr/lib/
RUN chmod 755 /usr/lib/libanalisisnumerico.so

# 2. Copy Java classes from builder
COPY --from=builder /app/Aplicacion /app/Aplicacion

# 3. Environment variables
ENV LD_LIBRARY_PATH=/usr/lib
ENV JAVA_OPTS="-Djava.library.path=/usr/lib"
ENV CLASSPATH=/app/Aplicacion

WORKDIR /app
EXPOSE 8080

# 4. Run with interactive terminal and tty
ENTRYPOINT ["java", "analisisNumerico"]
