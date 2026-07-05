
#include <iostream>
// IMPORTANTE: El include de GLAD debe estar siempre ANTES de el de GLFW
//#include <glad/glad.h>
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <deque>
#include "camara.h"
//#include <string>
//#include "Shader.h"
#include <imgui_stdlib.h>
#include "imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>

float colorfondo[4]={0.6,0.6,0.6,1};
std::deque<std::string> mensajesporventana;
bool raton = false;
double posicion_raton_anterior_X=0;
double posicion_raton_anterior_Y=0;
PAG::Movimiento_Camara movimiento_seleccionado= PAG::Movimiento_Camara::orbit;

static ImGui::FileBrowser fileDialogModelos;
static ImGui::FileBrowser fileDialogTexturas;
static ImGui::FileBrowser fileDialogNormales;
//Funcion para agregar los mensajes a la estructura
void aniadirmensajesporventana(const std::string& mensaje){
    mensajesporventana.push_back(mensaje);
}


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    /*
    if (action == GLFW_PRESS){
        //Si es necesario hacer algo con este evento, indicarlo aqui
        //Finalmente, comunica el evente de ratón a ImGui
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button,true);
    }else if(action == GLFW_RELEASE){
        //Si es necesario hacer algo con este evento, indicarlo aqui
        //Finalmente, comunica el evento de ratón a ImGui
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button,false);
    }
     */

    if (button == GLFW_MOUSE_BUTTON_LEFT){
     if (action == GLFW_PRESS){
         double x_posicion, y_posicion;
         glfwGetCursorPos(window,&x_posicion,&y_posicion);
         posicion_raton_anterior_X= x_posicion;
         posicion_raton_anterior_Y= y_posicion;
         raton=true;
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
     }   else {
         if (action == GLFW_RELEASE){
             raton = false;
             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
         }
     }
    }

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button,action == GLFW_PRESS);
}


void callbackRefrescoVentana(GLFWwindow* ventana){
    PAG::Renderer::getInstancia().refrescar();
    glfwSwapBuffers(ventana);
    //std::cout << "Finaliza el callback de refresco" << std::endl;
    aniadirmensajesporventana("Finaliza el callback de refresco");
}

// - Esta función callback será llamada cuando GLFW produzca algún error
void error_callback ( int errno, const char* desc )
{  std::string aux (desc);
    //std::cout << "Error de GLFW número " << errno << ": " << aux << std::endl;
    aniadirmensajesporventana("Error de GLFW número " + std::to_string(errno)+ ": " + std::string(aux));
}
/*
// - Esta función callback será llamada cada vez que el área de dibujo
// OpenGL deba ser redibujada.
void window_refresh_callback ( GLFWwindow *window )
{  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
    // intercambia el buffer back (que se ha estado dibujando) por el
    // que se mostraba hasta ahora front. Debe ser la última orden de
    // este callback
    //std::cout << "Refresh callback called" << std::endl;
}*/

// - Esta función callback será llamada cada vez que se cambie el tamaño
// del área de dibujo OpenGL.
void framebuffer_size_callback ( GLFWwindow *window, int width, int height )
{  //glViewport ( 0, 0, width, height );
    PAG::Renderer::getInstancia().setViewport(width,height);
    //std::cout << "Resize callback called" << std::endl;
    aniadirmensajesporventana("Resize callback called");
}

