#ifndef _SHADER_VARS_H
#define _SHADER_VARS_H
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3

#include "utils.h"

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
extern GLuint vertex_shader_id;
extern GLuint fragment_shader_id;
extern GLuint program_id;
extern GLint model_uniform;
extern GLint view_uniform;
extern GLint projection_uniform;
extern GLint object_id_uniform;
extern GLint bbox_min_uniform;
extern GLint bbox_max_uniform;

void LoadShadersFromFiles();
#endif // _SHADER_VARS_H
