//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
// INF01047 Fundamentos de Computação Gráfica 2017/2
//               Prof. Eduardo Gastal
//
//    Trabalho Final - Fundamentos de Computação Gráfica
//    Authors: Luis Silvestrin (228528) e Rodrigo Okido (252745)
//

#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

//#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "obj.h"
#include "textrendering.h"
#include "matrices.h"
#include "shader_vars.h"



// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Funções de detalhes da partida
// Função para exibir o tempo de game
void TextRendering_ShowGameTime(GLFWwindow* window);

// Função para exibir a pontuação atual do jogador
void TextRendering_ShowGameScore(GLFWwindow* window);

// Função para exibir comandos de ajuda
void TextRendering_ShowGameHelpCommands(GLFWwindow* window);

// Função para exibir a mira do jogador
void TextRendering_ShowGameCrosshair(GLFWwindow* window);

// Função para exibir a tela de fim
void TextRendering_ShowGameEnd(GLFWwindow* window);

//Função para reiniciar o game
void restartGame();

// Funções abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informações do programa. Definidas após main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);
void TextRendering_PrintModelMat(GLFWwindow* window, glm::mat4 model);


// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Lista que armazena as vacas do jogo
std::list<GameObject> g_CowList;

// Lista que armazena esferas no jogo
std::list<GameObject> g_SphereList;

// Lista de bullets do jogo
std::list<GameObject> g_BulletList;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse



// Verificação dos teclados. Comandos para movimentação da camera
// W,S,A,D (Frente, Trás, Esquerda e Direita)
// O "keyPressed" indica se o usuário mantém pressionando a tecla correspondente.
// E o "keyReleased" indica se o usuário soltou a tecla correspondente
bool W_keyPressed = false;
bool A_keyPressed = false;
bool S_keyPressed = false;
bool D_keyPressed = false;
bool W_keyReleased = false;
bool A_keyReleased = false;
bool S_keyReleased = false;
bool D_keyReleased = false;

// Botao para reiniciar o game
bool ENTER_keyPressed = false;
bool ENTER_keyReleased = false;


// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;


// Variáveis de controle do tempo
#define GAME_TIME 60
float time_begin; //Tempo de inicio
float timer; //Controlador do tempo
float ACTUAL_TIME = GAME_TIME; //Tempo atual

// Variável da pontuação do jogador
unsigned int score = 0;
bool gameEnded = false;

