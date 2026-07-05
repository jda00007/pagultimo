//
// Created by programas on 29/06/2026.
//

#include "Material.h"
namespace PAG {

        Material::Material()
        : colorAmbiente(0.2f, 0.2f, 0.2f),
        colorDifuso(0.8f, 0.8f, 0.8f),
        colorEspecular(1.0f, 1.0f, 1.0f),
        exponenteEspecular(32.0f),
        texturaColor(nullptr),
        texturaNormales(nullptr)
        {}

        Material::Material(const glm::vec3& ambiente, const glm::vec3& difuso,
        const glm::vec3& especular, float shininess)
        : colorAmbiente(ambiente), colorDifuso(difuso),
        colorEspecular(especular), exponenteEspecular(shininess),
        texturaColor(nullptr),
        texturaNormales(nullptr)
        {}

}