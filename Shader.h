//
// Created by programas on 08/10/2025.
//

#ifndef PAG2_SHADER_H
#define PAG2_SHADER_H
#include <string>
#include "glad/glad.h"
#include <glm/glm.hpp>
class Shader {
public:
    unsigned int ID;
    //Shader(const std::string& vertexPath, const std::string& fragmentPath);
    //void use() const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;


public:
    virtual ~Shader();


    static void creaShaderProgram(const std::string& nombre, GLuint& idVS, GLuint& idFS, GLuint& idSP);
    //void creaModelo();

    //void refrescar();

    std::string querySubroutines() const;
};


#endif //PAG2_SHADER_H
