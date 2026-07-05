//
// Created by programas on 01/07/2026.
//

#include "Textura.h"
#include "lodepng.h"
#include <stdexcept>


namespace PAG {

    Textura::Textura() : idTextura(0), ancho(0), alto(0) {}

    Textura::~Textura() {
        if (idTextura != 0) {
            glDeleteTextures(1, &idTextura);
        }
    }

    void Textura::cargar(const std::string& rutaArchivo) {
        ruta = rutaArchivo;

        // Cargar imagen PNG con LodePNG
        std::vector<unsigned char> imagen;
        unsigned anchoImg, altoImg;
        unsigned error = lodepng::decode(imagen,anchoImg,altoImg,rutaArchivo);

        ancho=static_cast<int> (anchoImg);
        alto =static_cast<int> (altoImg);

        if (error) {
            throw std::runtime_error("Error al cargar texturaColor: " +
                                     rutaArchivo + " - " + lodepng_error_text(error));
        }

        // Voltear la imagen verticalmente (PNG tiene Y hacia abajo, OpenGL Y hacia arriba)
        unsigned char* imgPtr = &imagen[0];
        int numComponentesColor = 4; // RGBA
        int incrementoAncho = ancho * numComponentesColor;

        for (int i = 0; i < alto / 2; i++) {
            unsigned char* top = imgPtr + i * incrementoAncho;
            unsigned char* bot = imgPtr + (alto - i - 1) * incrementoAncho;
            for (int j = 0; j < incrementoAncho; j++) {
                std::swap(*top, *bot);
                ++top;
                ++bot;
            }
        }

        // Crear texturaColor OpenGL
        if (idTextura == 0) {
            glGenTextures(1, &idTextura);
        }

        glBindTexture(GL_TEXTURE_2D, idTextura);

        // Subir los píxeles a la GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ancho, alto, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, imagen.data());

        // Parámetros de la texturaColor
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Generar mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Textura::activar(GLuint unidad) const {

        glActiveTexture(GL_TEXTURE0 + unidad);
        glBindTexture(GL_TEXTURE_2D, idTextura);
    }

    void Textura::desactivar() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}