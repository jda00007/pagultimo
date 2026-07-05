//
// Created by programas on 01/07/2026.
//

#ifndef PAGULTIMO_LUZ_H
#define PAGULTIMO_LUZ_H

#include <glm/glm.hpp>

namespace PAG {

    enum class TipoLuz {
        Ambiente,
        Puntual,
        Direccional,
        Foco
    };

    class Luz {
    private:
        TipoLuz tipo;

        // Propiedades comunes
        glm::vec3 colorAmbiente;    // Ia (solo para luz ambiente)
        glm::vec3 colorDifuso;      // Id
        glm::vec3 colorEspecular;   // Is

        // Propiedades específicas
        glm::vec3 posicion;         // p (puntual y foco)
        glm::vec3 direccion;        // d (direccional y foco)
        float anguloApertura;       // gamma (foco)
        float exponenteBordes;      // s (foco, bordes suaves)

    public:
        Luz(TipoLuz tipo,
            const glm::vec3& colorDifuso = glm::vec3(1.0f),
            const glm::vec3& colorEspecular = glm::vec3(1.0f),
            const glm::vec3& posicion = glm::vec3(0.0f),
            const glm::vec3& direccion = glm::vec3(0.0f, -1.0f, 0.0f),
            float anguloApertura = glm::radians(30.0f),
            float exponenteBordes = 1.0f,
            const glm::vec3& colorAmbiente = glm::vec3(0.0f));


        TipoLuz getTipo() const { return tipo; }
        const glm::vec3& getColorAmbiente() const { return colorAmbiente; }
        const glm::vec3& getColorDifuso() const { return colorDifuso; }
        const glm::vec3& getColorEspecular() const { return colorEspecular; }
        const glm::vec3& getPosicion() const { return posicion; }
        const glm::vec3& getDireccion() const { return direccion; }
        float getAnguloApertura() const { return anguloApertura; }
        float getExponenteBordes() const { return exponenteBordes; }

        void setTipo(TipoLuz t) { tipo = t; }
        void setColorAmbiente(const glm::vec3& c) { colorAmbiente = c; }
        void setColorDifuso(const glm::vec3& c) { colorDifuso = c; }
        void setColorEspecular(const glm::vec3& c) { colorEspecular = c; }
        void setPosicion(const glm::vec3& p) { posicion = p; }
        void setDireccion(const glm::vec3& d) { direccion = d; }
        void setAnguloApertura(float a) { anguloApertura = a; }
        void setExponenteBordes(float e) { exponenteBordes = e; }
    };

};


#endif //PAGULTIMO_LUZ_H
