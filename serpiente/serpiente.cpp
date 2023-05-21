#include <iostream>
#include "parte.h"
#include "fruta.h"
#include "serpiente.h"
#include <glfw3.h>

//constructor
Serpiente::Serpiente(int longitud, GLuint* VAOCubo, int nTriangulosCubo, GLuint* VAOEsfera, int nTriangulosEsfera) {
    //si la logitud no se sale del campo se establece a la longitud
    if (longitud < LIMITE) {
        this->longitud = longitud;
    } else {//sino se establece al limite
        this->longitud = LIMITE;
    }
    
    this->cabeza = Parte(0, 0, ESCALA / 2.0, ESCALA, 180, VAOCubo, 0, nTriangulosCubo);
    //creamos los ojos
    this->ojoDerecho = Parte(-0.3/0.15, 0.25/0.15, 1/0.15, 0.15, 180, VAOEsfera, 0, nTriangulosEsfera);
    this->ojoIzquierdo = Parte(-0.3/0.15, -0.25/0.15, 1/0.15, 0.15, 180, VAOEsfera, 0, nTriangulosEsfera);
    this->direccion = IZQUIERDA;
    //inicializamos las posiciones donde se encuentra el cuerpo de la serpiente
    this->cuerpo.push_back(this->cabeza);
    for (int i = 1; i < longitud; i++) {
        this->cuerpo.push_back(Parte(i, 0, ESCALA/2.0, ESCALA, 180, VAOCubo, 0, nTriangulosCubo));
    }
    this->anterior = 0;
    this->velocidad = VELOCIDAD;
    this->puntos = 0;
}

bool Serpiente::avanzar(Fruta* comida) {
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
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() + 0.25) / 0.15);
                this->ojoDerecho.setY((cabeza.getY() + 0.3) / 0.15);
                this->ojoIzquierdo.setX((cabeza.getX() - 0.25) / 0.15);
                this->ojoIzquierdo.setY((cabeza.getY() + 0.3) / 0.15);
                break;
            case ABAJO:
                if (this->cabeza.getY() == -LIMITE) {
                    this->cabeza.setY(LIMITE);
                }
                else {
                    this->cabeza.setY(this->cabeza.getY() - 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() - 0.25) / 0.15);
                this->ojoDerecho.setY((cabeza.getY() - 0.3) / 0.15);
                this->ojoIzquierdo.setX((cabeza.getX() + 0.25) / 0.15);
                this->ojoIzquierdo.setY((cabeza.getY() - 0.3) / 0.15);
                break;
            case IZQUIERDA:
                if (this->cabeza.getX() == -LIMITE) {
                    this->cabeza.setX(LIMITE);
                }
                else {
                    this->cabeza.setX(this->cabeza.getX() - 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() - 0.3) / 0.15);
                this->ojoDerecho.setY((cabeza.getY() + 0.25) / 0.15);
                this->ojoIzquierdo.setX((cabeza.getX() - 0.3) / 0.15);
                this->ojoIzquierdo.setY((cabeza.getY() - 0.25) / 0.15);
                break;
            case DERECHA:
                if (this->cabeza.getX() == LIMITE) {
                    this->cabeza.setX(-LIMITE);
                }
                else {
                    this->cabeza.setX(this->cabeza.getX() + 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() + 0.3) / 0.15);
                this->ojoDerecho.setY((cabeza.getY() - 0.25) / 0.15);
                this->ojoIzquierdo.setX((cabeza.getX() + 0.3) / 0.15);
                this->ojoIzquierdo.setY((cabeza.getY() + 0.25) / 0.15);
                break;
            default:
                break;
                
        }
        
        //comprobamos si la serpiente come
        if (cabeza.getX() == comida->getX() && cabeza.getY() == comida->getY()) {
            //aumentamos la longitud
            longitud++;
            //añadimos una parte mas a la serpiente al final de todo
            this->cuerpo.push_back(Parte(cuerpo.back().getX(), cuerpo.back().getY(), cuerpo.back().getZ(), cuerpo[0].getEscalado(), cuerpo.back().getGiro(), cuerpo[0].getGeometria(), cuerpo[longitud - 3].getTextura(), 36));
            //colocamos nueva comida
            comida->colocar(this);
            if(velocidad - 0.03 > 0.2)velocidad -= 0.02;
            puntos++;
            printf("%d\n", puntos);
        }
        //actualizamos las posiciones del cuerpo con la posicion del anterior
        for (std::size_t i = this->longitud - 1; i > 0; i--) {
            //la posicion del siguiente elemento es la que tenia el anterior
            this->cuerpo[i].setX(this->cuerpo[i - 1].getX());
            this->cuerpo[i].setY(this->cuerpo[i - 1].getY());
            this->cuerpo[i].setGiro(this->cuerpo[i - 1].getGiro());
        }
        //la posicion inicial es la de la cabeza
        this->cuerpo[0] = this->cabeza;
        //reestablecemos el tiempo
        anterior = actual;
        //comprobamos si hay colisión
        for (int i = 1; i < this->longitud; i++) {
            if (this->cabeza.getX() == this->getCuerpo()[i].getX() && this->cabeza.getY() == this->getCuerpo()[i].getY()) {
                return false;
            }
        }
        return true;
    }
    return true;
}

void Serpiente::setDireccion(int direccion){
    this->direccion = direccion;
}

int Serpiente::getDireccion() {
    return this->direccion;
}

void Serpiente::dibujar(GLuint shader){
    //dibujamos los ojos
    this->ojoDerecho.dibujar(shader);
    this->ojoIzquierdo.dibujar(shader);
    //dibujamos la cabeza
    for (int i = 0; i < this->longitud; i++) {
        this->cuerpo[i].dibujar(shader);
    }
}

Parte Serpiente::getCabeza(){
    return cabeza;
}

int Serpiente::getLongitud(){
    return this->longitud;
}

std::vector<Parte> Serpiente::getCuerpo(){
    return this->cuerpo;
}

void Serpiente::texturizar(GLuint textura1, GLuint textura2, GLuint texturaOjo){
    //establecemos las texturas alternando entre las partes de la serpiente
    for (int i = 0; i < this->longitud;i++) {
        if (i % 2 == 0) {
            this->cuerpo[i].setTextura(textura1);
        }
        else {
            this->cuerpo[i].setTextura(textura2);
        }
    }
    this->cabeza.setTextura(this->cuerpo[0].getTextura());
    //ponemos las texturas de los ojos
    this->ojoDerecho.setTextura(texturaOjo);
    this->ojoIzquierdo.setTextura(texturaOjo);
}

void Serpiente::girar(float giro){
    int angulo = this->cabeza.getGiro() + giro;
    this->cabeza.setGiro(angulo % 360);
    this->ojoDerecho.setGiro(angulo % 360);
    this->ojoIzquierdo.setGiro(angulo % 360);
}


