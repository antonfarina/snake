#include <ctime>
#include "comida.h"
#include "serpiente.h"

Comida::Comida(float escalado, GLuint* geometria, GLuint textura, GLuint n, int tipo, Serpiente s){
    this->escalado = escalado;
    this->geometria = geometria;
    this->textura = textura;
    this->nTriangulos = n;
    this->tipo = tipo;
    srand(std::time(nullptr));
    colocar(&s);
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


