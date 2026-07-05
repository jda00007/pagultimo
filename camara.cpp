//
// Created by programas on 21/10/2025.
//

#include "camara.h"
#include <cmath>

namespace PAG {
    camara::camara(glm::vec3 posicion_aux, glm::vec3 punto_de_referencia_aux, glm::vec3 vector_up_aux)
            : posicion(posicion_aux), punto_de_referencia(punto_de_referencia_aux), vector_up(vector_up_aux) {
        glm::vec3 direccion = glm::normalize(punto_de_referencia - posicion);
        radio = glm::length(punto_de_referencia - posicion);
        yaw = std::atan2(direccion.x, direccion.z);
        pitch = std::asin(direccion.y);
        fov = glm::radians(45.0f);
    }

// rotacion en latitud(pitch) y longitud(yaw) alrededor del punto de
// referencia. La cámara se mueve, el punto de referencia no.
    void camara::orbit(float deltaX, float deltaY) {

        yaw += glm::radians(deltaX);
        pitch -= glm::radians(deltaY);

        pitch = glm::clamp(pitch, -1.48f, 1.48f);

        float x = radio * sin(yaw) * cos(pitch);
        float y = radio * sin(pitch);
        float z = radio * cos(yaw) * cos(pitch);

        posicion = punto_de_referencia - glm::vec3(x, y, z);
    }

    // rotación horizontal de la camara sin moverla de su posicion
    // se rota el punto de referencia alrededor del eje v(vertical local)
    void camara::pan(float deltaX,float deltaY){
        glm::vec3 n = glm::normalize(punto_de_referencia - posicion); // direccion de mirada
        glm::vec3 u = glm::normalize(glm::cross(n, vector_up));       // eje u
        glm::vec3 v = glm::normalize(glm::cross(u, n));               // eje v

        float sensibilidad = 0.005f;
        float angulo= -deltaX*sensibilidad;

        glm::mat4  rot = glm::rotate(glm::mat4(1.0),angulo,v);
        glm::vec3 nuevaDireccion= glm::vec3 (rot* glm::vec4(punto_de_referencia-posicion,0.0f));
        punto_de_referencia=posicion+nuevaDireccion;
    }

    //traslación de la cámara en el plano XZ de la escena
    //se mueve tanto la cámara como el punto de referencia
    void camara::dolly(float delta) {
        glm::vec3 direccion= glm::normalize(punto_de_referencia - posicion);
        direccion.y=0.0f;
        if(glm::length(direccion)< 0.0001f){
            return;
        }
        direccion=glm::normalize(direccion);

        float sensibilidad= 0.05f;
        glm::vec3 traslacion= direccion * (delta*sensibilidad);

        glm::mat4 T= glm::translate(glm::mat4(1.0f),traslacion);
        posicion= glm::vec3(T * glm::vec4(posicion, 1.0f));
        punto_de_referencia= glm::vec3(T * glm::vec4(punto_de_referencia, 1.0f));
    }

    //rotación vertical de la cámara sin moverla de su posición
    void camara::tilt(float deltaAngulo) {

        glm::vec3 n = punto_de_referencia - posicion;
        glm::vec3 u = glm::normalize(glm::cross(n, vector_up));

        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), deltaAngulo, u);
        glm::vec3 nuevaDireccion = glm::vec3(rot * glm::vec4(n, 0.0f));

        punto_de_referencia = posicion + nuevaDireccion;
    }

    //cambia el ángulo de visión (FOV) sin mover la cámara
    void camara::zoom(float delta) {
        fov -= glm::radians(delta * 0.5f);
        fov = glm::clamp(fov, glm::radians(10.0f), glm::radians(120.0f));
    }

    // traslación en  el eje Y global
    // se mueve tanto la cámara como el punto de referencia
    void camara::crane(float deltaY) {
        glm::vec3 upGlobal = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 traslacion = upGlobal* (deltaY * 0.05f);

        glm::mat4 T= glm::translate(glm::mat4 (1.0f),traslacion);
        posicion= glm::vec3(T*glm::vec4(posicion,1.0f));
        punto_de_referencia= glm::vec3(T*glm::vec4 (punto_de_referencia,1.0f));
    }

    //reset del posicionamiento de la cámara
    void camara::reset() {
        posicion = glm::vec3(0.0f, 0.0f, 3.0f);
        punto_de_referencia = glm::vec3(0.0f, 0.0f, 0.0f);
        vector_up = glm::vec3(0.0f,1.0f,0.0f);
        radio= 3.0f;
        yaw= 0.0f;
        pitch= 0.0f;
    }
    glm::mat4 camara::getViewMatrix() const {
        return glm::lookAt(posicion, punto_de_referencia,vector_up);
    }

    glm::mat4 camara::getProjectionMatrix(float aspecto) const {
        return glm::perspective(fov, aspecto, 0.1f, 100.0f);
    }

}