#version 330 core

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da cor de cada vértice, definidas em "shader_vertex.glsl" e
// "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;
// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec3 color;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);



    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
	// sentido da fonte de luz é o mesmo da câmera
	vec4 l = vec4(v[0],v[1],v[2],v[3]);


    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2*n*dot(n,l); // PREENCHA AQUI o vetor de reflexão especular ideal
    
// Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong
    vec3 Kd0 = vec3(0.0,0.0,0.0);

    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI
        // Propriedades espectrais da esfera
        Kd = vec3(0.0,0.0,0.8);
        Ks = vec3(0.0,0.0,0.8);
        Ka = vec3(0.0,0.0,0.2);
        q = 1.0;
        
		vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        float len   = length(position_model - bbox_center);
        float theta = atan(position_model[0], position_model[2]);
        float phi   = asin(position_model[1]/len);

        U = (theta + M_PI)/(2*M_PI);
        V = (phi + M_PI_2)/M_PI;
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }
    else if ( object_id == BUNNY )
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.08,0.4,0.8);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.04,0.2,0.4);
        q = 32.0;
    }
    else if ( object_id == PLANE )
    {
		U = texcoords.x;
		V = texcoords.y;

        // PREENCHA AQUI
        // Propriedades espectrais do plano
        Kd = vec3(0.2,0.2,0.2);
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.0,0.0,0.0);
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
        q = 20.0;
    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0); // PREENCH AQUI o espectro da fonte de luz

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2); // PREENCHA AQUI o espectro da luz ambiente

	
    // Termo ambiente
    vec3 ambient_term = vec3(Ka[0]*Ia[0],Ka[1]*Ia[1],Ka[2]*Ia[2]); // PREENCHA AQUI o termo ambiente
		
	// Termo especular utilizando o modelo de iluminação de Phong
	vec3 phong_specular_term = pow(max(0,dot(r,v)),q) * vec3(Ks[0]*I[0],Ks[1]*I[1],Ks[2]*I[2]); // PREENCH AQUI o termo especular de Phong
		
	// Termo difuso utilizando a lei dos cossenos de Lambert
	vec3 lambert_diffuse_term = max(0,dot(n,l))*vec3(Kd[0]*I[0],Kd[1]*I[1],Kd[2]*I[2]); // PREENCHA AQUI o termo difuso de Lambert


    // Cor final utilizando textura
	color = Kd0 + lambert_diffuse_term + ambient_term + phong_specular_term;
    
// Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color = pow(color, vec3(1.0,1.0,1.0)/2.2);
} 
