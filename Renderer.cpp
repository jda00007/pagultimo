//
// Created by programas on 23/09/2025.
//


#include <cstdarg>
#include "Renderer.h"
//#include "imgui_impl_opengl3_loader.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


namespace PAG {
    Renderer* Renderer::instancia=nullptr;

    Renderer::Renderer()
            : indiceModeloSeleccionado(-1),
              indiceLuzSeleccionada(-1),
              texturaActual(nullptr),
              texturaActiva(false),
              modoVisualizacion(ModoVisualizacion::Plano),
              modoColor(ModoColor::Material),
              rojo(0.6f), verde(0.6f), azul(0.6f),
              anchoVentana(1024), altoVentana(576),
              idVAO(0), idVBO(0), idIBO(0),
              idVS(0), idFS(0), idSP(0),
              idVSSombras(0), idFSSombras(0), idSPSombras(0),
              _fboSombras(0),
              _usarSombras(false),
              _anchoShadowMap(1024),
              _altoShadowMap(1024),
              usarNormalMapping(false) {
    }

    Renderer::~Renderer() {

        if ( idVS!=0){
            glDeleteShader (idVS);
        }

        if (idFS!=0){
            glDeleteShader (idFS);
        }

        if (idSP!=0){
            glDeleteProgram (idSP);
        }

        if (idVBO!=0){
            glDeleteBuffers (1,&idVBO);
        }

        if (idIBO!=0){
            glDeleteBuffers (1,&idIBO);
        }

        if (idVAO!=0){
            glDeleteVertexArrays (1,&idVAO);
        }

    }

    void PAG::Renderer::inicializaOpenGL() {
        glClearColor(rojo,verde, azul, 1.0 );
        anchoVentana=1024;
        altoVentana=576;
        glViewport(0,0,anchoVentana,altoVentana);
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_MULTISAMPLE );
    }

    Renderer& Renderer::getInstancia() {
        if(!instancia){
            instancia=new Renderer;
        }
        return *instancia;
    }


