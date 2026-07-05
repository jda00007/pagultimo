//
// Created by programas on 23/09/2025.
//

#ifndef PAG2_RENDERER_H
#define PAG2_RENDERER_H

#include <string>
#include <memory>
#include "glad/glad.h"
#include "Shader.h"
#include "camara.h"
#include "Modelo.h"
#include "Luz.h"
#include <map>

namespace PAG {
    enum class ModoVisualizacion{
        Alambre,
        Plano
    };
    enum class ModoColor {
        Material,
        Textura
    };

    class Renderer {
    private:
        static Renderer* instancia ; ///< Puntero al unico objeto
        Renderer();

        float rojo=0.6;
        float verde=0.6;
        float azul=0.6;
        GLuint idVS = 0; // Identificador del vertex shader
        GLuint idFS = 0; // Identificador del fragment shader
        GLuint idSP = 0; // Identificador del shader program
        GLuint idVAO = 0; // Identificador del vertex array object
        GLuint idVBO = 0; // Identificador del vertex buffer object
        GLuint idIBO = 0; // Identificador del index buffer object
        Shader shader;
        PAG::camara camara;
        PAG::Movimiento_Camara movimientoCamara = PAG::Movimiento_Camara::orbit;

        int anchoVentana=1024;
        int altoVentana=576;

        std::vector<std::unique_ptr<Modelo>> modelos;
        int indiceModeloSeleccionado=-1;

        ModoVisualizacion modoVisualizacion= ModoVisualizacion::Plano;
        GLuint indiceSubrutinaFijo=0;
        GLuint indiceSubrutinaMaterial=0;

        std::vector<std::unique_ptr<Luz>> luces;
        int indiceLuzSeleccionada=-1;

        GLuint indiceLuzAmbiente =0;
        GLuint indiceLuzPuntual =0;
        GLuint indiceLuzDireccional=0;
        GLuint indiceLuzFoco=0;

        std::map<std::string, std::unique_ptr<Textura>> texturas;
        Textura* texturaActual = nullptr;
        bool usarTextura = false;
        bool texturaActiva=false;

        GLuint indiceColorMaterial=-1;
        GLuint indiceColorTextura=-1;


        PAG::ModoColor modoColor= ModoColor::Material;

        bool usarNormalMapping = false;

        GLuint _fboSombras = 0;
        std::map<Luz*, GLuint> _texturasSombras;
        std::map<Luz*, glm::mat4> _matricesVisionLuz;
        std::map<Luz*, glm::mat4> _matricesProyeccionLuz; // Matrices de proyección por luz
        bool _usarSombras = false;
        int _anchoShadowMap = 1024;
        int _altoShadowMap = 1024;

        GLuint idVSSombras, idFSSombras, idSPSombras;

    public:

        virtual ~Renderer();

        static Renderer& getInstancia ();

        void refrescar ();

        void colorfondo(float rojo1,float verde,float azul);

        void creaShaderProgram(const std::string& nombre);

        void creaModelo();

        void inicializaOpenGL();

        std::string informeopenGL();

        void llamadaviewport(int width, int height);

        //void cargaShaders();

        void setMovimientoCamara(PAG::Movimiento_Camara movimiento);
        void movimientoraton(float deltaX, float deltaY);
        glm::mat4 getViewMatrix() const;

        void reseteo_camara(){ camara.reset();}

        void setViewport(int ancho, int alto);

        void setModoVisualizacion(ModoVisualizacion modo);

        void cargarModelo(const std::string& ruta, const std::string& nombre);
        void eliminarModelo(int indice);
        void eliminarModeloSeleccionado();
        void seleccionarModelo(int indice);
        int getModeloSeleccionado() const { return indiceModeloSeleccionado; }
        int getNumModelos() const { return (int)modelos.size(); }
        const std::string& getNombreModelo(int indice) const;

        void trasladarModeloSeleccionado(const glm::vec3& desplazamiento);
        void rotarModeloSeleccionado(float angulo, const glm::vec3& eje);
        void escalarModeloSeleccionado(const glm::vec3& factor);
        void resetTransformModeloSeleccionado();

        void inicializarSubrutinas();
        std::string querySubroutines() const;

        void setMaterialModeloSeleccionado(const Material &mat);

        void aniadirLuz(std::unique_ptr<Luz> luz);
        void eliminarLuz(int indice);
        void eliminarLuzSeleccionada();
        void seleccionarLuz(int indice);
        int getNumLuces() const{ return (int)luces.size();}
        int getLuzSeleccionada() const {return indiceLuzSeleccionada;}
        Luz* getLuz(int indice) {return luces[indice].get();}

        void inicializarSubrutinasIluminacion();

        void cargarTextura(const std::string& ruta, const std::string& nombre);
        void asignarTexturaAModeloSeleccionado(const std::string& nombreTextura);
        void setModoColor(ModoColor modo) { modoColor = modo; }
        const std::map<std::string, std::unique_ptr<Textura>>& getTexturas() const {return texturas;}
        PAG::Material& getModeloSeleccionadoMaterial();

        void inicializarSubrutinasColorBase();

        void setUsarNormalMapping(bool activar) { usarNormalMapping= activar;}
        bool getUsarNormalMapping() const { return usarNormalMapping; }
        void setUsarSombras(bool activar) { _usarSombras = activar; }
        void crearFBOSombras();
        void crearTexturaSombraParaLuz(Luz* luz);
        void calcularMatricesLuz();
        void renderizarShadowMaps();
        void asignarNormalMapAModeloSeleccionado(const std::string& nombreTextura);

        void creaShaderProgramSombras();

        glm::mat4 getTransformModeloSeleccionado() const;

    };


} // PAG

#endif //PAG2_RENDERER_H
