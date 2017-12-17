#ifndef _OBJ_H
#define _OBJ_H

#include <cstdlib>
#include <cstdio>
#include <list>
#include <math.h>

// OPENGL headers
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3

#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include "matrices.h"
#include "shader_vars.h"


extern std::map<std::string, int> shader_ids;


// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando modelo \"%s\"... ", filename);

        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        printf("OK.\n");
    }
};


// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
	int shader_id;			// id usado para o rasterizador
    std::string  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;

};

// objeto que armazena as informações de um objeto interativo do jogo
struct GameObject
{
	std::string  name; // relaciona o game object com seu scene object
	glm::mat4	 model; // store the model transformations
	glm::mat4    rotate; // store the rotation of the object
	glm::vec4	 dir; // direction of movement
	float		 speed;
	bool		 toDraw; // define if the object should be drawn or not
	glm::vec3 bbox_min;
	glm::vec3 bbox_max;
	int			counter; // count movements made by the object
	};

// computa as normais de um objeto caso elas não existam
void ComputeNormals(ObjModel* model);
void PrintObjModelInfo(ObjModel*); // Função para debugging
void BuildTrianglesAndAddToVirtualScene(ObjModel*, std::map<std::string, SceneObject> &virtualScene); // Constrói representação de um ObjModel como malha de triângulos para renderização
void DrawVirtualObject(SceneObject obj, glm::mat4 model); // Desenha um objeto armazenado em g_VirtualScene
// detecta a colisão entre dois bounding boxes
bool DetectBboxCollision(GameObject* obj1, GameObject* obj2);
// detecta a colisão entre ponto e bounding box
bool DetectPointBboxCollision(glm::vec4 pt, GameObject obj);

GameObject createRandomObj(SceneObject objModel, int minX, int maxX, int minZ, int maxZ);
void drawList(std::list<GameObject> goList, std::map<std::string, SceneObject> &virtualScene);

void moveList(std::list<GameObject>& goList);

GameObject createBullet(SceneObject bulletModel, glm::vec4 dir, glm::vec4 position);
int detectBulletObjCollision(std::list<GameObject>& objList, std::list<GameObject>& bulList);
bool detectCameraObjCollision(std::list<GameObject> goList, glm::vec4 c_pos);
float pointPlaneDistance(glm::vec4 obj_center, glm::vec4 normal, glm::vec4 pt);
bool detectObjOutsideScene(GameObject o, float maxX, float maxZ, float minY, float maxY);
void removeObjOutsideScene(std::list<GameObject> &goList, float maxX, float maxZ, float minY, float maxY);

#endif // _OBJ_H
