#include "serpiente.hpp"
#include <glfw3.h>
#include <iostream>


Parte::Parte() {

}


Parte::Parte(float x, float y, float escalado, GLuint* geometria, GLuint textura, GLuint n) {
    this->x = x;
    this->y = y;
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

float Parte::getEscalado(){
    return this->escalado;
}


GLuint* Parte::getGeometria(){
    return this->geometria;
}

void Parte::setGeometria(GLuint* geom){
    this->geometria = geom;
}

GLuint Parte::getTextura(){
    return GLuint();
}

void Parte::setTextura(GLuint textura){
    this->textura = textura;
}

GLuint Parte::getNTriangulos(){
    return this->nTriangulos;
}

void Parte::setNTriangulos(GLuint n){
    this->nTriangulos = n;
}

//constructor
Serpiente::Serpiente(int longitud, GLuint* VAO, int nTriangulos) {
    //si la logitud no se sale del campo se establece a la longitud
    if (longitud < LIMITE) {
        this->longitud = longitud;
    }
    else {//sino se establece al limite
        this->longitud = LIMITE;
    }
    
    this->cabeza = Parte(0, 0, 0.3, VAO, 0, 1080);
    this->direccion = IZQUIERDA;
    //inicializamos las posiciones donde se encuentra el cuerpo de la serpiente
    this->cuerpo.push_back(this->cabeza);
    for (int i = 1; i < longitud; i++) {
        this->cuerpo.push_back(Parte(i, 0, 0.3, VAO, 0, 36));
    }
    this->anterior = 0;
    this->velocidad = 0.3;
    this->comida = Parte(1, 5, 0.3, VAO, 0, 36);
}

void Serpiente::avanzar() {
    double actual = glfwGetTime();
    // Realizar la actualización de la variable si han transcurrido 60 segundos desde la última actualización
    if (actual - anterior >= this->velocidad) {
        //comprobamos a que direccion hay que ir ya actualizamos la posicion de la cabeza de la serpiente
        switch (this->direccion) {
            case ARRIBA:
                if (this->cabeza.getY() == LIMITE) {
                    this->cabeza.setY(-LIMITE);
                }else {
                    this->cabeza.setY(this->cabeza.getY() + 1);
                }
                break;
            case ABAJO:
                if (this->cabeza.getY() == -LIMITE) {
                    this->cabeza.setY(+LIMITE);
                }
                else {
                    this->cabeza.setY(this->cabeza.getY() - 1);
                }
                break;
            case IZQUIERDA:
                if (this->cabeza.getX() == -LIMITE) {
                    this->cabeza.setX(LIMITE);
                }
                else {
                    this->cabeza.setX(this->cabeza.getX() - 1);
                }
                break;
            case DERECHA:
                if (this->cabeza.getX() == LIMITE) {
                    this->cabeza.setX(-LIMITE);
                }
                else {
                    this->cabeza.setX(this->cabeza.getX() + 1);
                }
                break;
            default:
                break;
        }
        //comprobamos si la serpiente come
        if (cabeza.getX() == comida.getX() && cabeza.getY() == comida.getY()) {
            //aumentamos la longitud
            longitud++;
            //añadimos una parte mas a la serpiente
            this->cuerpo.push_back(Parte(cuerpo.back().getX(), cuerpo.back().getY(), 0.4, cuerpo[0].getGeometria(), 0, 36));
            //colocamos nueva comida
            this->colocarComida();
            if(velocidad-0.03>0.2)velocidad -= 0.02;
        }
        //actualizamos las posiciones del cuerpo con la posicion del anterior
        for (std::size_t i = this->longitud - 1; i > 0; i--) {
            //la posicion del siguiente elemento es la que tenia el anterior
            this->cuerpo[i] = this->cuerpo[i - 1];
        }
        //la posicion inicial es la de la cabeza
        this->cuerpo[0] = this->cabeza;
        //reestablecemos el tiempo
        anterior = actual;
    }
}

void Serpiente::setDireccion(int direccion){
    this->direccion = direccion;
}

int Serpiente::getDireccion() {
    return this->direccion;
}

void Serpiente::dibujar(GLuint shader, glm::mat4 model){
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    for (int i = 0; i < this->longitud; i++) {
        //matriz identidad
        model = glm::mat4();
        //escalamos
        model = glm::scale(model, glm::vec3(this->cuerpo[i].getEscalado(), this->cuerpo[i].getEscalado(), this->cuerpo[i].getEscalado()));
        //trasladamos
        model = glm::translate(model, glm::vec3(this->cuerpo[i].getX(),this->cuerpo[i].getY(), 0));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //La cargo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //std::cout << this->cuerpo[i].getGeometria();
        glBindVertexArray(*this->cuerpo[i].getGeometria());
        glBindTexture(GL_TEXTURE_2D, this->cuerpo[i].getTextura());
        glDrawArrays(GL_TRIANGLES, 0, this->cuerpo[i].getNTriangulos());
    }
    //dibujamos la comida
    model = glm::mat4();
    //escalamos
    model = glm::scale(model, glm::vec3(0.3, 0.3, 0.3));
    //trasladamos
    model = glm::translate(model, glm::vec3(this->comida.getX(), this->comida.getY(), 0));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //La cargo
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(*this->comida.getGeometria());
   // glBindTexture(GL_TEXTURE_2D, this->comida.getTextura());
    glDrawArrays(GL_TRIANGLES, 0, this->comida.getNTriangulos());
}
//
//void Serpiente::estableceVAO(GLuint VAOcabeza, GLuint ncabeza, GLuint VAOcuerpo, GLuint ncuerpo){
//    this->cuerpo[0].setGeometria(VAOcabeza);
//    this->cuerpo[0].setNTriangulos(ncabeza);
//    this->cabeza.setGeometria(VAOcabeza);
//    this->cabeza=this->cuerpo[0];
//    for (int i = 1; i < this->longitud; i++) {
//        this->cuerpo[i].setGeometria(VAOcuerpo);
//        this->cuerpo[i].setNTriangulos(ncuerpo);
//    }
//}

Parte Serpiente::getCabeza(){
    return cabeza;
}

int Serpiente::getLongitud(){
    return this->longitud;
}

std::vector<Parte> Serpiente::getCuerpo(){
    return this->cuerpo;
}

void Serpiente::colocarComida(){
    float x, y, posicionValida;
    do {
        //establecemos la posicion como valida a priori
        posicionValida = 1;
        //generamos una posicion aleatoria
        x = rand() % (2 * LIMITE) - LIMITE;
        y = rand() % (2 * LIMITE) - LIMITE;
        //comprobamos si la serpiente no se encuentra en ese lugar ya
        for (int i = 0; i < this->getLongitud(); i++) {
            if (x == this->getCuerpo()[i].getX() && y == this->getCuerpo()[i].getY()) {
                posicionValida = 0;
                break;
            }
        }
    } while (posicionValida == 0);
    //establecemos la posicion a la posicion encontrada
    this->comida.setX(x);
    this->comida.setY(y);
}


