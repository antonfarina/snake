#include <ctime>
#include "comida.h"
#include "serpiente.h"

Comida::Comida(float escalado, float giro, GLuint* geometria, GLuint textura, GLuint n, int tipo, Serpiente s){
    this->escalado = escalado;
    this->geometria = geometria;
    this->textura = textura;
    this->nTriangulos = n;
    this->tipo = tipo;
    this->giro = giro;
    srand(std::time(nullptr));
    colocar(&s);
}

void Comida::dibujar(GLuint shader){
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    //matriz de transformacion
    glm::mat4 model = glm::mat4();
    //escalamos
    model = glm::scale(model, glm::vec3(this->escalado, this->escalado, this->escalado));
    //trasladamos
    model = glm::translate(model, glm::vec3(this->getX(), this->getY(), 0));
    model = glm::rotate(model, glm::radians((float)this->giro), glm::vec3(1, 0, 0));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //La cargo
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(*this->getGeometria());
    glBindTexture(GL_TEXTURE_2D, this->getTextura());
    glDrawArrays(GL_TRIANGLES, 0, this->getNTriangulos());

}

void Comida::colocar(Serpiente *s) {
    float x, y, posicionValida;
    do {
        //establecemos la posicion como valida a priori
        posicionValida = 1;
        //generamos una posicion aleatoria
        x = rand() % (2 * LIMITE) - LIMITE;
        y = rand() % (2 * LIMITE) - LIMITE;
        //comprobamos si la serpiente no se encuentra en ese lugar ya
        for (int i = 0; i < s->getLongitud(); i++) {
            if (x == s->getCuerpo()[i].getX() && y == s->getCuerpo()[i].getY()) {
                posicionValida = 0;
                break;
            }
        }
    } while (posicionValida == 0);
    //establecemos la posicion a la posicion encontrada
    this->x = x;
    this->y = y;
}


