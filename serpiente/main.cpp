#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <lecturaShader.h>
#include <string>
#include <unordered_map>

//Incluimos la definición de todas las clases que se utilizarán
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

GLuint shaderJuego, shaderTitulos; //Variables que referencian los shaders

bool comenzar = false;//Flag que marca si se ha iniciado el juego
bool perder = false;//Flag que marca si el jugador ha perdido
bool controles = false; //Flag que marca si el usuario se encuentra en la página de controles

GLuint camara = 0;//Flag que marca la cámara utilizada

//VAOS para crear los objetos
GLuint VAOCuadrado;
GLuint VAOCubo;
GLuint VAOEsfera;

Serpiente serpiente;
glm::vec3 observador;

//Índices de texturas
GLuint texturaSerpiente1, texturaSerpiente2, hierba1, hierba2, texturaOjo, texturaPerder, puntos[10], texturaPuntos, texturaSnake, texturaInicio, texturaControles;
std::unordered_map<unsigned int, GLuint> texturasFruta;//Mapa de las texturas de las frutas

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern GLuint setShaders(const char* nVertix, const char* nFrag);

void camaraAlejada() {//Visión del juego desde una posición cenital (usual del juego)
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//establecemos la posicion del observador
	observador = glm::vec3(0, 0, 20 * ESCALA);
	view = glm::lookAt(observador, glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, 1, .0f));
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

