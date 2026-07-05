//
// Created by programas on 05/11/2025.
//

#ifndef PAG2_MODELO_H
#define PAG2_MODELO_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Material.h"

namespace PAG {

    struct Vertex {
        glm::vec3 Posicion;
        glm::vec3 Normal;
        glm::vec2 coordenadas;
        glm::vec3 Tangente;
        glm::vec3 Bitangente;
    };

    class Modelo {
    private:
        std::string nombre;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int VAO, VBO, EBO;

        // Matriz de transformación de modelado
        glm::mat4 transform;

        void setupMesh();
        Material material;

    public:
        Modelo(const std::string &ruta, const std::string &nombreModelo);

        ~Modelo();

        Modelo(const Modelo &) = delete;

        Modelo &operator=(const Modelo &) = delete;

        Modelo(Modelo &&) noexcept;

        Modelo &operator=(Modelo &&) noexcept;

        void trasladar(const glm::vec3 &desplazamiento);

        void rotar(float anguloRadianes, const glm::vec3 &eje);

        void escalar(const glm::vec3 &factor);

        void resetTransform();

        const std::string &getNombre() const { return nombre; }

        const glm::mat4 &getTransform() const { return transform; }

        void draw(Shader& shader) const;

        Material& getMaterial() { return material; }
        const Material& getMaterial() const { return material; }
        void setMaterial(const Material& m) { material = m; }

        GLuint getVAO() const { return VAO; }
        size_t getNumIndices() const { return indices.size(); }
    };
}


#endif //PAG2_MODELO_H