//modificado para que incluir el shader
    void Renderer::refrescar() {

        // si hay sombras activadas, feneramos los mapas de progundidas
        if (_usarSombras && idSPSombras != 0) {
            if (_fboSombras == 0) {
                crearFBOSombras();
            }
            calcularMatricesLuz();
            renderizarShadowMaps();
        }

        //renderizado de la escena
        glClearColor(rojo, verde, azul, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(idSP);

        glm::mat4 view = camara.getViewMatrix();
        float aspectRatio = (altoVentana > 0) ?
                            (float)anchoVentana / (float)altoVentana : 1.0f;
        glm::mat4 projection = camara.getProjectionMatrix(aspectRatio);

        glUniformMatrix4fv(glGetUniformLocation(idSP, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(idSP, "projection"), 1, GL_FALSE, &projection[0][0]);

        //asignamos una textua fija para cada sampler
        glUniform1i(glGetUniformLocation(idSP, "uTextura"), 0);
        glUniform1i(glGetUniformLocation(idSP, "uMapaNormales"), 1);
        glUniform1i(glGetUniformLocation(idSP, "muestreadorSombra"), 2);
        glUniform1i(glGetUniformLocation(idSP, "uAplicarSombra"), 0);

        if (luces.empty()) {
            glDisable(GL_BLEND);
            glDepthFunc(GL_LESS);
            return;
        }

        GLint posIluminacion = glGetSubroutineUniformLocation(idSP, GL_FRAGMENT_SHADER, "uTipoLuz");
        GLint posColorBase = glGetSubroutineUniformLocation(idSP, GL_FRAGMENT_SHADER, "uColorBase");

        if (posIluminacion == -1 || posColorBase == -1) {
            std::cerr << "[ERROR] Subrutinas no encontradas en el shader." << std::endl;
            return;
        }

        GLint numSubrutinas = 0;
        glGetProgramStageiv(idSP, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubrutinas);
        std::vector<GLuint> subrutinas(numSubrutinas, 0);

        //por cada modelo debemos ver su textura y/o su mapa de normales
        for (const auto& modelo : modelos) {
            const Material& mat = modelo->getMaterial();
            Textura* tex = mat.getTextura();
            Textura* texNormales = mat.getTexturaNormales();

            bool usarTextura = (modoColor == ModoColor::Textura && tex && tex->estaCargada());
            bool usarNM = usarNormalMapping && texNormales && texNormales->estaCargada();

            // Activar textura de color
            if (usarTextura) {
                tex->activar(0);
            }

            // Activar mapa de normales
            if (usarNM) {
                texNormales->activar(1);
                glUniform1i(glGetUniformLocation(idSP, "uUsarNormalMap"), 1);
            } else {
                glUniform1i(glGetUniformLocation(idSP, "uUsarNormalMap"), 0);
            }

            // Enviar propiedades del material
            glUniform3fv(glGetUniformLocation(idSP, "uColorAmbiente"), 1, &mat.getAmbiente()[0]);
            glUniform3fv(glGetUniformLocation(idSP, "uColorDifuso"), 1, &mat.getDifuso()[0]);
            glUniform3fv(glGetUniformLocation(idSP, "uColorEspecular"), 1, &mat.getEspecular()[0]);
            glUniform1f(glGetUniformLocation(idSP, "uShininess"), mat.getShininess());

            // por defecto mandamos la matriz identidad
            glm::mat4 matrizSombrasSafe = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(idSP, "matrizSombras"), 1, GL_FALSE, &matrizSombrasSafe[0][0]);

            GLuint indiceColorBase = usarTextura ? indiceColorTextura : indiceColorMaterial;

            // bucle para manejar las luces ya que cada una aporta sobre el modelo
            for (int i = 0; i < (int)luces.size(); i++) {
                const Luz& luz = *luces[i];

                if (i == 0) {
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                } else {
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                }

                // Seleccionar tipo de luz
                GLuint indiceIluminacion;
                switch (luz.getTipo()) {
                    case TipoLuz::Ambiente:
                        indiceIluminacion = indiceLuzAmbiente;
                        break;
                    case TipoLuz::Puntual:
                        indiceIluminacion = indiceLuzPuntual;
                        break;
                    case TipoLuz::Direccional:
                        indiceIluminacion = indiceLuzDireccional;
                        break;
                    case TipoLuz::Foco:
                        indiceIluminacion = indiceLuzFoco;
                        break;
                    default:
                        indiceIluminacion = indiceLuzAmbiente;
                        break;
                }

                // Solo enviar subrutinas si los índices son válidos
                if (posIluminacion >= 0 && posColorBase >= 0 &&
                    indiceIluminacion != GL_INVALID_INDEX && indiceColorBase != GL_INVALID_INDEX) {
                    subrutinas[posIluminacion] = indiceIluminacion;
                    subrutinas[posColorBase] = indiceColorBase;
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, numSubrutinas, subrutinas.data());
                }

                // Modo de polígonos
                if (modoVisualizacion == ModoVisualizacion::Alambre) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                // Uniforms de luz
                glUniform3fv(glGetUniformLocation(idSP, "uLuzAmbiente"), 1, &luz.getColorAmbiente()[0]);
                glUniform3fv(glGetUniformLocation(idSP, "uLuzDifusa"), 1, &luz.getColorDifuso()[0]);
                glUniform3fv(glGetUniformLocation(idSP, "uLuzEspecular"), 1, &luz.getColorEspecular()[0]);

                glm::vec3 posicionLuzV = glm::vec3(view * glm::vec4(luz.getPosicion(), 1.0));
                glm::vec3 direccionLuzV = glm::vec3(view * glm::vec4(luz.getDireccion(), 0.0));

                glUniform3fv(glGetUniformLocation(idSP, "uPosicionLuzV"), 1, &posicionLuzV[0]);
                glUniform3fv(glGetUniformLocation(idSP, "uDireccionLuzV"), 1, &direccionLuzV[0]);
                glUniform1f(glGetUniformLocation(idSP, "uAnguloApertura"), luz.getAnguloApertura());
                glUniform1f(glGetUniformLocation(idSP, "uExponenteBordes"), luz.getExponenteBordes());

                // solo las luces direccionales y foco tienen mapade sombra asociado
                bool aplicarSombra = _usarSombras &&
                                     (luz.getTipo() == TipoLuz::Direccional ||
                                      luz.getTipo() == TipoLuz::Foco);

                glUniform1i(glGetUniformLocation(idSP, "uAplicarSombra"), aplicarSombra ? 1 : 0);

                if (aplicarSombra) {
                    auto itVision = _matricesVisionLuz.find(luces[i].get());
                    auto itProy = _matricesProyeccionLuz.find(luces[i].get());
                    auto itTex = _texturasSombras.find(luces[i].get());

                    if (itVision != _matricesVisionLuz.end() &&
                        itProy != _matricesProyeccionLuz.end() &&
                        itTex != _texturasSombras.end()) {

                        glm::mat4 matrizVision = itVision->second;
                        glm::mat4 matrizProyeccion = itProy->second;
                        glm::mat4 matrizModelo = modelo->getTransform();

                        glm::mat4 matrizSombras = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
                        matrizSombras[3][0] = 0.5f;
                        matrizSombras[3][1] = 0.5f;
                        matrizSombras[3][2] = 0.5f;
                        matrizSombras = matrizSombras * matrizProyeccion * matrizVision * matrizModelo;

                        glUniformMatrix4fv(glGetUniformLocation(idSP, "matrizSombras"), 1, GL_FALSE, &matrizSombras[0][0]);

                        GLuint texId = itTex->second;
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, texId);

                    }
                }


                // Enviar matriz del modelo
                glUniformMatrix4fv(glGetUniformLocation(idSP, "model"), 1, GL_FALSE, &modelo->getTransform()[0][0]);

                modelo->draw(shader);

            }

            // Desactivar texturas después de renderizar este modelo
            if (usarTextura) {
                tex->desactivar();
            }
            if (usarNM) {
                texNormales->desactivar();
            }
        }

        int modoVis = (modoVisualizacion == ModoVisualizacion::Alambre) ? 0 : 1;
        glUniform1i(glGetUniformLocation(idSP, "uModoVisualizacion"), modoVis);

        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

  /*  void Renderer::wakeUp(WindowType t, ... ){
        switch (t) {
            case WindowType::Background:
            {
                std::va_list args;
                va_start(args,t);
                _bgColor = *(va_arg(args,glm::vec3*));
                _showAxis=*(va_arg(args,bool*));
                va_end(args);
                break;
            }
            //procesar el resto de tipos de ventana

        }
        //termianr cualquier otro tipo de procesamiento que sea necesario
    }*/


  void Renderer::colorfondo(float rojo1,float verde1,float azul1){
      rojo=rojo1;
      verde=verde1;
      azul=azul1;


      glClearColor(rojo,verde,azul,1);
  }

  /*
   * Método para crear, compilar y enlazar el shader program
   * @note No se incluye ninguna comprobación de errores
   */

  void PAG::Renderer::creaShaderProgram(const std::string& nombre) {
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
      //llamo al crea shader de Shader.h
      Shader::creaShaderProgram(nombre,idVS,idFS,idSP);

      //inicializarSubrutinas();

      inicializarSubrutinasIluminacion();

      inicializarSubrutinasColorBase();
      shader.ID = idSP;
      /*
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

      std::string rutaVertex= "../Shaders/pag03-vs.glsl";
      std::string rutaFragment= "../Shaders/pag03-fs.glsl";

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
      glGetShaderiv ( idVS, GL_COMPILE_STATUS, &resultadoCompilacion );

      if ( resultadoCompilacion == GL_FALSE )
      {  // Ha habido un error en la compilación.
          // Para saber qué ha pasado, tenemos que recuperar el mensaje de error de OpenGL
          GLint tamMsj = 0;
          std::string mensaje = "";
          glGetShaderiv ( idVS, GL_INFO_LOG_LENGTH, &tamMsj );
          if ( tamMsj > 0 )
          {  GLchar* mensajeFormatoC = new GLchar[tamMsj];
              GLint datosEscritos = 0;
              glGetShaderInfoLog ( idVS, tamMsj, &datosEscritos, mensajeFormatoC );
              mensaje.assign ( mensajeFormatoC );
              delete[] mensajeFormatoC;
              mensajeFormatoC = nullptr;
              // En "mensaje" tenemos la información del error. Comunicarla de alguna
              // forma
              throw std::runtime_error(mensaje);
          }
      }


      const GLchar* codigofuenteFormatoCF = codigoFuenteShaderFag.c_str();
      glShaderSource(idFS,1,&codigofuenteFormatoCF,NULL);
      glCompileShader(idFS);

      GLint resultadoCompilacion2;
      glGetShaderiv ( idFS, GL_COMPILE_STATUS, &resultadoCompilacion2 );

      if ( resultadoCompilacion2 == GL_FALSE )
      {  // Ha habido un error en la compilación.
          // Para saber qué ha pasado, tenemos que recuperar el mensaje de error de OpenGL
          GLint tamMsj = 0;
          std::string mensaje = "";
          glGetShaderiv ( idFS, GL_INFO_LOG_LENGTH, &tamMsj );
          if ( tamMsj > 0 )
          {  GLchar* mensajeFormatoC = new GLchar[tamMsj];
              GLint datosEscritos = 0;
              glGetShaderInfoLog ( idFS, tamMsj, &datosEscritos, mensajeFormatoC );
              mensaje.assign ( mensajeFormatoC );
              delete[] mensajeFormatoC;
              mensajeFormatoC = nullptr;
              // En "mensaje" tenemos la información del error. Comunicarla de alguna
              // forma
              throw std::runtime_error(mensaje);
          }
      }



      glAttachShader ( idSP, idVS );
      glAttachShader ( idSP, idFS);

      glLinkProgram ( idSP );
      GLint resultadoEnlazado = 0;
      glGetProgramiv ( idSP, GL_LINK_STATUS, &resultadoEnlazado );
      if ( resultadoEnlazado == GL_FALSE )
      {  // Ha habido un error y hay que recuperar su descripción, para saber qué ha pasado
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
              // En "mensaje" tenemos la información del error. Comunicarla de alguna forma
              throw std::runtime_error(mensaje);
          }
      }


      idVS = glCreateShader ( GL_VERTEX_SHADER );
      const GLchar* fuenteVS = miVertexShader.c_str();
      glShaderSource (idVS,1,&fuenteVS, nullptr);
      glCompileShader( idFS);

      idSP = glCreateProgram ();
      glAttachShader (idSP, idVS);
      glAttachShader (idSP, idFS);
      glLinkProgram ( idSP);

*/  }


  void PAG::Renderer::creaModelo() {

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

  std::string PAG::Renderer::informeopenGL() {

     std::string aux;
      std::ostringstream oss;
      oss << glGetString(GL_RENDERER) << '\n'
          << glGetString(GL_VENDOR) << '\n'
          << glGetString(GL_VERSION) << '\n'
          << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
      aux = oss.str();
      return aux;
  }
  void PAG::Renderer::llamadaviewport(int width, int height) {
      glViewport ( 0, 0, width, height );
  }

  void PAG::Renderer::setMovimientoCamara(PAG::Movimiento_Camara movimiento) {
      movimientoCamara=movimiento;
  }
  void PAG::Renderer::movimientoraton(float deltaX, float deltaY) {

      //printf("Movimiento" );
      switch (movimientoCamara) {
          case PAG::Movimiento_Camara::orbit:
              camara.orbit(deltaX * 0.01f, deltaY * 0.01f);
              break;
          case PAG::Movimiento_Camara::pan:
              camara.pan(deltaX * 0.01f, deltaY * 0.01f);
              break;
          case PAG::Movimiento_Camara::dolly:
              camara.dolly(deltaY * 0.05f);
              break;
          case PAG::Movimiento_Camara::tilt:
              camara.tilt(glm::radians(deltaY));
              break;
          case PAG::Movimiento_Camara::crane:
              camara.crane(deltaY*0.01f);
              break;
          case PAG::Movimiento_Camara::zoom:
              camara.zoom(deltaY*0.05f);
              break;
          default:
              break;
      }
  }

  glm::mat4 PAG::Renderer::getViewMatrix() const {
      return camara.getViewMatrix();
  }

  void Renderer::setViewport(int ancho, int alto) {
      anchoVentana = ancho;
      altoVentana = alto;
      glViewport(0, 0, ancho, alto);
  }

  void Renderer::cargarModelo(const std::string& ruta, const std::string& nombre) {
      auto modelo = std::make_unique<Modelo>(ruta, nombre);
      modelos.push_back(std::move(modelo));

      indiceModeloSeleccionado = (int)modelos.size() - 1;
  }

  void Renderer::eliminarModelo(int indice) {
      if (indice < 0 || indice >= (int)modelos.size()) return;

      modelos.erase(modelos.begin() + indice);

      if (modelos.empty()) {
          indiceModeloSeleccionado = -1;
      } else if (indiceModeloSeleccionado >= (int)modelos.size()) {
          indiceModeloSeleccionado = (int)modelos.size() - 1;
      }
  }

  void Renderer::eliminarModeloSeleccionado() {
      eliminarModelo(indiceModeloSeleccionado);
  }

  void Renderer::seleccionarModelo(int indice) {
      if (indice >= -1 && indice < (int)modelos.size()) {
          indiceModeloSeleccionado = indice;
      }
  }

  const std::string& Renderer::getNombreModelo(int indice) const {
      static std::string vacio = "";
      if (indice < 0 || indice >= (int)modelos.size()) return vacio;
      return modelos[indice]->getNombre();
  }

  void Renderer::trasladarModeloSeleccionado(const glm::vec3& desplazamiento) {
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
      modelos[indiceModeloSeleccionado]->trasladar(desplazamiento);
  }

  void Renderer::rotarModeloSeleccionado(float angulo, const glm::vec3& eje) {
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
      modelos[indiceModeloSeleccionado]->rotar(angulo, eje);
  }

  void Renderer::escalarModeloSeleccionado(const glm::vec3& factor) {
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
      modelos[indiceModeloSeleccionado]->escalar(factor);
  }

  void Renderer::resetTransformModeloSeleccionado() {
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
      modelos[indiceModeloSeleccionado]->resetTransform();
  }

  void Renderer::setModoVisualizacion(ModoVisualizacion modo) {
      modoVisualizacion = modo;
  }

  void Renderer::inicializarSubrutinas() {
      glUseProgram(idSP);

      indiceSubrutinaFijo = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "colorDeMaterial");

      indiceSubrutinaMaterial = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "colorDeTextura");

      if (indiceSubrutinaFijo == GL_INVALID_INDEX ||indiceSubrutinaMaterial == GL_INVALID_INDEX) {
          throw std::runtime_error("Error: subrutinas no encontradas en el shader");
      }
  }

  void Renderer::setMaterialModeloSeleccionado(const Material& mat) {
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
      modelos[indiceModeloSeleccionado]->setMaterial(mat);
  }

  std::string Renderer::querySubroutines() const {
      return shader.querySubroutines();
  }

  void Renderer::aniadirLuz(std::unique_ptr<Luz> luz) {
      luces.push_back(std::move(luz));
      indiceLuzSeleccionada = (int)luces.size() - 1;
  }

  void Renderer::eliminarLuz(int indice) {
      if (indice < 0 || indice >= (int)luces.size()) return;

      luces.erase(luces.begin() + indice);

      if (luces.empty()) {
          indiceLuzSeleccionada = -1;
      } else if (indiceLuzSeleccionada >= (int)luces.size()) {
          indiceLuzSeleccionada = (int)luces.size() - 1;
      }
  }

  void Renderer::eliminarLuzSeleccionada() {
      eliminarLuz(indiceLuzSeleccionada);
  }

  void Renderer::seleccionarLuz(int indice) {
      if (indice >= -1 && indice < (int)luces.size()) {
          indiceLuzSeleccionada = indice;
      }
  }

  void Renderer::inicializarSubrutinasIluminacion() {
      glUseProgram(idSP);

      indiceLuzAmbiente = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "luzAmbiente");
      indiceLuzPuntual = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "luzPuntual");
      indiceLuzDireccional = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "luzDireccional");
      indiceLuzFoco = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "luzFoco");

      if (indiceLuzAmbiente == GL_INVALID_INDEX || indiceLuzPuntual == GL_INVALID_INDEX ||
          indiceLuzDireccional == GL_INVALID_INDEX || indiceLuzFoco == GL_INVALID_INDEX) {
          throw std::runtime_error("Error: implementaciones de subrutinas de iluminación no encontradas");
      }
  }

  void Renderer::cargarTextura(const std::string& ruta, const std::string& nombre) {
      auto textura = std::make_unique<Textura>();
      textura->cargar(ruta);
      texturas[nombre] = std::move(textura);
  }

  void Renderer::asignarTexturaAModeloSeleccionado(const std::string& nombreTextura) {
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
      auto it = texturas.find(nombreTextura);
      if (it != texturas.end()) {
          Textura* texPtr = it->second.get();
          modelos[indiceModeloSeleccionado]->getMaterial().setTextura(texPtr);
          texturaActual = texPtr;
      }
  }

  void Renderer::inicializarSubrutinasColorBase() {
      glUseProgram(idSP);
      indiceColorMaterial = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "colorDeMaterial");
      indiceColorTextura = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, "colorDeTextura");

      if (indiceColorMaterial == GL_INVALID_INDEX || indiceColorTextura == GL_INVALID_INDEX) {
          throw std::runtime_error("Error: implementaciones de subrutinas de color no encontradas");
      }
  }

  PAG::Material& Renderer::getModeloSeleccionadoMaterial() {
      static PAG::Material vacio;
      if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return vacio;
      return modelos[indiceModeloSeleccionado]->getMaterial();
  }

  void Renderer::crearFBOSombras() {
      if (_fboSombras != 0) return;
      glGenFramebuffers(1, &_fboSombras);
  }

  void Renderer::crearTexturaSombraParaLuz(Luz* luz) {
      if (_texturasSombras.find(luz) != _texturasSombras.end()) return; // Ya existe

      GLuint texId = 0;
      glGenTextures(1, &texId);
      glBindTexture(GL_TEXTURE_2D, texId);

      // Crear textura de profundidad
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,_anchoShadowMap, _altoShadowMap, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

      // Parámetros de la textura
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

      GLfloat borde[] = {1.0f, 1.0f, 1.0f, 1.0f};
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borde);

      // Modo de comparación para shadow mapping
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

      glBindTexture(GL_TEXTURE_2D, 0);

      _texturasSombras[luz] = texId;
  }

  void Renderer::calcularMatricesLuz() {
      for (auto& luz : luces) {
          if (luz->getTipo() == TipoLuz::Direccional) {
              // Luz direccional: proyección ortográfica
              glm::vec3 direccion = glm::normalize(luz->getDireccion());

              // Colocar la cámara alejada en la dirección opuesta a la luz
              glm::vec3 posicionCamara = glm::vec3(5.0f) * (-direccion);

              _matricesVisionLuz[luz.get()] = glm::lookAt(
                      posicionCamara,
                      glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f)
                      );

              _matricesProyeccionLuz[luz.get()] = glm::ortho(
                      -5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f
                      );

          } else if (luz->getTipo() == TipoLuz::Foco) {
              // Foco: proyección en perspectiva
              glm::vec3 posicion = luz->getPosicion();
              glm::vec3 direccion = glm::normalize(luz->getDireccion());

              _matricesVisionLuz[luz.get()] = glm::lookAt(
                      posicion,
                      posicion + direccion,
                      glm::vec3(0.0f, 1.0f, 0.0f));

              float anguloApertura = luz->getAnguloApertura();
              _matricesProyeccionLuz[luz.get()] = glm::perspective(
                      2.0f * anguloApertura,
                      static_cast<float>(_anchoShadowMap) / static_cast<float>(_altoShadowMap),
                      0.1f, 20.0f);
          }

          // Crear la textura de sombra para esta luz si es compatible
          if (luz->getTipo() == TipoLuz::Direccional || luz->getTipo() == TipoLuz::Foco) {
              crearTexturaSombraParaLuz(luz.get());
          }
      }
  }

  void Renderer::renderizarShadowMaps() {
      if (!_usarSombras || idSPSombras == 0) return;

      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);

      glBindFramebuffer(GL_FRAMEBUFFER, _fboSombras);
      glViewport(0, 0, _anchoShadowMap, _altoShadowMap);

      glClear(GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);

      glUseProgram(idSPSombras);

      for (auto& luz : luces) {
          if (luz->getTipo() != TipoLuz::Direccional && luz->getTipo() != TipoLuz::Foco) {
              continue;
          }

          auto it = _texturasSombras.find(luz.get());
          if (it == _texturasSombras.end()) {
              std::cerr << "Error: No hay texturaColor de sombra para esta luz" << std::endl;
              continue;
          }

          GLuint texId = it->second;

          glBindFramebuffer(GL_FRAMEBUFFER, _fboSombras);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);

          glReadBuffer(GL_NONE);
          glDrawBuffer(GL_NONE);

          GLenum estado = glCheckFramebufferStatus(GL_FRAMEBUFFER);
          if (estado != GL_FRAMEBUFFER_COMPLETE) {
              std::cerr << "Error: FBO no completo" << std::endl;
              continue;
          }

          glClear(GL_DEPTH_BUFFER_BIT);

          glm::mat4 matrizVision = _matricesVisionLuz[luz.get()];
          glm::mat4 matrizProyeccion = _matricesProyeccionLuz[luz.get()];
          glm::mat4 matrizVP = matrizProyeccion * matrizVision;

          GLint locMatriz = glGetUniformLocation(idSPSombras, "matrizModVisProy");

          for (const auto& modelo : modelos) {
              glm::mat4 matrizModelo = modelo->getTransform();
              glm::mat4 matrizMVP = matrizVP * matrizModelo;

              glUniformMatrix4fv(locMatriz, 1, GL_FALSE, &matrizMVP[0][0]);

              glBindVertexArray(modelo->getVAO()); // Necesitas un getter getVAO() en Modelo
              glDrawElements(GL_TRIANGLES, (int)modelo->getNumIndices(), GL_UNSIGNED_INT, 0);
              glBindVertexArray(0);
          }
      }

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

      glDisable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glDepthFunc(GL_LEQUAL);
  }

  void Renderer::creaShaderProgramSombras() {
      if (idSPSombras != 0) return;
      Shader::creaShaderProgram("shadow", idVSSombras, idFSSombras, idSPSombras);
  }

    void Renderer::asignarNormalMapAModeloSeleccionado(const std::string& nombreTextura) {
        if (indiceModeloSeleccionado < 0 || indiceModeloSeleccionado >= (int)modelos.size()) return;
        auto it = texturas.find(nombreTextura);
        if (it != texturas.end()) {
            modelos[indiceModeloSeleccionado]->getMaterial().setTexturaNormales(it->second.get());
            std::cout << "[INFO] Normal map asignado: " << nombreTextura << "\n";
        }
    }

    glm::mat4 Renderer::getTransformModeloSeleccionado() const {
        return modelos[indiceModeloSeleccionado]->getTransform();
    }

} // PAG
