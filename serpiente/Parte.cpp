#include "parte.h"

Parte::Parte() {

}

Parte::Parte(float x, float y, float escalado, int giro, GLuint* geometria, GLuint textura, GLuint n) {
    this->x = x;
    this->y = y;
    this->giro = giro;
    this->escalado = escalado;
    this->geometria = geometria;
    this->textura = textura;
    this->nTriangulos = n;
    
}


void Parte::setX(float x) {
    this->x = x;
}

void Parte::setY(float y) {
    this->y = y;
}

float Parte::getX() {
    return this->x;
}

float Parte::getY() {
    return this->y;
}

float Parte::getEscalado() {
    return this->escalado;
}

void Parte::setEscalado(float escalado) {
    this->escalado = escalado;
}

void Parte::setGiro(int giro){
    this->giro = giro;
}

int Parte::getGiro(){
    return this->giro;
}


GLuint* Parte::getGeometria() {
    return this->geometria;
}

void Parte::setGeometria(GLuint* geom) {
    this->geometria = geom;
}

GLuint Parte::getTextura() {
    return this->textura;
}

void Parte::setTextura(GLuint textura) {
    this->textura = textura;
}

GLuint Parte::getNTriangulos() {
    return this->nTriangulos;
}

void Parte::setNTriangulos(GLuint n) {
    this->nTriangulos = n;
}

void Parte::dibujar(GLuint shader){
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    //matriz de transformacion
    glm::mat4 model =  glm::mat4();
    //escalamos
    model = glm::scale(model, glm::vec3(this->escalado, this->escalado, this->escalado));
    //trasladamos
    model = glm::translate(model, glm::vec3(this->getX(), this->getY(), 0));
    model = glm::rotate(model, glm::radians((float)this->giro), glm::vec3(0, 0, 1));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //La cargo
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(*this->getGeometria());
    glBindTexture(GL_TEXTURE_2D, this->getTextura());
    glDrawArrays(GL_TRIANGLES, 0, this->getNTriangulos());
}
