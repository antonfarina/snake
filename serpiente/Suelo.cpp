#include "suelo.h"


Suelo::Suelo(GLuint VAO, GLuint textura1, GLuint textura2) {
	this->limite = LIMITE;
	this->escala = ESCALA;
	this->VAO = VAO;
	this->textura1 = textura1;
	this->textura2 = textura2;
}

void Suelo::dibujar(GLuint shader) {//Función que dibuja el suelo
	//creamos las matrices del modelo
	glm::mat4 model;
	//la busco en el shader
	unsigned int modelLoc = glGetUniformLocation(shader, "model");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	int i, j;
	for (i = -limite; i <= limite; i++) {
		for (j = -limite; j <= limite; j++) {
			//Calculo la matriz
			model = glm::mat4(); //identity matrix
			//trasladamos para dibujar cada cuadrado
			model = glm::translate(model, glm::vec3(i * escala, j * escala, 0.0f));
			model = glm::scale(model, glm::vec3(escala, escala, 1));
			if ((i + j) % 2) {
				glBindTexture(GL_TEXTURE_2D, textura1);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, textura2);
			}
			//La cargo
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//dibujamos el cuadrado
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}

int Suelo::getLimite(){
	return this->limite;
}

void Suelo::setLimite(int limite){
	this->limite = limite;
}

float Suelo::getEscala(){
	return this->escala;
}

void Suelo::setEscala(float escala){
	this->escala = escala;
}
