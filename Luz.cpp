//
// Created by programas on 01/07/2026.
//

#include "Luz.h"

namespace PAG {
    Luz::Luz(TipoLuz tipo,
             const glm::vec3& colorDifuso,
             const glm::vec3& colorEspecular,
             const glm::vec3& posicion,
             const glm::vec3& direccion,
             float anguloApertura,
             float exponenteBordes,
             const glm::vec3& colorAmbiente)
            : tipo(tipo),
              colorDifuso(colorDifuso),
              colorEspecular(colorEspecular),
              posicion(posicion),
              direccion(glm::normalize(direccion)),
              anguloApertura(anguloApertura),
              exponenteBordes(exponenteBordes),
              colorAmbiente(colorAmbiente) {}
}