// Variáveis do mapa
float g_map_size = 60.0f;
float g_map_floor = -1.1f;

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "INF01047 - Cows on the Run", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slide 217 e 219 do documento no Moodle
    // "Aula_03_Rendering_Pipeline_Grafico.pdf".
    //
    LoadShadersFromFiles();

    // Carregamos duas imagens para serem utilizadas como textura
    LoadTextureImage("../../data/tc-earth_daymap_surface.jpg");      // TextureImage0
    //LoadTextureImage("../../data/tc-earth_nightmap_citylights.gif");
    LoadTextureImage("../../data/cow_texture.jpg"); //TextureImage1
    LoadTextureImage("../../data/rabbit_texture.jpg"); //TextureImage2
    LoadTextureImage("../../data/terrain.jpg"); //TextureImage3
    LoadTextureImage("../../data/bullet_texture.png"); //TextureImage4



    // Construímos a representação de objetos geométricos através de malhas de triângulos
    ObjModel spheremodel("../../data/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel, g_VirtualScene);

    ObjModel bunnymodel("../../data/bunny.obj");
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel, g_VirtualScene);

    ObjModel planemodel("../../data/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel, g_VirtualScene);

    ObjModel cowmodel("../../data/cow.obj");
    ComputeNormals(&cowmodel);
    BuildTrianglesAndAddToVirtualScene(&cowmodel, g_VirtualScene);

    ObjModel bulletmodel("../../data/bullet.obj");
    ComputeNormals(&bulletmodel);
    BuildTrianglesAndAddToVirtualScene(&bulletmodel, g_VirtualScene);


    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model, g_VirtualScene);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 66 à 68 do documento "Aula_13_Clipping_and_Culling.pdf".
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 22 à 34 do documento "Aula_13_Clipping_and_Culling.pdf".
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Variáveis auxiliares utilizadas para chamada à função
    // TextRendering_ShowModelViewProjection(), armazenando matrizes 4x4.
    glm::mat4 the_projection;
    glm::mat4 the_model;
    glm::mat4 the_view;


    // Prepara posições iniciais da câmera
    glm::vec4 camera_position_c  = glm::vec4(1.0f,1.0f,1.0f,1.0f);
    glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    //Velocidade que a camera andará pelo cenário quando alguma tecla
    //de movimentação for usada (W,S,A,D).
    float camera_speed = 0.2f;

	// inicializa time begin e seta o timer para o ACTUAL_ATUAL
	time_begin = (float)glfwGetTime();
  timer = ACTUAL_TIME;


	bool create_more_cows = true; //flag para controlar a criação de vacas no cenário
  bool create_more_sphere = true; //flag para controlar a criação de esferas no cenário

  glm::vec4 mov; // vetor movimento
  glm::mat4 view;

    // Ficamos em loop, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(0.3f, 0.6f, 1.0f, 0.5f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(program_id);

        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        // Veja slide 165 do documento "Aula_08_Sistemas_de_Coordenadas.pdf".
        //glm::vec4 camera_position_c  = glm::vec4(x,y,z,1.0f); // Ponto "c", centro da câmera

        glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        glm::vec4 camera_view_vector; // Vetor "view", sentido para onde a câmera está virada

        if(timer > 0){
          camera_view_vector =  glm::vec4(x,y,z,0.0f);


        camera_position_c.y = 0.5f; // Para não movimentar o personagem para cima e para baixo livremente,
                                    // mantemos y da camera constante.

        //Preparação para realizar a movimentação livre pelo mapa (Free Camera)
        glm::vec4 w = -camera_view_vector;
        glm::vec4 u = crossproduct(camera_up_vector, w);

        // Normalizamos os vetores u e w
        w = w / norm(w);
        u = u / norm(u);

		    // vetor movimento
		    mov = glm::vec4(0.0,0.0,0.0,0.0);

        if(camera_position_c.x > g_map_size)
            camera_position_c.x -= camera_speed*2;
        else if(camera_position_c.x < -g_map_size)
            camera_position_c.x += camera_speed*2;
        else if(camera_position_c.z > g_map_size)
            camera_position_c.z -= camera_speed*2;
        else if(camera_position_c.z < -g_map_size)
            camera_position_c.z += camera_speed*2;
        else
        {
            if(W_keyPressed) // Atualiza posição da camera se for pra frente
                mov += camera_speed * (-w);

            if(S_keyPressed) // Atualiza posição da camera se for pra tras
                mov += camera_speed * w;

            if(A_keyPressed) // Atualiza posição da camera se for pra esquerda
                mov += camera_speed * (-u);

            if(D_keyPressed) // Atualiza posição da camera se for pra direita
                mov += camera_speed * u;
        }

      camera_position_c += mov;



      }else{
        camera_position_c  = glm::vec4(x,y,z,1.0f);
        camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada
      }

        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slide 169 do
        // documento "Aula_08_Sistemas_de_Coordenadas.pdf".
        view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 198-200 do documento
        // "Aula_09_Projecoes.pdf".
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -30.0f; // Posição do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Projeção Perspectiva.
            // Para definição do field of view (FOV), veja slide 234 do
            // documento "Aula_09_Projecoes.pdf".
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Projeção Ortográfica.
            // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // veja slide 243 do documento "Aula_09_Projecoes.pdf".
            // Para simular um "zoom" ortográfico, computamos o valor de "t"
            // utilizando a variável g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));


		/**
        // Desenhamos o modelo da esfera
		g_VirtualScene["sphere"].model =
        		Matrix_Translate(-8.0f,0.0f,6.0f)
              * Matrix_Rotate_Z(0.6f)
              * Matrix_Rotate_X(0.2f)
              * Matrix_Rotate_Y(g_AngleY + (float)glfwGetTime() * 0.1f);


        DrawVirtualObject(g_VirtualScene["sphere"]);

        // Desenhamos o modelo do coelho
		g_VirtualScene["bunny"].model =
        		Matrix_Translate((float)glfwGetTime()*0.3f,0.0f,0.0f);
              // * Matrix_Rotate_X(g_AngleX + (float)glfwGetTime() * 0.1f);

        DrawVirtualObject(g_VirtualScene["bunny"]);

		g_VirtualScene["bunny"].model =
        		Matrix_Translate((float)glfwGetTime()*(-0.3f),0.0f,0.0f);
              // * Matrix_Rotate_X(g_AngleX + (float)glfwGetTime() * 0.1f);

        DrawVirtualObject(g_VirtualScene["bunny"]);
        // Desenho do modelo da vaca
		g_VirtualScene["cow"].model =
       			Matrix_Translate(7.0f,0.0f,9.0f)
              * Matrix_Rotate_X(g_AngleX + (float)glfwGetTime() * 0.1f);

        DrawVirtualObject(g_VirtualScene["cow"]);
		**/


        // Desenhamos o plano do chão
		glm::mat4 model =
        		Matrix_Translate(0.0f,g_map_floor,0.0f)
                * Matrix_Scale(g_map_size,1.0f,g_map_size);
        DrawVirtualObject(g_VirtualScene["plane"], model);

		// cria nova vaca a cada 3 segundos
		if ((int)timer % 3 == 0 && create_more_cows) {
			GameObject cow = createRandomObj(g_VirtualScene["cow"], -g_map_size/2, g_map_size/2, -g_map_size/2, g_map_size/2);
			g_CowList.push_back(cow);
			// atualiza a variavel para garantir que só crie uma vaca por iteração
			create_more_cows = false;
		}

		if ((int)timer % 3 != 0) {
		  create_more_cows = true;
		}

    // cria nova esfera a cada 18 segundos
    if ((int)timer % 18 == 0 && create_more_sphere) {
      GameObject sphere = createRandomObj(g_VirtualScene["sphere"], -g_map_size/2, g_map_size/2, -g_map_size/2, g_map_size/2);
      g_SphereList.push_back(sphere);
      // atualiza a variavel para garantir que só crie uma esfera por iteração
      create_more_sphere = false;
    }

    if ((int)timer % 18 != 0) {
      create_more_sphere = true;
    }

		// create a bullet when mouse button is pressed
		if (g_LeftMouseButtonPressed) {
			GameObject bul = createBullet(g_VirtualScene["sphere"], camera_view_vector, camera_position_c);
			g_BulletList.push_back(bul);
		}

		// atualiza lista de bullets removendo aqueles fora do mapa
		removeObjOutsideScene(g_BulletList, g_map_size, g_map_size, g_map_floor, 50.0f);
		removeObjOutsideScene(g_CowList, g_map_size, g_map_size, g_map_floor, 50.0f);
		removeObjOutsideScene(g_SphereList, g_map_size, g_map_size, g_map_floor, 50.0f);
		// desenha objectos do jogo
		moveList(g_CowList);
	    moveList(g_SphereList);
		moveList(g_BulletList);
		drawList(g_BulletList, g_VirtualScene);
		drawList(g_CowList, g_VirtualScene);
    drawList(g_SphereList, g_VirtualScene);
		int n_collisions = detectBulletObjCollision(g_CowList, g_BulletList);
		if(n_collisions > 0)
    {
      if (!gameEnded)
      {
        score = score + 5*n_collisions;
      }
    }
	n_collisions = detectBulletObjCollision(g_SphereList, g_BulletList);
    if(n_collisions > 0){
      if(!gameEnded)
      {
        score = score + 15*n_collisions;
        ACTUAL_TIME = ACTUAL_TIME + 5;
      }
    }
        if(timer > 0)
          timer = ACTUAL_TIME - ((float)glfwGetTime() - time_begin);

        // Mostra o tempo da partida
        TextRendering_ShowGameTime(window);



         //testa colisao da camera com as vacas
        		if (detectCameraObjCollision(g_CowList, camera_position_c)) {
        			camera_position_c -= mov;
              ACTUAL_TIME = 1;
        	   }

            if (detectCameraObjCollision(g_SphereList, camera_position_c)) {
               camera_position_c -= mov;
            }


        // Condições para fim do jogo
        if(timer <= 0)
        {
          timer = 0; //Seta o tempo para 0
          //Ativa flag de fim de jogo
          gameEnded = true;

          //Muda camera para encerramento
          //g_CameraPhi = 1;

          camera_position_c  = glm::vec4(x,y,z,1.0f); // Ponto "c", centro da câmera
          camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
          camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada
          camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)

          // Mostra tela de fim de jogo
          TextRendering_ShowGameEnd(window);
        }


        // Após a tela de game over, caso o jogador aperte ENTER o jogo reinicia
        if(ENTER_keyPressed)
        {
            if(timer<=0){
              restartGame();
              camera_position_c  = glm::vec4(1.0f,1.0f,1.0f,1.0f);
              camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);
            }
        }

        // Mostra a pontuação atual do jogador
        TextRendering_ShowGameScore(window);

        // Mostra os comandos de ajuda
        TextRendering_ShowGameHelpCommands(window);

        // Exibe a mira do jogador na tela
        TextRendering_ShowGameCrosshair(window);

        // Pegamos um vértice com coordenadas de modelo (0.5, 0.5, 0.5, 1) e o
        // passamos por todos os sistemas de coordenadas armazenados nas
        // matrizes the_model, the_view, e the_projection; e escrevemos na tela
        // as matrizes e pontos resultantes dessas transformações.
        //glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);
        //TextRendering_ShowModelViewProjection(window, projection, view, model, p_model);

        // Imprimimos na tela os ângulos de Euler que controlam a rotação do
        // terceiro cubo.
        TextRendering_ShowEulerAngles(window);

        // Imprimimos na informação sobre a matriz de projeção sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informação sobre o número de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}


