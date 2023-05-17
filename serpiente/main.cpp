#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <lecturaShader.h>
#include <string>
#include <unordered_map>


#include "geometrias.h"
#include "serpiente.h"
#include "fruta.h"

//transformaciones
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//para la carga de texturas
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//ancho y alto de la ventana
GLuint ANCHO = 1200;
GLuint ALTO = 800;

unsigned int comenzar = 0;
unsigned int perder = 0;
GLuint shaderProgram;

//VAOS para crear los objetos
GLuint VAOCuadrado;
GLuint VAOCubo;
GLuint VAOEsfera;

Serpiente serpiente(3, &VAOCubo, 36, &VAOEsfera, 1080);

GLuint texturaSerpiente1, texturaSerpiente2, hierba1, hierba2, texturaOjo, texturaPerder;
std::unordered_map<unsigned int, GLuint> texturasFruta;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern GLuint setShaders(const char* nVertix, const char* nFrag);

void camaraAlejada() {
	glViewport(0, 0, ANCHO, ALTO);
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//establecemos la posicion del observador
	view = glm::lookAt(glm::vec3(0, 0, 20 * ESCALA), glm::vec3(.0f, .0f, .0f), glm::vec3(.0f,1, .0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f,(float) ANCHO/ (float) ALTO, 0.01f, 21.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void camaraCabeza() {
	glViewport(0, 0, ANCHO, ALTO);
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//establecemos la posicion del observador
	view = glm::lookAt(glm::vec3(serpiente.getCuerpo()[1].getX()-1, serpiente.getCuerpo()[1].getY()-1, 1), glm::vec3(serpiente.getCabeza().getX(), serpiente.getCabeza().getY(), 1), glm::vec3(.0f, 0, 1.0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f, (float)ANCHO / (float)ALTO, 0.01f, 10.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void iluminacion(Fruta comida) {
	//el color de la luz ambiente 
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	//luz blanca
	glUniform3f(lightLoc, 1.0f, 1.0f, 1.0f);
	//luz difusa 
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, serpiente.getCabeza().getX(), serpiente.getCabeza().getY(), serpiente.getCabeza().getZ() + 5);

	unsigned int luzDirLoc = glGetUniformLocation(shaderProgram, "luzDir");
	glUniform3f(luzDirLoc, comida.getX()-serpiente.getCabeza().getX(), comida.getY()-serpiente.getCabeza().getY(), 1);
	//luz especular 
	//unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	//glUniform3f(viewPosLoc, posicionObservador.getX(), posicionObservador.getY(), posicionObservador.getZ());
}


//funcion que crea el VAO en la variable VAO a partir del array de vertices
void creaVAO(float* vertices, unsigned int tam, GLuint* VAO, int color) {
	unsigned int VBO, repeticion = 8;
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, tam, vertices, GL_STATIC_DRAW);
	//comprobamos si el array de vertices incluye el color a parte (en el caso de la esfera se usan las normales)
	if (color) {
		//Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(0);
		//Normales
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		//Textura
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		//Color
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);		
	}else {
		//Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(0);
		//Normales
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		//Textura
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		//color
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(3);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}

//funcion de dibujo del suelo
void dibujaSuelo(GLuint shaderProgram) {
	//creamos las matrices del modelo
	glm::mat4 model;
	//la busco en el shader
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	int i, j;
	for (i = -LIMITE; i <= LIMITE; i++) {
		for (j = -LIMITE; j <= LIMITE; j++) {
			//Calculo la matriz
			model = glm::mat4(); //identity matrix
			//trasladamos para dibujar cada cuadrado
			model = glm::translate(model, glm::vec3(i*ESCALA, j*ESCALA, 0.0f));
			model = glm::scale(model, glm::vec3(ESCALA, ESCALA, 1));
			if ((i + j) % 2) {
				glBindTexture(GL_TEXTURE_2D, hierba1);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, hierba2);
			}
			
			//La cargo
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//dibujamos el cuadrado
			glBindVertexArray(VAOCuadrado);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}


void cargaTextura(unsigned int* textura, const char* ruta) {
	int width, height, nrChannels, formato;
	//geneneramos la textura
	glGenTextures(1, textura);
	glBindTexture(GL_TEXTURE_2D, *textura);
	//parametros wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//parametros de escala
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	//cargamos la imagen
	unsigned char* imagen = stbi_load(ruta, &width, &height, &nrChannels, 0);
	if (nrChannels == 1) {
		formato = GL_RED;
	}else if (nrChannels == 3) {
		formato = GL_RGB;
	} else{
		formato = GL_RGBA;
	}
	if (imagen) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, formato, width, height, 0, formato, GL_UNSIGNED_BYTE, imagen);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		printf("Fallo en la carga de la textura %s\n", imagen);
	}
	stbi_image_free(imagen);
}

void dibujarFin() {
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	//matriz de transformacion
	glm::mat4 model = glm::mat4();
	model = glm::scale(model, glm::vec3(30,20, 1));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, texturaPerder);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void openGlInit() {
	glClearDepth(1.0f); //Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f); //valor limpieza buffer color
	glEnable(GL_DEPTH_TEST); //z-buffer
	//glEnable(GL_CULL_FACE); //ocultacion caras back
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

int main() {
	
	//glfw: initalize and configure
	//-----------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creo la ventana
	GLFWwindow* window = glfwCreateWindow(ANCHO, ALTO, "Snake", NULL, NULL);
	if (window == NULL)	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetKeyCallback(window, keyCallback);
	openGlInit();

	//generarShader();
	shaderProgram = setShaders("shader.vert", "shader.frag");
	glUseProgram(shaderProgram);

	//creamos los VAOS
	creaVAO(vertices_cuadrado, sizeof(vertices_cuadrado), &VAOCuadrado, 1);
	creaVAO(vertices_esfera, sizeof(vertices_esfera), &VAOEsfera, 0);
	creaVAO(vertices_cubo, sizeof(vertices_cubo), &VAOCubo, 1);

	//creamos las texturas
	cargaTextura(&texturaSerpiente1,"../texturas/serpiente_marron.jpg");
	cargaTextura(&texturaSerpiente2, "../texturas/serpiente_amarilla.jpg");
	//texturas de las frutas
	GLuint texturaFruta;
	cargaTextura(&texturaFruta, "../texturas/piña.jpg");
	texturasFruta.insert({ 0, texturaFruta });
	cargaTextura(&texturaFruta, "../texturas/sandia.jpg");
	texturasFruta.insert({ 1, texturaFruta });
	cargaTextura(&texturaFruta, "../texturas/naranja.jpg");
	texturasFruta.insert({ 2, texturaFruta });
	cargaTextura(&texturaFruta, "../texturas/melocoton.jpg");
	texturasFruta.insert({ 3, texturaFruta });
	cargaTextura(&texturaFruta, "../texturas/coco.jpg");
	texturasFruta.insert({ 4, texturaFruta });
	cargaTextura(&texturaFruta, "../texturas/manzana.jpg");
	texturasFruta.insert({ 5, texturaFruta });
	cargaTextura(&hierba1, "../texturas/hierba1.jpg");
	cargaTextura(&hierba2, "../texturas/hierba2.jpg");
	cargaTextura(&texturaOjo, "../texturas/ojo.jpg");
	cargaTextura(&texturaPerder, "../texturas/perder.jpg");
	serpiente.texturizar(texturaSerpiente1, texturaSerpiente2, texturaOjo);
	// Lazo de la ventana mientras no la cierre
	// -----------
	Fruta comida(ESCALA/2.0, 0, &VAOEsfera, texturasFruta, 1080, serpiente);
	while (!glfwWindowShouldClose(window)){
		// input
		// -----
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Borro el buffer de la ventana
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		camaraAlejada();
		if (!perder) {
			//Dibujo del suelo
			dibujaSuelo(shaderProgram);

			serpiente.dibujar(shaderProgram);
			comida.dibujar(shaderProgram);
			if (comenzar) {
				if (!serpiente.avanzar(&comida)) {
					perder++;
					comenzar--;
				}
			}
		}
		else {
			dibujarFin();
		}
		
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAOCubo);
	glDeleteVertexArrays(1, &VAOCuadrado);
	glDeleteVertexArrays(1, &VAOEsfera);

	// glfw: terminate, clearing all 
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//funcion de teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key){
		//movimiento de la serpiente
		case 265://flecha arriba
			if (serpiente.getDireccion() != ABAJO && action == GLFW_RELEASE) {
				if (serpiente.getDireccion() == IZQUIERDA) {
					serpiente.girar(-90);
				}
				else {
					serpiente.girar(90);
				}
				serpiente.setDireccion(ARRIBA);
			}
			break;
		case 264://flecha abajo
			if (serpiente.getDireccion() != ARRIBA && action == GLFW_RELEASE) {
				if (serpiente.getDireccion() == IZQUIERDA) {
					serpiente.girar(90);
				}
				else {
					serpiente.girar(-90);
				}
				serpiente.setDireccion(ABAJO);
			}
			break;
		case 262://flecha derecha
			if (serpiente.getDireccion() != IZQUIERDA && action == GLFW_RELEASE) {
				if (serpiente.getDireccion() == ARRIBA) {
					serpiente.girar(-90);
				}
				else {
					serpiente.girar(90);
				}
				serpiente.setDireccion(DERECHA);
			}
			break;
		case 263://flecha izquierda
			if (serpiente.getDireccion() != DERECHA && action == GLFW_RELEASE) {
				if (serpiente.getDireccion() == ARRIBA) {
					serpiente.girar(90);
				}
				else {
					serpiente.girar(-90);
				}
				serpiente.setDireccion(IZQUIERDA);
			}
		break;
		//dandole al espacio se empieza el juego
		case 32:
			comenzar = 1;
		default:
			break;
			
	}
}

//funcion de reescalado
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	ANCHO = width;
	ALTO = height;
}
