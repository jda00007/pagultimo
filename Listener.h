//
// Created by programas on 23/09/2025.
//


#ifndef PAG2_LISTENER_H
#define PAG2_LISTENER_H

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include "Renderer.h"
//#include <imgui.h>
//#include <imgui_impl_glfw.h>
//#include <imgui_impl_opengl3.h>


class Listener{
public:
    Listener ()=default;
    virtual ~Listener () = default;
    //WindowType es un tipo enumerado propio para identificar
    //el tipo de ventana de la interfaz
    //virtual void wakeUp(WindowType t, ... ) = 0;
};
#endif //PAG2_LISTENER_H
