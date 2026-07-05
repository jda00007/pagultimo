//
// Created by programas on 05/11/2025.
//

#include "Modelo.h"

#include <iostream>

namespace PAG {

    Modelo::Modelo(const std::string& ruta, const std::string& nombreModelo)
            : nombre(nombreModelo), transform(glm::mat4(1.0f))
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(ruta,
                                                 aiProcess_Triangulate |
                                                 aiProcess_GenNormals |
                                                 aiProcess_CalcTangentSpace |
                                                 //aiProcess_FlipUVs |
                                                 aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error("ERROR::ASSIMP:: " + std::string(importer.GetErrorString()));
        }

        if(scene->mNumMeshes == 0){
            throw std::runtime_error("El modelo cargado no tiene mallas");
        }


        for (unsigned int m = 0; m < scene->mNumMeshes; m++){

            const aiMesh* malla = scene->mMeshes[m];

            unsigned int offsetVertices = (unsigned int)vertices.size();

            // 1. Cargar Vértices
            for (unsigned int i = 0; i < malla->mNumVertices; i++) {
                Vertex vertex;
                vertex.Posicion = glm::vec3(malla->mVertices[i].x, malla->mVertices[i].y, malla->mVertices[i].z);
                vertex.Normal = glm::vec3(malla->mNormals[i].x, malla->mNormals[i].y, malla->mNormals[i].z);

                if (malla->mTextureCoords[0]) {
                    vertex.coordenadas = glm::vec2(malla->mTextureCoords[0][i].x, malla->mTextureCoords[0][i].y);
                } else {
                    vertex.coordenadas = glm::vec2(0.0f, 0.0f);
                }
                //vertices.push_back(vertex);


                if (malla->mTangents && malla->mBitangents) {
                    vertex.Tangente = glm::vec3(malla->mTangents[i].x, malla->mTangents[i].y, malla->mTangents[i].z);
                    vertex.Bitangente = glm::vec3(malla->mBitangents[i].x, malla->mBitangents[i].y, malla->mBitangents[i].z);
                } else {
                    // Generar tangente perpendicular a la normal
                    glm::vec3 normal = vertex.Normal;
                    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

                    // ✅ CORRECCIÓN: Calcular cross SIN normalizar primero
                    glm::vec3 tangent = glm::cross(worldUp, normal);

                    // Verificar si el cross product es demasiado pequeño (normal paralela a worldUp)
                    if (glm::length(tangent) < 0.001f) {
                        tangent = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), normal);
                    }

                    // ✅ Ahora sí normalizar
                    vertex.Tangente = glm::normalize(tangent);

                    // Bitangente = normal × tangente (completar la base ortonormal)
                    vertex.Bitangente = glm::cross(normal, vertex.Tangente);
                }

                if (i == 0) { // Solo imprimir para el primer vértice
                    std::cout << "[DEBUG] Vértice 0 - Tangente: ("
                              << vertex.Tangente.x << ", " << vertex.Tangente.y << ", " << vertex.Tangente.z << ")\n";
                    std::cout << "[DEBUG] Vértice 0 - Bitangente: ("
                              << vertex.Bitangente.x << ", " << vertex.Bitangente.y << ", " << vertex.Bitangente.z << ")\n";
                    std::cout << "[DEBUG] Vértice 0 - Normal: ("
                              << vertex.Normal.x << ", " << vertex.Normal.y << ", " << vertex.Normal.z << ")\n";
                }

                vertices.push_back(vertex);

            }


            // 2. Cargar Índices
            for (unsigned int i = 0; i < malla->mNumFaces; i++) {
                aiFace face = malla->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j] + offsetVertices);
                }
            }
        }

        setupMesh();
    }

    Modelo::~Modelo() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    Modelo::Modelo(Modelo&& other) noexcept
            : nombre(std::move(other.nombre)),
              vertices(std::move(other.vertices)),
              indices(std::move(other.indices)),
              VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
              transform(other.transform)
    {
        other.VAO = other.VBO = other.EBO = 0;
    }

    Modelo& Modelo::operator=(Modelo&& other) noexcept {
        if (this != &other) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            nombre = std::move(other.nombre);
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            VAO = other.VAO; VBO = other.VBO; EBO = other.EBO;
            transform = other.transform;

            other.VAO = other.VBO = other.EBO = 0;
        }
        return *this;
    }

    void Modelo::setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),&vertices[0], GL_STATIC_DRAW);

        if (!indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                         &indices[0], GL_STATIC_DRAW);
        }
        //Posicion
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        //Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        // Coordenadas
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)offsetof(Vertex, coordenadas));
        glEnableVertexAttribArray(2);

        //Tangente
        glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,Tangente));
        glEnableVertexAttribArray(3);
        //Bitangente
        glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,Bitangente));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);
    }


    void Modelo::trasladar(const glm::vec3& desplazamiento) {
        transform = glm::translate(transform, desplazamiento);
    }

    void Modelo::rotar(float anguloRadianes, const glm::vec3& eje) {
        transform = glm::rotate(transform, anguloRadianes, eje);
    }

    void Modelo::escalar(const glm::vec3& factor) {
        transform = glm::scale(transform, factor);
    }

    void Modelo::resetTransform() {
        transform = glm::mat4(1.0f);
    }

    void Modelo::draw(Shader &shader) const {
        shader.setMat4("model", transform);
        glBindVertexArray(VAO);
        if (!indices.empty()) {
            glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }



}