// - Esta función callback será llamada cada vez que se pulse una tecla
// dirigida al área de dibujo OpenGL.
void key_callback ( GLFWwindow *window, int key, int scancode, int action, int mods )
{  if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {  glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    //std::cout << "Key callback called" << std::endl;
    //aniadirmensajesporventana("Key callback called");
}

// - Esta función callback será llamada cada vez que se pulse algún botón
// del ratón sobre el área de dibujo OpenGL.
/*void mouse_button_callback ( GLFWwindow *window, int button, int action, int mods )
{  if ( action == GLFW_PRESS )
    {  //std::cout << "Pulsado el botón: " << button << std::endl;
        aniadirmensajesporventana("Pulsado el botón: " + std::to_string(button));
    }
    else if ( action == GLFW_RELEASE )
    {  //std::cout << "Soltado el botón: " << button << std::endl;
        aniadirmensajesporventana("Soltado el botón: " + std::to_string(button));
    }
}*/

// - Esta función callback será llamada cada vez que se mueva la rueda
// del ratón sobre el área de dibujo OpenGL.
void scroll_callback ( GLFWwindow *window, double xoffset, double yoffset )
{  //std::cout << "Movida la rueda del ratón " << xoffset
             //<< " Unidades en horizontal y " << yoffset
             //<< " unidades en vertical" << std::endl;

    //aniadirmensajesporventana("Movida la rueda del ratón " + std::to_string(xoffset)
      //                          + "Unidades en horizontal y " + std::to_string(yoffset) + "unidades en vertical"
         //                       );

    float delta=(float)yoffset;
    if(movimiento_seleccionado==PAG::Movimiento_Camara::zoom){
        PAG::Renderer::getInstancia().movimientoraton(0.0f, yoffset * 5.0f);
    }else{
        for (int i = 0; i < 3; ++i) {
            colorfondo[i]+=delta*0.02f;
            if(colorfondo[i]<=0){
                colorfondo[i]=0;
            }
            if(colorfondo[i]>=1){
                colorfondo[i]=1;
            }

        }
        //agragado para cambiar el color cuando se use la rueda del raton
        PAG::Renderer::getInstancia().colorfondo(colorfondo[0],colorfondo[1],colorfondo[2]);
    }


}

void posicion_cursor_callback(GLFWwindow* window, double x_posicion, double y_posicion){
    if (raton){
        //aniadirmensajesporventana("raton moviendose");
        float deltaX = (float)(x_posicion - posicion_raton_anterior_X);
        float deltaY = (float) (y_posicion - posicion_raton_anterior_Y);

        PAG::Renderer::getInstancia().movimientoraton(deltaX,deltaY);

        posicion_raton_anterior_X= x_posicion;
        posicion_raton_anterior_Y= y_posicion;

        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent((float)x_posicion,(float) y_posicion);
    }
}

int main()
{  //std::cout << "Starting Application PAG - Prueba 01" << std::endl;
    aniadirmensajesporventana("Starting Application PAG - Prueba 01");
    // - Este callback hay que registrarlo ANTES de llamar a glfwInit
    glfwSetErrorCallback ( (GLFWerrorfun) error_callback );

    // - Inicializa GLFW. Es un proceso que sólo debe realizarse una vez en la aplicación
    if ( glfwInit () != GLFW_TRUE )
    //como es un error lo dejo para que se ponga en la consola
    {  std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // - Definimos las características que queremos que tenga el contexto gráfico
    // OpenGL de la ventana que vamos a crear. Por ejemplo, el número de muestras o el
    // modo Core Profile.
    glfwWindowHint ( GLFW_SAMPLES, 4 ); // - Activa antialiasing con 4 muestras.
    glfwWindowHint ( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE ); // - Esta y las 2
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 4 ); // siguientes activan un contexto
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 1 ); // OpenGL Core Profile 4.1.

    // - Definimos el puntero para guardar la dirección de la ventana de la aplicación y
    // la creamos
    GLFWwindow *window;
    // - Tamaño, título de la ventana, en ventana y no en pantalla completa,
    // sin compartir recursos con otras ventanas.
    window = glfwCreateWindow ( 1024, 576, "PAG Introduction", nullptr, nullptr );

    // - Comprobamos si la creación de la ventana ha tenido éxito.
    if ( window == nullptr )
        //como es un error lo dejo para que se ponga en la consola
    {  std::cout << "Failed to open GLFW window" << std::endl;
        glfwTerminate (); // - Liberamos los recursos que ocupaba GLFW.
        return -2;
    }

    // - Hace que el contexto OpenGL asociado a la ventana que acabamos de crear pase a
    // ser el contexto actual de OpenGL para las siguientes llamadas a la biblioteca
    glfwMakeContextCurrent ( window );

    // - Ahora inicializamos GLAD.
    if ( !gladLoadGLLoader ( (GLADloadproc) glfwGetProcAddress ) )
    {
        //como es un error lo dejo para que se ponga en la consola
        std::cout << "GLAD initialization failed" << std::endl;
        glfwDestroyWindow ( window ); // - Liberamos los recursos que ocupaba GLFW.
        window = nullptr;
        glfwTerminate ();
        return -3;
    }

    // - Registramos los callbacks que responderán a los eventos principales
    glfwSetWindowRefreshCallback ( window, callbackRefrescoVentana );
    glfwSetFramebufferSizeCallback ( window, framebuffer_size_callback );
    glfwSetKeyCallback ( window, key_callback );
    glfwSetMouseButtonCallback ( window, mouseButtonCallback );
    glfwSetScrollCallback ( window, scroll_callback );
    //añadida
    glfwSetCursorPosCallback(window, posicion_cursor_callback);




/*

    // - Interrogamos a OpenGL para que nos informe de las propiedades del contexto
    // 3D construido.
    std::cout << glGetString ( GL_RENDERER ) << std::endl
              << glGetString ( GL_VENDOR ) << std::endl
              << glGetString ( GL_VERSION ) << std::endl
              << glGetString ( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
*/


    // - Establecemos un gris medio como color con el que se borrará el frame buffer.
    // No tiene por qué ejecutarse en cada paso por el ciclo de eventos.
    //glClearColor ( colorfondo[0] , colorfondo[1] , colorfondo[2] , colorfondo[3] );

    // - Le decimos a OpenGL que tenga en cuenta la profundidad a la hora de dibujar.
    // No tiene por qué ejecutarse en cada paso por el ciclo de eventos.
    //glEnable ( GL_DEPTH_TEST );



    //IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io= ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Aquí w es el puntero a la ventana GLFW (GLFWWindow*)
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init();



    //no se donde hay que ponerlo
    PAG::Renderer::getInstancia().inicializaOpenGL();
    aniadirmensajesporventana(PAG::Renderer::getInstancia().informeopenGL());

    try {
        PAG::Renderer::getInstancia().creaShaderProgram("shader");
        aniadirmensajesporventana("Shader cargado correctamente");


        PAG::Renderer::getInstancia().inicializarSubrutinasIluminacion();
        PAG::Renderer::getInstancia().inicializarSubrutinasColorBase();


        std::string info = PAG::Renderer::getInstancia().querySubroutines();
        aniadirmensajesporventana(info);


        PAG::Renderer::getInstancia().creaShaderProgramSombras();
        aniadirmensajesporventana("Shader de sombras cargado");
        PAG::Renderer::getInstancia().setUsarSombras(false);

    } catch (const std::exception& e) {
        aniadirmensajesporventana(std::string("Error al cargar shader: ") + e.what());
    }

    auto& renderer = PAG::Renderer::getInstancia();

    //Luz ambiente
    renderer.aniadirLuz(std::make_unique<PAG::Luz>(
            PAG::TipoLuz::Ambiente,
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f),
            0.0f, 1.0f,
            glm::vec3(0.2f, 0.2f, 0.2f)
    ));

    // Luz puntual roja
    renderer.aniadirLuz(std::make_unique<PAG::Luz>(
            PAG::TipoLuz::Puntual,
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(2.0f, 2.0f, 2.0f)
    ));

    // Luz direccional azul
    renderer.aniadirLuz(std::make_unique<PAG::Luz>(
            PAG::TipoLuz::Direccional,
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
    ));

    //Luz focal
    renderer.aniadirLuz(std::make_unique<PAG::Luz>(
            PAG::TipoLuz::Foco,
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 3.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::radians(30.0f),
            1.0f
    ));


    fileDialogModelos.SetTitle("Seleccionar modelo 3D");
    fileDialogModelos.SetTypeFilters({ ".obj", ".fbx", ".gltf", ".glb" });
    fileDialogModelos.SetPwd(std::filesystem::current_path()/"../modelos");

    fileDialogTexturas.SetTitle("Seleccionar texturaColor");
    fileDialogTexturas.SetTypeFilters({ ".png", ".jpg", ".jpeg", ".bmp" });
    fileDialogTexturas.SetPwd(std::filesystem::current_path()/"../texturas");

    fileDialogNormales.SetTitle("Seleccionar mapanormales");
    fileDialogNormales.SetTypeFilters({ ".png", ".jpg", ".jpeg", ".bmp" });
    fileDialogNormales.SetPwd(std::filesystem::current_path()/"../mapaNormales");

