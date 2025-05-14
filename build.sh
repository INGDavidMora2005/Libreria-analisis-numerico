#!/bin/bash

echo "==> Compilando clase Java y generando encabezado JNI..."
javac -h . analisisNumerico.java

echo "==> Compilando código en C con JNI..."
gcc -c -fPIC -I/usr/lib/jvm/java-17-openjdk-amd64/include \
            -I/usr/lib/jvm/java-17-openjdk-amd64/include/linux \
            analisisNumerico.c tinyexpr.c

echo "==> Generando y enlazando biblioteca compartida con cJSON y tinyexpr..."
gcc -shared -o libanalisisnumerico.so analisisNumerico.o tinyexpr.o -lcjson

echo "✅ Biblioteca 'libanalisisnumerico.so' generada correctamente."


