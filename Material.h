//
// Created by programas on 29/06/2026.
//

#ifndef PAGULTIMO_MATERIAL_H
#define PAGULTIMO_MATERIAL_H

#include <glm/glm.hpp>
#include "Textura.h"
namespace PAG {

    class Material {
    private:
        glm::vec3 colorAmbiente;
        glm::vec3 colorDifuso;
        glm::vec3 colorEspecular;
        float exponenteEspecular;

        Textura* texturaColor= nullptr;
        Textura* texturaNormales= nullptr;
    public:
        Material();
        Material(const glm::vec3& ambiente, const glm::vec3& difuso,
                 const glm::vec3& especular, float shininess);

        // Getters
        const glm::vec3& getAmbiente() const { return colorAmbiente; }
        const glm::vec3& getDifuso() const { return colorDifuso; }
        const glm::vec3& getEspecular() const { return colorEspecular; }
        float getShininess() const { return exponenteEspecular; }

        Textura* getTextura() const {return texturaColor;}

        // Setters
        void setAmbiente(const glm::vec3& c) { colorAmbiente = c; }
        void setDifuso(const glm::vec3& c) { colorDifuso = c; }
        void setEspecular(const glm::vec3& c) { colorEspecular = c; }
        void setShininess(float s) { exponenteEspecular = s; }

        void setTextura(Textura* t){ texturaColor = t;}

        void setTexturaNormales(Textura* t){ texturaNormales = t;}
        Textura* getTexturaNormales() const {return texturaNormales;}


    };

} // namespace PAG


#endif //PAGULTIMO_MATERIAL_H