// se supone que va aqui
/*try{
    Shader::getInstancia().creaShaderProgram();
}catch (const std::exception& e){
    aniadirmensajesporventana(e.what());
}
*/
/*
    Shader::getInstancia().creaModelo();
*/
    // - Ciclo de eventos de la aplicación. La condición de parada es que la
    // ventana principal deba cerrarse. Por ejemplo, si el usuario pulsa el
    // botón de cerrar la ventana (la X).

    while ( !glfwWindowShouldClose ( window ) )
    {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //Se dibujan los controles de Dear ImGui
        ImGui::SetNextWindowPos ( ImVec2 (10, 10), ImGuiCond_Once );
        if(ImGui::Begin("Mensajes"))
        {
            //La ventana está despegada
            ImGui::SetWindowFontScale(1.0f); //Escalamos el texto si fuera necesario

            for (const auto& mensaje : mensajesporventana) {
                ImGui::TextUnformatted(mensaje.c_str());
            }

            if (ImGui::GetScrollY()>= ImGui::GetScrollMaxY()){
                ImGui::SetScrollHereY(1);
            }

            //Pintamos los controles

            //si la ventana no esta desplegada devuelve false
            ImGui::End();
        }
        //Aqui ve el dibujado de la escena con instrucciones OpenGL


        ImGui::SetNextWindowPos ( ImVec2 (700, 10), ImGuiCond_Once );

        if(ImGui::Begin("Fondo"))
        {
            //La ventana está despegada
            ImGui::SetWindowFontScale(1.0f); //Escalamos el texto si fuera necesario
            //Pintamos los controles
            if(ImGui::ColorPicker4("color de fondo", (float*)colorfondo,ImGuiColorEditFlags_PickerHueWheel)){
                PAG::Renderer::getInstancia().colorfondo(colorfondo[0],colorfondo[1],colorfondo[2]);
            }

            //si la ventana no esta desplegada devuelve false
            ImGui::End();
        }


        static std::string _name="";
        ImGui::SetNextWindowPos ( ImVec2 (500, 100), ImGuiCond_Once );
        if(ImGui::Begin("Shader Program"))
        {
            //La ventana está despegada
            ImGui::SetWindowFontScale(1.0f); //Escalamos el texto si fuera necesario
            //Pintamos los controles
            ImGui::InputText("##",&_name,ImGuiInputTextFlags_AutoSelectAll);


            bool _buttonPressed = ImGui::Button("Load");

            if (_buttonPressed && !_name.empty()){
                try{

                    PAG::Renderer::getInstancia().creaShaderProgram(_name);
                    //esta llamada es para crear el modelo del triangulo
                    //PAG::Renderer::getInstancia().creaModelo();
                }catch (const std::exception& e){
                    aniadirmensajesporventana(e.what());
                }
            }

            //si la ventana no esta desplegada devuelve false
            ImGui::End();
        }

// no se como poner para que no afecte el movimiento del raton cuando se mueve el en otro menu
        ImGui::SetNextWindowPos(ImVec2(700, 200), ImGuiCond_Once);
        if (ImGui::Begin("Camara")){
            const char* vector_movimientos[] = {"orbit", "pan" , "dolly","crame","tilt","zoom"};
            static int movimiento_seleccionado_vector = 0;

            if (ImGui::Combo("Movement",&movimiento_seleccionado_vector,vector_movimientos,6)){
                movimiento_seleccionado= static_cast<PAG::Movimiento_Camara>(movimiento_seleccionado_vector);
                aniadirmensajesporventana(vector_movimientos[movimiento_seleccionado_vector]);
                PAG::Renderer::getInstancia().setMovimientoCamara(movimiento_seleccionado);
            }
            if (ImGui::Button("Reseteo Camara")){
                PAG::Renderer::getInstancia().reseteo_camara();
            }
            ImGui::End();
        }
        ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_Once);
        if (ImGui::Begin("Modelos")) {
            auto& renderer = PAG::Renderer::getInstancia();

            // Botón para abrir el FileBrowser
            if (ImGui::Button("Cargar modelo")) {
                fileDialogModelos.Open();
            }

            ImGui::Separator();
            ImGui::Text("Modelos cargados: %d", renderer.getNumModelos());

            // Lista de modelos para seleccionar
            for (int i = 0; i < renderer.getNumModelos(); i++) {
                bool seleccionado = (i == renderer.getModeloSeleccionado());
                if (ImGui::Selectable(renderer.getNombreModelo(i).c_str(), seleccionado)) {
                    renderer.seleccionarModelo(i);
                }
            }

            ImGui::Separator();

            // Transformaciones del modelo seleccionado
            if (renderer.getModeloSeleccionado() >= 0) {
                ImGui::Text("Transformando: %s",renderer.getNombreModelo(renderer.getModeloSeleccionado()).c_str());
                //para obtener la posición actual del modelo
                glm::mat4 transformActual = renderer.getTransformModeloSeleccionado();
                glm::vec3 posActual = glm::vec3(transformActual[3]);
                ImGui::Text("Posicion actual: (%.2f, %.2f, %.2f)", posActual.x, posActual.y, posActual.z);
                // Traslación
                static float trasl[3] = {0, 0, 0};
                if (ImGui::DragFloat3("Trasladar", trasl, 0.01f)) {}
                if (ImGui::Button("Aplicar traslacion")) {
                    renderer.trasladarModeloSeleccionado(glm::vec3(trasl[0], trasl[1], trasl[2]));
                    trasl[0] = trasl[1] = trasl[2] = 0;
                }

                // Rotación
                static float angulo = 0;
                static int ejeIdx = 1;
                const char* ejes[] = {"X", "Y", "Z"};
                ImGui::Combo("Eje", &ejeIdx, ejes, 3);
                if (ImGui::SliderFloat("Rotar (grados)", &angulo, -180, 180)) {}
                if (ImGui::Button("Aplicar rotacion")) {
                    glm::vec3 eje = (ejeIdx == 0) ? glm::vec3(1,0,0) :
                                    (ejeIdx == 1) ? glm::vec3(0,1,0) : glm::vec3(0,0,1);
                    renderer.rotarModeloSeleccionado(glm::radians(angulo), eje);
                    angulo = 0;
                }

                // Escalado
                static float escala[3] = {1, 1, 1};
                if (ImGui::DragFloat3("Escalar", escala, 0.01f, 0.01f, 10.0f)) {}
                if (ImGui::Button("Aplicar escalado")) {
                    renderer.escalarModeloSeleccionado(glm::vec3(escala[0], escala[1], escala[2]));
                    escala[0] = escala[1] = escala[2] = 1.0f;
                }

                ImGui::Separator();
                if (ImGui::Button("Reset transform")) {
                    renderer.resetTransformModeloSeleccionado();
                }
                if (ImGui::Button("Eliminar modelo")) {
                    renderer.eliminarModeloSeleccionado();
                }
            } else {
                ImGui::TextDisabled("Selecciona un modelo para transformarlo");
            }

            ImGui::Separator();
            ImGui::Text("Material del modelo seleccionado:");

            if (renderer.getModeloSeleccionado() >= 0) {
                static int modeloMostrado = -1;

                static float colAmb[3] = {0.2f, 0.2f, 0.2f};
                static float colDif[3] = {0.8f, 0.8f, 0.8f};
                static float colEsp[3] = {1.0f, 1.0f, 1.0f};
                static float shininess = 32.0f;

                //para mostrar los valores del modelo selecionado respecto al material
                if(renderer.getModeloSeleccionado() != modeloMostrado){
                    PAG::Material& materialSeleccionado= renderer.getModeloSeleccionadoMaterial();

                    glm::vec3 ca = materialSeleccionado.getAmbiente();
                    glm::vec3 cd = materialSeleccionado.getDifuso();
                    glm::vec3 ce = materialSeleccionado.getEspecular();

                    colAmb[0] = ca.x;
                    colAmb[1] = ca.y;
                    colAmb[2] = ca.z;
                    colDif[0] = cd.x;
                    colDif[1] = cd.y;
                    colDif[2] = cd.z;
                    colEsp[0] = ce.x;
                    colEsp[1] = ce.y;
                    colEsp[2] = ce.z;
                    shininess = materialSeleccionado.getShininess();

                    modeloMostrado = renderer.getModeloSeleccionado();
                }


                ImGui::ColorEdit3("Color ambiente", colAmb);
                ImGui::ColorEdit3("Color difuso", colDif);
                ImGui::ColorEdit3("Color especular", colEsp);
                ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);

                if (ImGui::Button("Aplicar material")) {
                    // Guardar la texturaColor actual
                    PAG::Material& matActual = renderer.getModeloSeleccionadoMaterial();
                    PAG::Textura* texActual = matActual.getTextura();

                    // Crear nuevo material
                    PAG::Material nuevoMat(
                            glm::vec3(colAmb[0], colAmb[1], colAmb[2]),
                            glm::vec3(colDif[0], colDif[1], colDif[2]),
                            glm::vec3(colEsp[0], colEsp[1], colEsp[2]),
                            shininess
                    );

                    // Restaurar la texturaColor si existía
                    if (texActual != nullptr) {
                        nuevoMat.setTextura(texActual);
                    }

                    renderer.setMaterialModeloSeleccionado(nuevoMat);
                }

                ImGui::Separator();

                // Modo de visualización
                static int modoIdx = 1;  // 0=Alambre, 1=Plano
                const char* modos[] = {"Alambre", "Plano"};
                if (ImGui::Combo("Modo visualizacion", &modoIdx, modos, 2)) {
                    renderer.setModoVisualizacion(
                            modoIdx == 0 ? PAG::ModoVisualizacion::Alambre
                                         : PAG::ModoVisualizacion::Plano);
                }
            }

            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(10, 600), ImGuiCond_Once);
        if (ImGui::Begin("Luces")) {
            auto& renderer = PAG::Renderer::getInstancia();

            // Botones para añadir luces
            if (ImGui::Button("Añadir luz ambiente")) {
                renderer.aniadirLuz(std::make_unique<PAG::Luz>(
                        PAG::TipoLuz::Ambiente,
                        glm::vec3(0.0f), // colorDifuso (no se usa)
                        glm::vec3(0.0f), // colorEspecular (no se usa)
                        glm::vec3(0.0f), // posicion (no se usa)
                        glm::vec3(0.0f), // direccion (no se usa)
                        0.0f, 1.0f,
                        glm::vec3(0.2f, 0.2f, 0.2f)
                ));
            }
            if (ImGui::Button("Añadir luz puntual")) {
                renderer.aniadirLuz(std::make_unique<PAG::Luz>(
                        PAG::TipoLuz::Puntual,
                        glm::vec3(1.0f, 1.0f, 1.0f), // colorDifuso
                        glm::vec3(1.0f, 1.0f, 1.0f), // colorEspecular
                        glm::vec3(2.0f, 2.0f, 2.0f) // posicion
                ));
            }
            if (ImGui::Button("Añadir luz direccional")) {
                renderer.aniadirLuz(std::make_unique<PAG::Luz>(
                        PAG::TipoLuz::Direccional,
                        glm::vec3(1.0f, 1.0f, 1.0f), // colorDifuso
                        glm::vec3(1.0f, 1.0f, 1.0f), // colorEspecular
                        glm::vec3(0.0f), // posicion (no se usa)
                        glm::vec3(0.0f, -1.0f, 0.0f) // direccion
                ));
            }
            if (ImGui::Button("Añadir foco")) {
                renderer.aniadirLuz(std::make_unique<PAG::Luz>(
                        PAG::TipoLuz::Foco,
                        glm::vec3(1.0f, 1.0f, 1.0f), // colorDifuso
                        glm::vec3(1.0f, 1.0f, 1.0f), // colorEspecular
                        glm::vec3(0.0f, 3.0f, 0.0f), // posicion
                        glm::vec3(0.0f, -1.0f, 0.0f), // direccion
                        glm::radians(30.0f) // anguloApertura
                ));
            }

            ImGui::Separator();
            ImGui::Text("Luces en la escena: %d", renderer.getNumLuces());

            // Lista de luces
            for (int i = 0; i < renderer.getNumLuces(); i++) {
                bool seleccionado = (i == renderer.getLuzSeleccionada());
                if (ImGui::Selectable(("Luz " + std::to_string(i)).c_str(), seleccionado)) {
                    renderer.seleccionarLuz(i);
                }
            }

            ImGui::Separator();

            // para ver los valores de la luz seleccionada
            if (renderer.getLuzSeleccionada() >= 0) {
                PAG::Luz* luz = renderer.getLuz(renderer.getLuzSeleccionada());


                static int luzMostrada = -1;
                static int tipoIdx = 0;
                static float colAmb[3] = {0.2f, 0.2f, 0.2f};
                static float colDif[3] = {1.0f, 1.0f, 1.0f};
                static float colEsp[3] = {1.0f, 1.0f, 1.0f};
                static float pos[3] = {2.0f, 2.0f, 2.0f};
                static float dir[3] = {0.0f, -1.0f, 0.0f};
                static float angulo = 30.0f;

                if(renderer.getLuzSeleccionada() != luzMostrada){
                    tipoIdx = static_cast<int>(luz->getTipo());

                    glm::vec3 ca = luz->getColorAmbiente();
                    glm::vec3 cd = luz->getColorDifuso();
                    glm::vec3 ce = luz->getColorEspecular();
                    colAmb[0] = ca.x;
                    colAmb[1] = ca.y;
                    colAmb[2] = ca.z;
                    colDif[0] = cd.x;
                    colDif[1] = cd.y;
                    colDif[2] = cd.z;
                    colEsp[0] = ca.x;
                    colEsp[1] = ca.x;
                    colEsp[2] = ca.x;

                    glm::vec3 p = luz->getPosicion();
                    pos[0] = p.x;
                    pos[1] = p.y;
                    pos[2] = p.z;

                    glm::vec3  d = luz->getDireccion();
                    dir[0] = d.x;
                    dir[1] = d.y;
                    dir[2] = d.z;

                    angulo = glm::degrees(luz->getAnguloApertura());

                    luzMostrada = renderer.getLuzSeleccionada();

                }

                // Tipo de luz
                const char* tipos[] = {"Ambiente", "Puntual", "Direccional", "Foco"};
                if (ImGui::Combo("Tipo", &tipoIdx, tipos, 4)) {
                    luz->setTipo(static_cast<PAG::TipoLuz>(tipoIdx));
                }

                ImGui::ColorEdit3("Color ambiente", colAmb);
                ImGui::ColorEdit3("Color difuso", colDif);
                ImGui::ColorEdit3("Color especular", colEsp);

                if (ImGui::Button("Aplicar colores")) {
                    luz->setColorAmbiente(glm::vec3(colAmb[0], colAmb[1], colAmb[2]));
                    luz->setColorDifuso(glm::vec3(colDif[0], colDif[1], colDif[2]));
                    luz->setColorEspecular(glm::vec3(colEsp[0], colEsp[1], colEsp[2]));
                }

                // Posición (para puntual y foco)
                if (luz->getTipo() == PAG::TipoLuz::Puntual || luz->getTipo() == PAG::TipoLuz::Foco) {
                    static float pos[3] = {2.0f, 2.0f, 2.0f};
                    ImGui::DragFloat3("Posición", pos, 0.1f);
                    if (ImGui::Button("Aplicar posición")) {
                        luz->setPosicion(glm::vec3(pos[0], pos[1], pos[2]));
                    }
                }

                // Dirección (para direccional y foco)
                if (luz->getTipo() == PAG::TipoLuz::Direccional || luz->getTipo() == PAG::TipoLuz::Foco) {
                    static float dir[3] = {0.0f, -1.0f, 0.0f};
                    ImGui::DragFloat3("Dirección", dir, 0.1f);
                    if (ImGui::Button("Aplicar dirección")) {
                        luz->setDireccion(glm::make_vec3(dir));
                    }
                }

                // Ángulo de apertura (para foco)
                if (luz->getTipo() == PAG::TipoLuz::Foco) {
                    static float angulo = 30.0f;
                    ImGui::SliderFloat("Ángulo apertura (grados)", &angulo, 1.0f, 90.0f);
                    if (ImGui::Button("Aplicar ángulo")) {
                        luz->setAnguloApertura(glm::radians(angulo));
                    }
                }

                if (ImGui::Button("Eliminar luz")) {
                    renderer.eliminarLuz(renderer.getLuzSeleccionada());
                }
            }

            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(2000, 600), ImGuiCond_Once);
        if (ImGui::Begin("Texturas")) {
            auto& renderer = PAG::Renderer::getInstancia();

            if (ImGui::Button("Cargar texturaColor")) {
                fileDialogTexturas.Open();
            }

            ImGui::Separator();
            ImGui::Text("Texturas cargadas:");

            for (const auto& [nombre, tex] : renderer.getTexturas()) {
                if (ImGui::Selectable(nombre.c_str(), false)) {
                    renderer.asignarTexturaAModeloSeleccionado(nombre);
                    aniadirmensajesporventana("Textura asignada: " + nombre);
                }
            }

            ImGui::Separator();

            static int modoColorIdx = 0;
            const char* modosColor[] = { "Material", "Textura" };
            if (ImGui::Combo("Modo color", &modoColorIdx, modosColor, 2)) {
                renderer.setModoColor(
                        modoColorIdx == 0 ? PAG::ModoColor::Material
                                          : PAG::ModoColor::Textura);
            }

            ImGui::Separator();

            static bool normalMappingActivo = false;
            if (ImGui::Checkbox("Activar Normal Mapping", &normalMappingActivo)) {
                renderer.setUsarNormalMapping(normalMappingActivo);
            }

            if (ImGui::Button("Cargar mapa de normales")) {
                fileDialogNormales.Open();
            }

            ImGui::Separator();
            static bool sombrasActivas = false;
            if (ImGui::Checkbox("Activar sombras proyectadas", &sombrasActivas)) {
                renderer.setUsarSombras(sombrasActivas);
            }

            ImGui::End();

        }

        fileDialogModelos.Display();
        if (fileDialogModelos.HasSelected()) {
            std::string ruta = fileDialogModelos.GetSelected().string();
            std::string nombre = fileDialogModelos.GetSelected().filename().string();

            try {
                PAG::Renderer::getInstancia().cargarModelo(ruta, nombre);
                aniadirmensajesporventana("Modelo cargado: " + nombre);
            } catch (const std::exception& e) {
                aniadirmensajesporventana(std::string("Error al cargar: ") + e.what());
            }

            fileDialogModelos.ClearSelected();
        }

        fileDialogTexturas.Display();
        if (fileDialogTexturas.HasSelected()) {
            std::string ruta = fileDialogTexturas.GetSelected().string();
            std::string nombre = fileDialogTexturas.GetSelected().filename().string();

            try {
                auto& renderer = PAG::Renderer::getInstancia();
                renderer.cargarTextura(ruta, nombre);
                aniadirmensajesporventana("Textura cargada: " + nombre);
            } catch (const std::exception & e) {
                aniadirmensajesporventana(std::string("Error al cargar texturaColor: ") + e.what());
            }

            fileDialogTexturas.ClearSelected();
        }

        fileDialogNormales.Display();
        if (fileDialogNormales.HasSelected()) {
            std::string ruta = fileDialogNormales.GetSelected().string();
            std::string nombre = fileDialogNormales.GetSelected().filename().string();
            try {
                auto& renderer = PAG::Renderer::getInstancia();
                renderer.cargarTextura(ruta, nombre);
                renderer.asignarNormalMapAModeloSeleccionado(nombre);
                aniadirmensajesporventana("Mapa de normales cargado y asignado: " + nombre);
            } catch (const std::exception & e) {
                aniadirmensajesporventana(std::string("Error al cargar mapa de normales: ") + e.what());
            }
            fileDialogNormales.ClearSelected();
        }


        PAG::Renderer::getInstancia().refrescar();

        ImGui::Render();
        //PAG::Renderer::getInstancia().colorfondo(colorfondo[0],colorfondo[1],colorfondo[2]);
        //PAG::Renderer::getInstancia().refrescar();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());












        // - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
        // intercambia el buffer back (en el que se ha estado dibujando) por el
        // que se mostraba hasta ahora (front).
        glfwSwapBuffers ( window );
        // - Obtiene y organiza los eventos pendientes, tales como pulsaciones de
        // teclas o de ratón, etc. Siempre al final de cada iteración del ciclo
        // de eventos y después de glfwSwapBuffers(window);
        glfwPollEvents ();
    }

    // - Una vez terminado el ciclo de eventos, liberar recursos, etc.
    aniadirmensajesporventana("Finishing application pag prueba");
    //std::cout << "Finishing application pag prueba" << std::endl;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //glfwDestroyWindow(window);

    glfwDestroyWindow ( window ); // - Cerramos y destruimos la ventana de la aplicación.
    window = nullptr;
    glfwTerminate (); // - Liberamos los recursos que ocupaba GLFW.

}



//cosas extras
//Redimensionar la ventana al tamaño de los controles que contiene
//if(ImGui::Begin(getTitle().c_str(),nullptr,ImGuioWindowFlags_AlwaysAutoResize))
//Cambiar la escala del texto (1.0 es sin escalar)
//ImGui::SetWindowsFrontScale(_MYGUI_TEXT_SCALE_);
//Combo sencillo con un vector con char o a pelo
//if(_changedM == ImGui::Combo("##MovType",(int*)&_active,"None\0Zoom\0Crane\0Dolly\0Pan\0Tilt\0Orbit\0\0"))
//switch (_active)
  //      {case Movement::Zoom:}
