#ifndef _MATRICES_H
#define _MATRICES_H

#include <cstdio>
#include <cstdlib>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Matrix(float, float, float, float,
				float,float,float,float,
				float,float,float,float,
				float,float,float,float);

glm::mat4 Matrix_Identity();
glm::mat4 Matrix_Translate(float, float, float);

glm::mat4 Matrix_Scale(float sx, float sy, float sz);
glm::mat4 Matrix_Rotate_X(float angle);
glm::mat4 Matrix_Rotate_Y(float angle);
glm::mat4 Matrix_Rotate_Z(float angle);
float norm(glm::vec4 v);
glm::mat4 Matrix_Rotate(float angle, glm::vec4 axis);
glm::vec4 crossproduct(glm::vec4 u, glm::vec4 v);
float dotproduct(glm::vec4 u, glm::vec4 v);
glm::mat4 Matrix_Camera_View(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector);
glm::mat4 Matrix_Orthographic(float l, float r, float b, float t, float n, float f);
glm::mat4 Matrix_Perspective(float field_of_view, float aspect, float n, float f);
glm::vec4 vec3_to_point(glm::vec3); // convert vec3 object to homogeneous coordinates point
void PrintMatrix(glm::mat4 M);
void PrintVector(glm::vec4 v);
void PrintMatrixVectorProduct(glm::mat4 M, glm::vec4 v);
void PrintMatrixVectorProductDivW(glm::mat4 M, glm::vec4 v);


#endif // _MATRICES_H
// vim: set spell spelllang=pt_br :
