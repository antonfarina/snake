#include <ctime>
#include "fruta.h"
#include "serpiente.h"

Fruta::Fruta(float escalado, float giro, GLuint* geometria, std::unordered_map<unsigned int, GLuint> texturas, GLuint n, Serpiente s){
    this->z = escalado;
    this->escalado = escalado;
    this->geometria = geometria;
    this->texturas = texturas;
    this->nTriangulos = n;
    this->giro = giro;
    srand(std::time(nullptr));
    colocar(&s);//Se coloca la fruta en el suelo, en una posición no ocupada por la serpiente
}

void Fruta::colocar(Serpiente *s) {//Función que coloca la fruta en una posición aleatoria distinta de la ocupada por la serpiente
    float x, y, posicionValida;
    do {
        //establecemos la posicion como valida a priori
        posicionValida = 1;
        //generamos una posicion aleatoria
        x = rand() % (2 * LIMITE) - LIMITE;
        y = rand() % (2 * LIMITE) - LIMITE;
        //comprobamos si la serpiente no se encuentra en ese lugar ya, en ese caso, la posición deja de ser válida
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
    //le ponemos una textura aleatoria
    this->textura = this->texturas[rand() % this->texturas.size()];
}

void Fruta::dibujar(GLuint shader) {//Función que dibuja la fruta, sobreescribe el método de la clase padre Parte
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    unsigned int textura;
    //matriz de transformacion
    glm::mat4 model = glm::mat4();
    //trasladamos
    model = glm::translate(model, glm::vec3(this->getX(), this->getY(), this->getZ()));
    model = glm::scale(model, glm::vec3(this->escalado, this->escalado, this->escalado));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //La cargo
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(*this->getGeometria());
    glBindTexture(GL_TEXTURE_2D, this->getTextura());
    glDrawArrays(GL_TRIANGLES, 0, this->getNTriangulos());
    glBindTexture(GL_TEXTURE_2D, 0);
}



