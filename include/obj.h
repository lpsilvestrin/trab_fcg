#ifndef _OBJ_H
#define _OBJ_H

#include <cstdlib>
#include <cstdio>

// OPENGL headers
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>
    
#include "matrices.h"
#include "shader_vars.h"

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
    std::string  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

// computa as normais de um objeto caso elas não existam
void ComputeNormals(ObjModel* model);
void PrintObjModelInfo(ObjModel*); // Função para debugging
void BuildTrianglesAndAddToVirtualScene(ObjModel*, std::map<std::string, SceneObject> &virtualScene); // Constrói representação de um ObjModel como malha de triângulos para renderização
void DrawVirtualObject(const char* object_name, std::map<std::string, SceneObject> &virtualScene); // Desenha um objeto armazenado em g_VirtualScene


#endif // _OBJ_H
