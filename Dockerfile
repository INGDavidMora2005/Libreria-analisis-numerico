FROM openjdk:21
VOLUME /tmp
EXPOSE 8080
ARG JAR_FILE_target/archivo.jar
ADD ${JAR_FILE} app.jar
COPY analisisNumerico.so /lib64
ENV LD_LIBRARY_PATH=/usr/local/lib/jni:$LD_LIBRARY_PATH
ENTRYPOINT ["java","-jar","app.jar"]
