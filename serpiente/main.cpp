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
#include "suelo.h"

//transformaciones
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//para la carga de texturas
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//ancho y alto de la ventana
GLuint ANCHO = 1600;
GLuint ALTO = 800;

GLuint comenzar = 0;
GLuint perder = 0;
GLuint shaderJuego, shaderTitulos;
GLuint camara = 0;

//VAOS para crear los objetos
GLuint VAOCuadrado;
GLuint VAOCubo;
GLuint VAOEsfera;

Serpiente serpiente(3, &VAOCubo, 36, &VAOEsfera, 1080);

GLuint texturaSerpiente1, texturaSerpiente2, hierba1, hierba2, texturaOjo, texturaPerder, puntos[10], texturaPuntos;
std::unordered_map<unsigned int, GLuint> texturasFruta;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern GLuint setShaders(const char* nVertix, const char* nFrag);

void camaraAlejada() {
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//establecemos la posicion del observador
	view = glm::lookAt(glm::vec3(0, 0, 20 * ESCALA), glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, 1, .0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderJuego, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f, (float) ANCHO/2/ (float) ALTO, 0.01f, 21.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderJuego, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void camaraCabeza() {
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//establecemos la posicion del observador
	glm::vec3 observador = glm::vec3(serpiente.getCabeza().getX() - 8*glm::cos(glm::radians((float)serpiente.getCabeza().getGiro())), serpiente.getCabeza().getY() - 8*glm::sin(glm::radians((float)serpiente.getCabeza().getGiro())), 8);
	glm::vec3 vision = glm::vec3(serpiente.getCabeza().getX() + glm::cos(glm::radians((float)serpiente.getCabeza().getGiro())), serpiente.getCabeza().getY() + glm::sin(glm::radians((float)serpiente.getCabeza().getGiro())), 0);
	view = glm::lookAt(observador,vision, glm::vec3(.0f, 0, 1.0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderJuego, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f, (float)ANCHO/2 / (float)ALTO, 0.01f, 30.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderJuego, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void iluminacion(Fruta comida) {
	//el color de la luz ambiente 
	unsigned int lightLoc = glGetUniformLocation(shaderJuego, "colorLuz");
	//luz blanca
	//glUniform3f(lightLoc, 0.3f, 0.3f, 1.0f);
	glUniform3f(lightLoc, 1.0f, 1.0f, 1.0f);
	//luz difusa en la cabeza 
	unsigned int lightPosLoc = glGetUniformLocation(shaderJuego, "posicionLuzSerpiente");
	glUniform3f(lightPosLoc, serpiente.getCabeza().getX(), serpiente.getCabeza().getY(), 0.5);
	unsigned int luzDirLoc = glGetUniformLocation(shaderJuego, "direccionLuzSerpiente");
	glUniform3f(luzDirLoc, glm::cos(glm::radians((float)serpiente.getCabeza().getGiro())), glm::sin(glm::radians((float)serpiente.getCabeza().getGiro())), 0);
	//luz difusa en la fruta
	lightPosLoc = glGetUniformLocation(shaderJuego, "posicionLuzFruta");
	glUniform3f(lightPosLoc, comida.getX(), comida.getY(), 8);
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

GLuint cargaTextura(const char* ruta) {
	GLuint textura;
	int width, height, nrChannels, formato;
	//geneneramos la textura
	glGenTextures(1, &textura);
	glBindTexture(GL_TEXTURE_2D, textura);
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
	return textura;
}

void dibujarFin() {
	unsigned int modelLoc = glGetUniformLocation(shaderJuego, "model");
	//matriz de transformacion
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(20,20, 1));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, texturaPerder);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void dibujarPuntos(GLuint shader) {
	//viewport de la izquierda
	glViewport(0, 0, 800, 800);
	unsigned int modelLoc = glGetUniformLocation(shader, "model");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glm::mat4 model;
	//dibujamos el titulo snake
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, 9, 0));
	model = glm::scale(model, glm::vec3(9, 2, 1));
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//dibujamos el titulo puntos
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, 3, 0));
	model = glm::scale(model, glm::vec3(10, 3, 1));
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glBindTexture(GL_TEXTURE_2D, texturaPuntos);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//dibujamos los puntos
	for (int i = -3; i <= 3; i+=3) {
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, glm::vec3(i, -2, 0));
		model = glm::scale(model, glm::vec3(3, 4, 1));
		//La cargo
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOCuadrado);
		//puntos
		if (i == -3) {
			glBindTexture(GL_TEXTURE_2D, puntos[serpiente.getPuntos() / 100]);
		}
		else if (i == 0) {
			glBindTexture(GL_TEXTURE_2D, puntos[serpiente.getPuntos() / 10]);
		}
		else if (i == 3) {
			glBindTexture(GL_TEXTURE_2D, puntos[serpiente.getPuntos() % 10]);
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
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
	shaderJuego = setShaders("shader.vert", "shaderJuego.frag");
	shaderTitulos = setShaders("shader.vert", "shaderTitulos.frag");

	//creamos los VAOS
	creaVAO(vertices_cuadrado, sizeof(vertices_cuadrado), &VAOCuadrado, 1);
	creaVAO(vertices_esfera, sizeof(vertices_esfera), &VAOEsfera, 0);
	creaVAO(vertices_cubo, sizeof(vertices_cubo), &VAOCubo, 1);

	//creamos las texturas
	texturaSerpiente1 = cargaTextura("../texturas/serpiente_marron.jpg");
	texturaSerpiente2 = cargaTextura("../texturas/serpiente_amarilla.jpg");
	//texturas de las frutas
	GLuint texturaFruta;
	texturaFruta = cargaTextura("../texturas/piña.jpg");
	texturasFruta.insert({ 0, texturaFruta });
	texturaFruta = cargaTextura("../texturas/sandia.jpg");
	texturasFruta.insert({ 1, texturaFruta });
	texturaFruta = cargaTextura("../texturas/naranja.jpg");
	texturasFruta.insert({ 2, texturaFruta });
	texturaFruta = cargaTextura("../texturas/melocoton.jpg");
	texturasFruta.insert({ 3, texturaFruta });
	texturaFruta = cargaTextura("../texturas/coco.jpg");
	texturasFruta.insert({ 4, texturaFruta });
	texturaFruta = cargaTextura("../texturas/manzana.jpg");
	texturasFruta.insert({ 5, texturaFruta });
	hierba1 = cargaTextura("../texturas/hierba1.jpg");
	hierba2 = cargaTextura("../texturas/hierba2.jpg");
	texturaOjo = cargaTextura("../texturas/ojo.jpg");
	texturaPerder = cargaTextura("../texturas/perder.png");
	texturaPuntos = cargaTextura("../texturas/puntos.png");
	serpiente.texturizar(texturaSerpiente1, texturaSerpiente2, texturaOjo);
	//texturas de los puntos
	std::string ruta;
	for (int i = 0; i <= 9; i++) {
		ruta = "../texturas/" + std::to_string(i) + ".png";
		puntos[i] = cargaTextura(ruta.c_str());
	}
	// Lazo de la ventana mientras no la cierre
	// -----------
	Fruta comida(ESCALA/2.0, 0, &VAOEsfera, texturasFruta, 1080, serpiente);
	Suelo suelo(VAOCuadrado, hierba1, hierba2);
	glUseProgram(shaderJuego);
	glUseProgram(shaderTitulos);
	while (!glfwWindowShouldClose(window)){
		// input
		// -----
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Borro el buffer de la ventana
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		switch (camara)	{
			case 0:
				camaraAlejada();
				break;
			case 1: 
				camaraCabeza();
				break;
			default:
				break;
		}
		iluminacion(comida);
		if (!perder) {
			glUseProgram(shaderTitulos);
			dibujarPuntos(shaderTitulos);
			glUseProgram(shaderJuego);
			glViewport(ANCHO/2.1, 0, ANCHO/2, ALTO);
			//Dibujo del suelo
			suelo.dibujar(shaderJuego);
			serpiente.dibujar(shaderJuego);
			comida.dibujar(shaderJuego);
			if (comenzar) {
				if (!serpiente.avanzar(&comida)) {
					perder = ~perder;
					comenzar = ~comenzar;
				}
			}
		}else {
			glUseProgram(shaderTitulos);
			dibujarPuntos(shaderTitulos);
			glUseProgram(shaderJuego);
			glViewport(ANCHO / 2.1, 0, ANCHO / 2, ALTO);
			//Dibujo del suelo
			suelo.dibujar(shaderJuego);
			glUseProgram(shaderTitulos);
			camaraAlejada();
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
			if (camara == 0) {
				if (comenzar && action == GLFW_RELEASE) {
					if (serpiente.getDireccion() != ARRIBA && serpiente.getDireccion() != ABAJO) {
						if (serpiente.getDireccion() == IZQUIERDA) {
							serpiente.girar(-90);
						}
						else {
							serpiente.girar(90);
						}
						serpiente.setDireccion(ARRIBA);
					}
				}
			}
			break;
		case 264://flecha abajo
			if (camara == 0) {
				if (comenzar && action == GLFW_RELEASE) {
					if (serpiente.getDireccion() != ABAJO && serpiente.getDireccion() != ARRIBA) {
						if (serpiente.getDireccion() == IZQUIERDA) {
							serpiente.girar(90);
						}
						else {
							serpiente.girar(-90);
						}
						serpiente.setDireccion(ABAJO);
					}
				}
			}			
			break;
		case 262://flecha derecha
			if (camara == 0) {
				if (comenzar && action == GLFW_RELEASE) {
					if (serpiente.getDireccion() != DERECHA && serpiente.getDireccion() != IZQUIERDA) {
						if (serpiente.getDireccion() == ARRIBA) {
							serpiente.girar(-90);
						}
						else {
							serpiente.girar(90);
						}
						serpiente.setDireccion(DERECHA);
					}
				}
			}else {
				if (comenzar && action == GLFW_RELEASE) {
					serpiente.girar(-90);
					switch (serpiente.getDireccion()) {
						case ARRIBA:
							serpiente.setDireccion(DERECHA);
							break;
						case ABAJO:
							serpiente.setDireccion(IZQUIERDA);
							break;
						case IZQUIERDA:
							serpiente.setDireccion(ARRIBA);
							break;
						case DERECHA:
							serpiente.setDireccion(ABAJO);
							break;
					}
				}
			}
			
			break;
		case 263://flecha izquierda
			if (camara == 0) {
				if (comenzar && action == GLFW_RELEASE) {
					if (serpiente.getDireccion() != IZQUIERDA && serpiente.getDireccion() != DERECHA) {
						if (serpiente.getDireccion() == ARRIBA) {
							serpiente.girar(90);
						}
						else {
							serpiente.girar(-90);
						}
						serpiente.setDireccion(IZQUIERDA);
					}
				}
			}
			else {
				if (comenzar && action == GLFW_RELEASE) {
					serpiente.girar(90);
					switch (serpiente.getDireccion()) {
					case ARRIBA:
						serpiente.setDireccion(IZQUIERDA);
						break;
					case ABAJO:
						serpiente.setDireccion(DERECHA);
						break;
					case IZQUIERDA:
						serpiente.setDireccion(ABAJO);
						break;
					case DERECHA:
						serpiente.setDireccion(ARRIBA);
						break;
					}
				}
			}
		break;
		//dandole al espacio se empieza el juego
		case 32:
			comenzar = 1;
			break;
			//numero 0 camara alejada
		case 48:
			camara = 0;
			break;
			//numero 1 camara en la cabeza
		case 49:
			camara = 1;
			break;
		default:
			break;
	}
}

//funcion de reescalado
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	ANCHO = width;
	ALTO = height;
}
