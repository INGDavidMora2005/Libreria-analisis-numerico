#!/bin/bash

echo "==> Compilando clase Java y generando encabezado JNI..."
javac -h . analisisNumerico.java

echo "==> Compilando código en C con JNI..."
gcc -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -c analisisNumerico.c -o analisisNumerico.o -lm

echo "==> Generando biblioteca compartida..."
gcc -shared -o libanalisisnumerico.so analisisNumerico.o -lm

echo "✅ Biblioteca 'libanalisisnumerico.so' generada correctamente."