void camaraCabeza() {//Visión del juego desde la perspectiva de la serpiente
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//establecemos la posicion del observador
	observador = glm::vec3(serpiente.getCabeza().getX() - 8 * glm::cos(glm::radians((float)serpiente.getCabeza().getGiro())), serpiente.getCabeza().getY() - 8*glm::sin(glm::radians((float)serpiente.getCabeza().getGiro())), 8);
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
	glUniform3f(lightLoc, 1.0f, 1.0f, 1.0f);
	//luz difusa en la cabeza 
	unsigned int lightPosLoc = glGetUniformLocation(shaderJuego, "posicionLuzSerpiente");
	glUniform3f(lightPosLoc, serpiente.getCabeza().getX(), serpiente.getCabeza().getY(), 0.5);
	unsigned int luzDirLoc = glGetUniformLocation(shaderJuego, "direccionLuzSerpiente");
	glUniform3f(luzDirLoc, glm::cos(glm::radians((float)serpiente.getCabeza().getGiro())), glm::sin(glm::radians((float)serpiente.getCabeza().getGiro())), 0);
	//luz difusa en la fruta
	lightPosLoc = glGetUniformLocation(shaderJuego, "posicionLuzFruta");
	glUniform3f(lightPosLoc, comida.getX(), comida.getY(), 8);
	//luz especular
	GLuint observadorLoc = glGetUniformLocation(shaderJuego, "observador");
	glUniform3f(observadorLoc, observador.x, observador.y, observador.z);
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
	//Seleccinamos según el número de canales utilizados
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

void dibujarFin(GLuint shader) {//Función que dibuja la pantalla de fin de partida
	glUseProgram(shader);
	//viewport de la derecha
	glViewport(ANCHO / 2.0, 0, ANCHO / 2, ALTO);
	unsigned int modelLoc = glGetUniformLocation(shader, "model");
	//matriz de transformacion
	glm::mat4 model = glm::mat4();
	//trasladamos y escalamos
	model = glm::translate(model, glm::vec3(0, 0, 2));
	model = glm::scale(model, glm::vec3(20,20, 1));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, texturaPerder);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void dibujarInicio(GLuint shader) {//Función que dibuja la pantalla de inicio de la partida
	glUseProgram(shader);
	//pantalla completa
	glViewport(0, 0, ANCHO, ALTO);
	unsigned int modelLoc = glGetUniformLocation(shader, "model");
	//matriz de transformacion
	glm::mat4 model = glm::mat4();
	//escalamos
	model = glm::scale(model, glm::vec3(20, 20, 1));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, texturaInicio);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void dibujarControles(GLuint shader) {//Función que dibuja la pantalla de controles de la partida
	glUseProgram(shader);
	//pantalla completa
	glViewport(0, 0, ANCHO, ALTO);
	unsigned int modelLoc = glGetUniformLocation(shader, "model");
	//matriz de transformacion
	glm::mat4 model = glm::mat4();
	//escalamos
	model = glm::scale(model, glm::vec3(15, 20, 1));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, texturaControles);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void dibujarPuntos(GLuint shader) {//Función que dibuja los puntos y los títulos del juego
	glUseProgram(shader);
	//viewport de la izquierda
	glViewport(0, 0, ANCHO/2.0, ALTO);
	unsigned int modelLoc = glGetUniformLocation(shader, "model");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glm::mat4 model;
	//dibujamos el titulo snake
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, 6, 0));
	model = glm::scale(model, glm::vec3(15, 9, 1));
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glBindTexture(GL_TEXTURE_2D, texturaSnake);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//dibujamos el titulo puntos
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, -1, 0));
	model = glm::scale(model, glm::vec3(10, 3, 1));
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAOCuadrado);
	glBindTexture(GL_TEXTURE_2D, texturaPuntos);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//dibujamos los puntos
	for (int i = -3; i <= 3; i+=3) {
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, glm::vec3(i, -6, 0));
		model = glm::scale(model, glm::vec3(3, 4, 1));
		//La cargo
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOCuadrado);
		//puntos
		if (i == -3) {//En esta posición van las centenas
			glBindTexture(GL_TEXTURE_2D, puntos[serpiente.getPuntos() / 100]);
		}
		else if (i == 0) {//En esta posición van las decenas
			glBindTexture(GL_TEXTURE_2D, puntos[serpiente.getPuntos() / 10]);
		}
		else if (i == 3) {//En esta posición van las unidades
			glBindTexture(GL_TEXTURE_2D, puntos[serpiente.getPuntos() % 10]);
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void dibujarJuego(GLuint shader, Suelo suelo, Serpiente serpiente, Fruta comida) {
	glUseProgram(shader);
	glViewport(ANCHO / 2.0, 0, ANCHO / 2.0, ALTO);
	//Dibujo del suelo
	suelo.dibujar(shader);
	//dibujo la serpiente
	serpiente.dibujar(shader);
	//dibujo la comida
	comida.dibujar(shader);
	//iluminamos
	iluminacion(comida);
}


void openGlInit() {
	glClearDepth(1.0f); //Valor z-buffer
	glClearColor(7 / 255.0, 5 / 255.0, 18 / 255.0, 1.0f); //valor limpieza buffer color
	glEnable(GL_DEPTH_TEST); //z-buffer
	glEnable(GL_CULL_FACE); //ocultacion caras back
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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetKeyCallback(window, keyCallback);
	openGlInit();

	//Se setean los distintos shaders
	shaderJuego = setShaders("shader.vert", "shaderJuego.frag");
	shaderTitulos = setShaders("shader.vert", "shaderTitulos.frag");

	//creamos los VAOS
	creaVAO(vertices_cuadrado, sizeof(vertices_cuadrado), &VAOCuadrado, 1);
	creaVAO(vertices_esfera, sizeof(vertices_esfera), &VAOEsfera, 0);
	creaVAO(vertices_cubo, sizeof(vertices_cubo), &VAOCubo, 1);

	//creamos las texturas de la serpiente
	texturaSerpiente1 = cargaTextura("../texturas/serpiente_marron.jpg");
	texturaSerpiente2 = cargaTextura("../texturas/serpiente_amarilla.jpg");
	texturaOjo = cargaTextura("../texturas/ojo.jpg");

	//creamos las texturas de las frutas y las añadimos al mapa de texturas
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
	//creamos las texturas de la hierba
	hierba1 = cargaTextura("../texturas/hierba1.jpg");
	hierba2 = cargaTextura("../texturas/hierba2.jpg");

	//Creamos las texturas para los cambios de pantalla
	texturaPerder = cargaTextura("../texturas/perder.png");
	texturaPuntos = cargaTextura("../texturas/puntos.png");
	texturaSnake = cargaTextura("../texturas/snake.png");
	texturaInicio = cargaTextura("../texturas/inicio.png");
	texturaControles = cargaTextura("../texturas/controles.png");
	//texturas de los puntos
	std::string ruta;
	for (int i = 0; i <= 9; i++) {
		ruta = "../texturas/" + std::to_string(i) + ".png";
		puntos[i] = cargaTextura(ruta.c_str());
	}

	//creamos la serpiente
	serpiente = Serpiente(3, &VAOCubo, 36, &VAOEsfera, 1080, texturaSerpiente1, texturaSerpiente2, texturaOjo);
	//creamos la comida
	Fruta comida(ESCALA/2.0, 0, &VAOEsfera, texturasFruta, 1080, serpiente);
	//creamos el suelo
	Suelo suelo(VAOCuadrado, hierba1, hierba2);

	glUseProgram(shaderJuego);
	glUseProgram(shaderTitulos);
	//mientras no se cierre la ventana
	while (!glfwWindowShouldClose(window)){
		// input
		// -----
		processInput(window);

		glClearColor(7/255.0, 5/255.0, 18/255.0, 1.0f);//Borro el buffer de la ventana
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		switch (camara)	{//Se selecciona la cámara
			case 0:
				camaraAlejada();
				break;
			case 1: 
				camaraCabeza();
				break;
			default:
				break;
		}

		if (!controles) {
			if (!perder) {//mientras no perdamos
				if (!comenzar) {//si no empezamos
					//mostramos la pantalla de inicio
					dibujarInicio(shaderTitulos);
				}
				else {
					//sino dibujamos los puntos y la pantalla del juego
					dibujarPuntos(shaderTitulos);
					dibujarJuego(shaderJuego, suelo, serpiente, comida);
					//avanzamos y comprobamos que no haya colisiones
					perder = !serpiente.avanzar(&comida);
				}
			}
			else {//si perdemos
				comenzar = false;
				camara = 0;
				camaraAlejada();
				//dibujamos el fin
				dibujarPuntos(shaderTitulos);
				dibujarJuego(shaderJuego, suelo, serpiente, comida);
				dibujarFin(shaderTitulos);
			}
		}
		else {
			dibujarControles(shaderTitulos);
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
			if (camara == 0) {//Solo si se está jugando con la cámara alejada
				if (comenzar && action == GLFW_RELEASE) {
					//Si la serpiente no va ya en esa dirección o en la opuesta, se gira y actualiza la dirección
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
			if (camara == 0) {//Solo si se está jugando con la cámara alejada
				if (comenzar && action == GLFW_RELEASE) {
					//Si la serpiente no va ya en esa dirección o en la opuesta, se gira y actualiza la dirección
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
			if (camara == 0) {//Si se juega con la cámara alejada
				if (comenzar && action == GLFW_RELEASE) {
					//Si la serpiente no va ya en esa dirección o en la opuesta, se gira y actualiza la dirección
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
			}else {//Si se juega con la cámara de la serpiente
				if (comenzar && action == GLFW_RELEASE) {//Se gira y actualiza la dirección
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
			if (camara == 0) {//Si se juega con la cámara alejada
				if (comenzar && action == GLFW_RELEASE) {
					//Si la serpiente no va ya en esa dirección o en la opuesta, se gira y actualiza la dirección
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
			else {//Si se juega con la cámara de la serpiente
				if (comenzar && action == GLFW_RELEASE) {//Se gira y actualiza la dirección
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
		case 32://Dándole al espacio se empieza o reinicia el juego
			if (action == GLFW_RELEASE) {
				if (!controles) {
					if (perder) {//si hemos perdido volvemos a empezar
						comenzar = false;
						//reseteamos la serpiente
						serpiente = Serpiente(3, &VAOCubo, 36, &VAOEsfera, 1080, texturaSerpiente1, texturaSerpiente2, texturaOjo);
					}
					else {//si no hemos perdido empezamos
						comenzar = true;
					}
					perder = false;
				}
			}
			break;
		case 48://Número cambia a cámara alejada
			camara = 0;
			break;
		case 49://Número 1 cámara en la serpiente
			camara = 1;
			break;
		case 67://letra c para acceder a los controles
			if (action == GLFW_RELEASE && !comenzar && !perder) {
				controles = !controles;
			}
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
