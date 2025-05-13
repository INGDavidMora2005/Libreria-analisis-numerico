#include <jni.h>
#include <stdio.h>
#include <math.h>
#include "analisisNumerico.h"

// Función para lanzar excepciones desde C a Java
void throwException(JNIEnv *env, const char *msg) {
    jclass exceptionClass = (*env)->FindClass(env, "java/lang/Exception");
    if (exceptionClass == NULL) {
        return; // No se pudo encontrar la clase
    }
    (*env)->ThrowNew(env, exceptionClass, msg);
}

// Función g(x) para Punto Fijo
double g(double x) {
    return cos(x);
}

// Función f(x) para Bisección y Newton-Raphson
double f(double x) {
    return cos(x) - x;
}

// Derivada de f(x) para Newton-Raphson
double df(double x) {
    return -sin(x) - 1;
}

JNIEXPORT jdouble JNICALL Java_analisisNumerico_puntoFijo
  (JNIEnv *env, jclass cls, jdouble x0, jdouble tol, jint maxIter) {
    double x = x0;
    for (int i = 0; i < maxIter; i++) {
        double x1 = g(x);
        if (fabs(x1 - x) < tol) {
            return x1;
        }
        x = x1;
    }

    // Lanzamos una excepción si no se encuentra la raíz dentro del número de iteraciones
    throwException(env, "No se encontró la raíz en el número máximo de iteraciones.");
    return 0.0; // Esta línea no se alcanzará nunca si la excepción se lanza correctamente.
}

JNIEXPORT jdouble JNICALL Java_analisisNumerico_biseccion
  (JNIEnv *env, jclass cls, jdouble a, jdouble b, jdouble tol, jint maxIter) {

    double fa = f(a);
    double fb = f(b);

    if (fa * fb > 0) {
        // Lanzamos una excepción si los signos de f(a) y f(b) no son opuestos
        throwException(env, "f(a) y f(b) deben tener signos opuestos.");
        return NAN;
    }

    double c;
    for (int i = 0; i < maxIter; i++) {
        c = (a + b) / 2;
        double fc = f(c);

        if (fabs(fc) < tol || (b - a) / 2 < tol) {
            return c;
        }

        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
    }

    // Lanzamos una excepción si no se encuentra la raíz dentro del número de iteraciones
    throwException(env, "No se encontró la raíz en el número máximo de iteraciones.");
    return 0.0; // Esta línea no se alcanzará nunca si la excepción se lanza correctamente.
}

JNIEXPORT jdouble JNICALL Java_analisisNumerico_newtonRaphson
  (JNIEnv *env, jclass cls, jdouble x0, jdouble tol, jint maxIter) {

    double x = x0;
    for (int i = 0; i < maxIter; i++) {
        double fx = f(x);
        double dfx = df(x);

        if (fabs(dfx) < 1e-10) {
            // Lanzamos una excepción si la derivada es muy pequeña
            throwException(env, "Derivada demasiado pequeña en Newton-Raphson.");
            return NAN;
        }

        double x1 = x - fx / dfx;

        if (fabs(x1 - x) < tol) {
            return x1;
        }

        x = x1;
    }

    // Lanzamos una excepción si no se encuentra la raíz dentro del número de iteraciones
    throwException(env, "No se encontró la raíz en el número máximo de iteraciones.");
    return 0.0; // Esta línea no se alcanzará nunca si la excepción se lanza correctamente.
}

