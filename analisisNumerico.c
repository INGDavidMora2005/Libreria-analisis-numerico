#include <cjson/cJSON.h>
#include <jni.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "tinyexpr.h"
#include "analisisNumerico.h"

// Función para imprimir el resultado en formato JSON
void imprimirResultadoJSON(const char* metodo, double resultado, int exito, const char* mensajeError) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "metodo", metodo);
    cJSON_AddBoolToObject(json, "exito", exito);

    if (exito) {
        cJSON_AddNumberToObject(json, "resultado", resultado);
    } else {
        cJSON_AddStringToObject(json, "error", mensajeError);
    }

    char *jsonStr = cJSON_Print(json);
    printf("%s\n", jsonStr);
    cJSON_Delete(json);
    free(jsonStr);
}

// Función para lanzar excepciones desde C a Java
void throwException(JNIEnv *env, const char *msg) {
    jclass exceptionClass = (*env)->FindClass(env, "java/lang/Exception");
    if (exceptionClass == NULL) {
        return; // No se pudo encontrar la clase
    }
    (*env)->ThrowNew(env, exceptionClass, msg);
}

// Punto Fijo
JNIEXPORT jdouble JNICALL Java_analisisNumerico_puntoFijo
  (JNIEnv *env, jobject obj, jdouble x0, jdouble tol, jint maxIter, jstring funcion) {

    const char *gx = (*env)->GetStringUTFChars(env, funcion, NULL);
    double x = x0;
    double variable;

    te_variable vars[] = {{"x", &variable}};
    te_expr *expr = te_compile(gx, vars, 1, NULL);
    if (!expr) {
        const char* msg = "Error al compilar la función g(x).";
        imprimirResultadoJSON("puntoFijo", NAN, 0, msg);
        throwException(env, msg);
        return NAN;
    }

    for (int i = 0; i < maxIter; i++) {
        variable = x;
        double x1 = te_eval(expr);
        if (fabs(x1 - x) < tol) {
            te_free(expr);
            (*env)->ReleaseStringUTFChars(env, funcion, gx);
            imprimirResultadoJSON("puntoFijo", x1, 1, NULL);
            return x1;
        }
        x = x1;
    }

    te_free(expr);
    (*env)->ReleaseStringUTFChars(env, funcion, gx);
    const char* msg = "No se encontró la raíz en el número máximo de iteraciones.";
    imprimirResultadoJSON("puntoFijo", x, 0, msg);
    throwException(env, msg);
    return 0.0;
}

// Bisección
JNIEXPORT jdouble JNICALL Java_analisisNumerico_biseccion
  (JNIEnv *env, jobject obj, jdouble a, jdouble b, jdouble tol, jint maxIter, jstring funcion) {

    const char *fx = (*env)->GetStringUTFChars(env, funcion, NULL);
    double variable;

    te_variable vars[] = {{"x", &variable}};
    te_expr *expr = te_compile(fx, vars, 1, NULL);
    if (!expr) {
        const char* msg = "Error al compilar la función f(x).";
        imprimirResultadoJSON("biseccion", NAN, 0, msg);
        throwException(env, msg);
        return NAN;
    }

    variable = a;
    double fa = te_eval(expr);
    variable = b;
    double fb = te_eval(expr);

    if (fa * fb > 0) {
        te_free(expr);
        (*env)->ReleaseStringUTFChars(env, funcion, fx);
        const char* msg = "f(a) y f(b) deben tener signos opuestos.";
        imprimirResultadoJSON("biseccion", NAN, 0, msg);
        throwException(env, msg);
        return NAN;
    }

    double c;
    for (int i = 0; i < maxIter; i++) {
        c = (a + b) / 2;
        variable = c;
        double fc = te_eval(expr);

        if (fabs(fc) < tol || (b - a) / 2 < tol) {
            te_free(expr);
            (*env)->ReleaseStringUTFChars(env, funcion, fx);
            imprimirResultadoJSON("biseccion", c, 1, NULL);
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

    te_free(expr);
    (*env)->ReleaseStringUTFChars(env, funcion, fx);
    const char* msg = "No se encontró la raíz en el número máximo de iteraciones.";
    imprimirResultadoJSON("biseccion", c, 0, msg);
    throwException(env, msg);
    return 0.0;
}

// Newton-Raphson
JNIEXPORT jdouble JNICALL Java_analisisNumerico_newtonRaphson
  (JNIEnv *env, jobject obj, jstring fx, jstring dfx, jdouble x0, jdouble tol, jint maxIter) {

    const char *fx_str = (*env)->GetStringUTFChars(env, fx, NULL);
    const char *dfx_str = (*env)->GetStringUTFChars(env, dfx, NULL);
    double variable;

    te_variable vars[] = {{"x", &variable}};
    te_expr *f_expr = te_compile(fx_str, vars, 1, NULL);
    te_expr *df_expr = te_compile(dfx_str, vars, 1, NULL);

    if (!f_expr || !df_expr) {
        if (f_expr) te_free(f_expr);
        if (df_expr) te_free(df_expr);
        (*env)->ReleaseStringUTFChars(env, fx, fx_str);
        (*env)->ReleaseStringUTFChars(env, dfx, dfx_str);
        const char* msg = "Error al compilar f(x) o f'(x).";
        imprimirResultadoJSON("newtonRaphson", NAN, 0, msg);
        throwException(env, msg);
        return NAN;
    }

    double x = x0;
    for (int i = 0; i < maxIter; i++) {
        variable = x;
        double fxVal = te_eval(f_expr);
        double dfxVal = te_eval(df_expr);

        if (fabs(dfxVal) < 1e-10) {
            te_free(f_expr);
            te_free(df_expr);
            (*env)->ReleaseStringUTFChars(env, fx, fx_str);
            (*env)->ReleaseStringUTFChars(env, dfx, dfx_str);
            const char* msg = "Derivada demasiado pequeña en Newton-Raphson.";
            imprimirResultadoJSON("newtonRaphson", NAN, 0, msg);
            throwException(env, msg);
            return NAN;
        }

        double x1 = x - fxVal / dfxVal;

        if (fabs(x1 - x) < tol) {
            te_free(f_expr);
            te_free(df_expr);
            (*env)->ReleaseStringUTFChars(env, fx, fx_str);
            (*env)->ReleaseStringUTFChars(env, dfx, dfx_str);
            imprimirResultadoJSON("newtonRaphson", x1, 1, NULL);
            return x1;
        }

        x = x1;
    }

    te_free(f_expr);
    te_free(df_expr);
    (*env)->ReleaseStringUTFChars(env, fx, fx_str);
    (*env)->ReleaseStringUTFChars(env, dfx, dfx_str);
    const char* msg = "No se encontró la raíz en el número máximo de iteraciones.";
    imprimirResultadoJSON("newtonRaphson", x, 0, msg);
    throwException(env, msg);
    return 0.0;
}

