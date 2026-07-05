//
// Created by programas on 08/10/2025.
//

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>
//#include <iostream>
#include "Shader.h"




Shader::~Shader(){

}

void Shader::creaShaderProgram(const std::string& nombre,GLuint& idVS,GLuint& idFS, GLuint& idSP) {
    /*std::string miVertexShader =
              "#version 410\n"
              "layout (location = 0) in vec posicion;\n"
              "void main ()\n"
              "{ gl_Position = vec4 ( posicion, 1 );\n"
              "}\n";
*/
    /*
    std::string  miFragmentShader =
            "#version 410\n"
            "out vec4 colorFragmento;\n"
            "void main ()\n"
            "{ colorFragmento = vec4 (1.0,0.4,0.2,1.0);\n"
            "}\n";
*/
    //Creacion del shader program
    idSP= glCreateProgram();
    if (idSP==0){
        throw std::runtime_error("Error: No se pudo crear el shader program ");
    }



    idVS  = glCreateShader(GL_VERTEX_SHADER);
    if (idVS ==0){
        throw std::runtime_error("Error: No se pudo crear el Shader: Vertex_Shader");
    }


    idFS = glCreateShader(GL_FRAGMENT_SHADER);
    if (idFS==0){
        throw std::runtime_error("Error: No se pudo crear el Shader: Fragment_Shader");
    }
/*
    std::string rutaVertex= "../Shaders/pag03-vs.glsl";
    std::string rutaFragment= "../Shaders/pag03-fs.glsl";
*/
    std::string rutaVertex = "../Shaders/"+ nombre+ "-vs.glsl";
    std::string rutaFragment= "../Shaders/"+ nombre+ "-fs.glsl";

    printf("Intentando cargar:\n  Vertex: %s\n  Fragment: %s\n",
           rutaVertex.c_str(), rutaFragment.c_str());
    std::ifstream archivoShader;
    archivoShader.open (rutaVertex);

    if (!archivoShader.is_open()){
        //error abriendo el archivo
        throw std::runtime_error("Error: No se pudo abrir el archivo del VertexShader");
    }

    std::stringstream streamShader;
    streamShader << archivoShader.rdbuf ();
    std::string codigoFuenteShader = streamShader.str();

    archivoShader.close();

    std::ifstream archivoShaderFrag;
    archivoShaderFrag.open (rutaFragment);

    if (!archivoShaderFrag.is_open()){
        //error abriendo el archivo
        throw std::runtime_error("Error: No se pudo abrir el archivo del FragmentShader");
    }

    std::stringstream streamShaderFrag;
    streamShaderFrag << archivoShaderFrag.rdbuf ();
    std::string codigoFuenteShaderFag = streamShaderFrag.str();

    archivoShaderFrag.close();


    const GLchar* codigofuenteFormatoC = codigoFuenteShader.c_str();
    glShaderSource(idVS,1,&codigofuenteFormatoC,NULL);
    glCompileShader(idVS);

    GLint resultadoCompilacion;
    glGetShaderiv(idVS, GL_COMPILE_STATUS, &resultadoCompilacion);
    if (resultadoCompilacion == GL_FALSE) {
        GLint tamMsj = 0;
        glGetShaderiv(idVS, GL_INFO_LOG_LENGTH, &tamMsj);
        if (tamMsj > 0) {
            GLchar* mensajeFormatoC = new GLchar[tamMsj];
            glGetShaderInfoLog(idVS, tamMsj, nullptr, mensajeFormatoC);
            std::cerr << "[ERROR COMPILACIÓN VS]: " << mensajeFormatoC << std::endl;
            delete[] mensajeFormatoC;
        }
    }




    const GLchar* codigofuenteFormatoCF = codigoFuenteShaderFag.c_str();
    glShaderSource(idFS,1,&codigofuenteFormatoCF,NULL);
    glCompileShader(idFS);

    GLint resultadoCompilacion2;
    glGetShaderiv(idFS, GL_COMPILE_STATUS, &resultadoCompilacion2);
    if (resultadoCompilacion2 == GL_FALSE) {
        GLint tamMsj = 0;
        glGetShaderiv(idFS, GL_INFO_LOG_LENGTH, &tamMsj);
        if (tamMsj > 0) {
            GLchar* mensajeFormatoC = new GLchar[tamMsj];
            glGetShaderInfoLog(idFS, tamMsj, nullptr, mensajeFormatoC);
            std::cerr << "[ERROR COMPILACIÓN FS]: " << mensajeFormatoC << std::endl;
            delete[] mensajeFormatoC;
        }
    }



    glAttachShader ( idSP, idVS );
    glAttachShader ( idSP, idFS);

    glLinkProgram ( idSP );
    GLint resultadoEnlazado = 0;
    glGetProgramiv ( idSP, GL_LINK_STATUS, &resultadoEnlazado );
    if ( resultadoEnlazado == GL_FALSE )
    {
        GLint tamMsj = 0;
        std::string mensaje = "";
        glGetProgramiv ( idSP, GL_INFO_LOG_LENGTH, &tamMsj );
        if ( tamMsj > 0 )
        {  GLchar* mensajeFormatoC = new GLchar[tamMsj];
            GLint datosEscritos = 0;
            glGetProgramInfoLog ( idSP, tamMsj, &datosEscritos, mensajeFormatoC );
            mensaje.assign ( mensajeFormatoC );
            delete[] mensajeFormatoC;
            mensajeFormatoC = nullptr;
            throw std::runtime_error(mensaje);
        }
    }

    /*
    idVS = glCreateShader ( GL_VERTEX_SHADER );
    const GLchar* fuenteVS = miVertexShader.c_str();
    glShaderSource (idVS,1,&fuenteVS, nullptr);
    glCompileShader( idFS);

    idSP = glCreateProgram ();
    glAttachShader (idSP, idVS);
    glAttachShader (idSP, idFS);
    glLinkProgram ( idSP);
*/
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

std::string Shader::querySubroutines() const {
    std::stringstream ss;
    ss << "Subroutines info from shader program" << std::endl;

    GLint nSubroutines = 0;
    glGetProgramStageiv(ID, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &nSubroutines);
    ss << "\tNumber of active subroutines: " << nSubroutines << std::endl;

    GLint nImplementations = 0;
    glGetProgramStageiv(ID, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINES, &nImplementations);
    ss << "\tNumber of available implementations: " << nImplementations << std::endl;

    GLchar* auxChar = new GLchar[256];
    GLsizei length = 0;

    for (int i = 0; i < nSubroutines; ++i) {
        glGetActiveSubroutineUniformName(ID, GL_FRAGMENT_SHADER, i, 256, &length, auxChar);
        ss << "\tImplementations for subroutine " << auxChar << std::endl;

        GLint numCompatibleImplementations = 0;
        glGetActiveSubroutineUniformiv(ID, GL_FRAGMENT_SHADER, i,
                                       GL_NUM_COMPATIBLE_SUBROUTINES,
                                       &numCompatibleImplementations);

        GLint* indices = new GLint[numCompatibleImplementations];
        glGetActiveSubroutineUniformiv(ID, GL_FRAGMENT_SHADER, i,
                                       GL_COMPATIBLE_SUBROUTINES, indices);

        for (int j = 0; j < numCompatibleImplementations; ++j) {
            glGetActiveSubroutineName(ID, GL_FRAGMENT_SHADER, indices[j], 256, &length, auxChar);
            ss << "\t\tImplementation " << indices[j] << ":" << auxChar << std::endl;
        }
        delete[] indices;
    }

    delete[] auxChar;
    return ss.str();
}
/*
void Shader::creaModelo() {
    GLfloat vertices[] = { -0.5,-0.5, 0,
                           0.5,-0.5,0,
                           0.0,0.5,0};
    GLuint indices[]={0,1,2};
    glGenVertexArrays (1, &idVAO);
    glBindVertexArray (idVAO);
    glGenBuffers (1, &idVBO);
    glBindBuffer (GL_ARRAY_BUFFER, idVBO);
    glBufferData (GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer (0,3,GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray (0);
    glGenBuffers (1,&idIBO);
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER,idIBO);
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER,3*sizeof(GLuint),indices, GL_STATIC_DRAW);
}
*/
/*
void Shader::refrescar() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glUseProgram ( idSP );
    glBindVertexArray ( idVAO );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO );
    glDrawElements( GL_TRIANGLES, 3 , GL_UNSIGNED_INT, nullptr );

}*/


/*Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {


}


void Shader::use() const{

}
 *//*
void Shader::setMat4(const std::string &name, const glm::mat4 &value) const{

    glUseProgram(ID);

    int informaciondeLocation = glGetUniformLocation(ID,name.c_str());

    if(informaciondeLocation == -1){
        std::cerr << "Aviso:" << name << " no encontrada en el shader.";
        return;

        glUni
    }
}
*/