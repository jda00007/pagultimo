//
// Created by programas on 01/07/2026.
//

#ifndef PAGULTIMO_TEXTURA_H
#define PAGULTIMO_TEXTURA_H

#include <string>
#include <glad/glad.h>

namespace PAG {

    class Textura {
    private:
        GLuint idTextura;
        int ancho;
        int alto;
        std::string ruta;

    public:
        Textura();
        ~Textura();

        // Carga una texturaColor desde un archivo PNG
        void cargar(const std::string& rutaArchivo);

        // Activa la texturaColor en una unidad específica
        void activar(GLuint unidad = 0) const;

        // Desactiva la texturaColor
        void desactivar() const;

        GLuint getId() const { return idTextura; }
        int getAncho() const { return ancho; }
        int getAlto() const { return alto; }
        const std::string& getRuta() const { return ruta; }
        bool estaCargada() const { return idTextura != 0; }
    };

}


#endif //PAGULTIMO_TEXTURA_H
