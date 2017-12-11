#ifndef _OBJ_H
#define _OBJ_H

#include <cstdlib>
#include <cstdio>

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
	glm::mat4	 model; // store the model transformations
};

// computa as normais de um objeto caso elas não existam
void ComputeNormals(ObjModel* model);
void PrintObjModelInfo(ObjModel*); // Função para debugging
void BuildTrianglesAndAddToVirtualScene(ObjModel*, std::map<std::string, SceneObject> &virtualScene); // Constrói representação de um ObjModel como malha de triângulos para renderização
void DrawVirtualObject(SceneObject obj); // Desenha um objeto armazenado em g_VirtualScene
// detecta a colisão entre dois bounding boxes
bool DetectBboxCollision(SceneObject* obj1, SceneObject* obj2);
// detecta a colisão entre ponto e bounding box
bool DetectPointBboxCollision(glm::vec4 pt, SceneObject* obj);


#endif // _OBJ_H