// Função de reinicio do game. Seta todas as variáveis para os valores
// como se fosse novamente o inicio de jogo.
void restartGame()
{
  gameEnded = false;
  ACTUAL_TIME = GAME_TIME;
  timer = ACTUAL_TIME;
  score = 0;
  time_begin = (float)glfwGetTime();
  g_CameraTheta = 0.0f;
  g_CameraPhi = 0.0f;
  g_CameraDistance = 3.5f;

}



// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}



// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slides 32 até 40
    // do documento "Aula_07_Transformacoes_Geometricas_3D.pdf").
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slide 234
    // do documento "Aula_09_Projecoes.pdf".
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Deslocamento do cursor do mouse em x e y de coordenadas de tela!
	float dx = xpos - g_LastCursorPosX;
	float dy = ypos - g_LastCursorPosY;

	// Atualizamos parâmetros da câmera com os deslocamentos
	g_CameraTheta -= 0.01f*dx;
  if(timer > 0){
	 g_CameraPhi   -= 0.01f*dy;
  }else{
   g_CameraPhi += 0.01f*dy;
}
  // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
	float phimax = 3.141592f/2;
	float phimin = -phimax;

	if (g_CameraPhi > phimax)
		g_CameraPhi = phimax;

	if (g_CameraPhi < phimin)
		g_CameraPhi = phimin;

	// Atualizamos as variáveis globais para armazenar a posição atual do
	// cursor como sendo a última posição conhecida do cursor.
	g_LastCursorPosX = xpos;
	g_LastCursorPosY = ypos;

}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi  pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
    }

    // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = true;
    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = false;
    }

    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    // Se o usuário apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }



    // *** Adicionando as funções do teclado para o movimento de câmera


    // Aciona flags para mover pra frente
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        W_keyPressed = true;
        W_keyReleased = false;
    }

    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        W_keyPressed = false;
        W_keyReleased = true;
    }

    // Aciona flags para mover pra tras
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        S_keyPressed = true;
        S_keyReleased = false;
    }

    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        S_keyPressed = false;
        S_keyReleased = true;
    }

    // Aciona flags para mover pra esquerda
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        A_keyPressed = true;
        A_keyReleased = false;
    }

    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        A_keyPressed = false;
        A_keyReleased = true;
    }

    // Aciona flags para mover pra direita
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        D_keyPressed = true;
        D_keyReleased = false;
    }

    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        D_keyPressed = false;
        D_keyReleased = true;
    }

    // Aciona flags para fazer o reinicio do jogo
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        ENTER_keyPressed = true;
        ENTER_keyReleased = false;
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
    {
        ENTER_keyPressed = false;
        ENTER_keyReleased = true;
    }

}


// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}


// Exibe na tela o tempo da partida do game
void TextRendering_ShowGameTime(GLFWwindow* window)
{
    //if(timer <= 0)
      //return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    char buffer[30];
    snprintf(buffer, 30, "TEMPO DE PARTIDA: %ds\n", (int)timer);


    TextRendering_PrintString(window, buffer, 1.0f-65*charwidth, 0.98f-lineheight, 1.5f);

}


// Exibe na tela a pontuação atual do jogador
void TextRendering_ShowGameScore(GLFWwindow* window)
{
    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    char buffer[15];
    snprintf(buffer, 15, "SCORE: %d\n", score);

    TextRendering_PrintString(window, buffer, -1.0f+charwidth, 0.98f-lineheight, 1.5f);
}

// Exibe na tela a pontuação atual do jogador
void TextRendering_ShowGameHelpCommands(GLFWwindow* window)
{
    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    char buffer[15];
    snprintf(buffer, 15, "[ESC] Quit\n");

    TextRendering_PrintString(window, buffer, -1.0f+charwidth, 0.90f-lineheight, 0.9f);
}


// Função que projeta a mira do jogador na tela
void TextRendering_ShowGameCrosshair(GLFWwindow* window)
{

  float lineheight = TextRendering_LineHeight(window);
  float charwidth = TextRendering_CharWidth(window);

  char showCrosshair[2];
  snprintf(showCrosshair, 2, "+");

  char hideCrosshair[2];
  snprintf(hideCrosshair, 2, " ");
  if (gameEnded){
    TextRendering_PrintString(window, hideCrosshair, -0.02f-(charwidth)/2, lineheight-0.08f, 2.5f);
  } else{
    TextRendering_PrintString(window, showCrosshair, -0.02f-(charwidth)/2, lineheight-0.08f, 2.5f);
  }
}


// Função que exibe quando o jogo termina
void TextRendering_ShowGameEnd(GLFWwindow* window){

  float lineheight = TextRendering_LineHeight(window);
  float charwidth = TextRendering_CharWidth(window);

  char message1[60];
  snprintf(message1, 60, "GAME OVER - Final Score: %d\n", score);

  TextRendering_PrintString(window, message1, -0.4f-(charwidth)/2, lineheight, 1.5f);

  char message2[35];
  snprintf(message2, 35, "Thanks for playing!\n");
  TextRendering_PrintString(window, message2, -0.25f-(charwidth)/2, lineheight-0.1, 1.2f);

  char message3[35];
  snprintf(message3, 35, "Press ENTER to try again \n");
  TextRendering_PrintString(window, message3, -0.31f-(charwidth)/2, lineheight-0.2, 1.2f);
}


// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     World", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     Camera", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                   NDC", -1.0f, 1.0f-13*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-14*pad, 1.0f);
}

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    //float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    //TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de projeção está sendo utilizada. (perspective - if / ortographic - else)
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, " ", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, " ", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

void TextRendering_PrintModelMat(GLFWwindow* window, glm::mat4 model)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);

float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintMatrix(window, model, 1.0f-(20 + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
