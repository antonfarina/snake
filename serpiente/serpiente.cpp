#include <iostream>
#include "parte.h"
#include "fruta.h"
#include "serpiente.h"
#include <glfw3.h>

#define VELOCIDAD 0.3 //Tiempo que pasa entre cada paso de la serpiente al avanzar
#define OJOS_X 0.25 //Factor de variación coordenada x de los ojos
#define OJOS_Y 0.3 //Factor de variación coordenada y de los ojos
#define OJOS_COCIENTE 0.15 //Factor que divide la posición de los ojos con la variación

//constructor
Serpiente::Serpiente(int longitud, GLuint* VAOCubo, int nTriangulosCubo, GLuint* VAOEsfera, int nTriangulosEsfera) {
    //si la logitud no se sale del campo se establece a la longitud
    if (longitud < LIMITE) {
        this->longitud = longitud;
    } else {//sino se establece al limite
        this->longitud = LIMITE;
    }
    //Se crea la cabeza
    this->cabeza = Parte(0, 0, ESCALA / 2.0, ESCALA, 180, VAOCubo, 0, nTriangulosCubo);
    //Se crean los ojos
    this->ojoDerecho = Parte(-0.3/0.15, 0.25/0.15, 1/0.15, 0.15, 180, VAOEsfera, 0, nTriangulosEsfera);
    this->ojoIzquierdo = Parte(-0.3/0.15, -0.25/0.15, 1/0.15, 0.15, 180, VAOEsfera, 0, nTriangulosEsfera);
    this->direccion = IZQUIERDA;//Se fija una dirección inicial
    //inicializamos las posiciones donde se encuentra el cuerpo de la serpiente
    this->cuerpo.push_back(this->cabeza);
    for (int i = 1; i < longitud; i++) {
        this->cuerpo.push_back(Parte(i, 0, ESCALA/2.0, ESCALA, 180, VAOCubo, 0, nTriangulosCubo));
    }
    this->anterior = 0;//Tiempo en el que se ha actualizado la posición de la serpiente
    this->velocidad = VELOCIDAD;//Fijamos la velocidad como la velocidad inicial
    this->puntos = 0;//Ponemos los puntos a cero
}

bool Serpiente::avanzar(Fruta* comida) {//Función que avanza la serpiente, detecta si se come la fruta y colisiones
    double actual = glfwGetTime();
    // Realizar la actualización de la variable si han transcurrido 60 segundos desde la última actualización
    if (actual - anterior >= this->velocidad) {
        //comprobamos a que direccion hay que ir, que será la que marque la cabeza
        switch (this->direccion) {
            case ARRIBA:
                if (this->cabeza.getY() == LIMITE) {//Si se sale del tablero, vuelve a aparecer por el lado opuesto
                    this->cabeza.setY(-LIMITE);
                }else {//En otro caso, avanza una posición en la dirección
                    this->cabeza.setY(this->cabeza.getY() + 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() + OJOS_X) / OJOS_COCIENTE);
                this->ojoDerecho.setY((cabeza.getY() + OJOS_Y) / OJOS_COCIENTE);
                this->ojoIzquierdo.setX((cabeza.getX() - OJOS_X) / OJOS_COCIENTE);
                this->ojoIzquierdo.setY((cabeza.getY() + OJOS_Y) / OJOS_COCIENTE);
                break;
            case ABAJO:
                if (this->cabeza.getY() == -LIMITE) {//Si se sale del tablero, vuelve a aparecer por el lado opuesto
                    this->cabeza.setY(LIMITE);
                }
                else {//En otro caso, avanza una posición en la dirección
                    this->cabeza.setY(this->cabeza.getY() - 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() - OJOS_X) / OJOS_COCIENTE);
                this->ojoDerecho.setY((cabeza.getY() - OJOS_Y) / OJOS_COCIENTE);
                this->ojoIzquierdo.setX((cabeza.getX() + OJOS_X) / OJOS_COCIENTE);
                this->ojoIzquierdo.setY((cabeza.getY() - OJOS_Y) / OJOS_COCIENTE);
                break;
            case IZQUIERDA:
                if (this->cabeza.getX() == -LIMITE) {//Si se sale del tablero, vuelve a aparecer por el lado opuesto
                    this->cabeza.setX(LIMITE);
                }
                else {//En otro caso, avanza una posición en la dirección
                    this->cabeza.setX(this->cabeza.getX() - 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() - OJOS_Y) / OJOS_COCIENTE);
                this->ojoDerecho.setY((cabeza.getY() + OJOS_X) / OJOS_COCIENTE);
                this->ojoIzquierdo.setX((cabeza.getX() - OJOS_Y) / OJOS_COCIENTE);
                this->ojoIzquierdo.setY((cabeza.getY() - OJOS_X) / OJOS_COCIENTE);
                break;
            case DERECHA:
                if (this->cabeza.getX() == LIMITE) {//Si se sale del tablero, vuelve a aparecer por el lado opuesto
                    this->cabeza.setX(-LIMITE);
                }
                else {//En otro caso, avanza una posición en la dirección
                    this->cabeza.setX(this->cabeza.getX() + 1);
                }
                //actualizamos la posicion de los ojos
                this->ojoDerecho.setX((cabeza.getX() + OJOS_Y) / OJOS_COCIENTE);
                this->ojoDerecho.setY((cabeza.getY() - OJOS_X) / OJOS_COCIENTE);
                this->ojoIzquierdo.setX((cabeza.getX() + OJOS_Y) / OJOS_COCIENTE);
                this->ojoIzquierdo.setY((cabeza.getY() + OJOS_X) / OJOS_COCIENTE);
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
            //Aumentamos la velocidad para aumentar la dificultad
            if(velocidad - 0.03 > 0.2)velocidad -= 0.02;
            puntos++;//Sumamos un punto al contador
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
                return false;//Si la hay termina el juego
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

int Serpiente::getPuntos(){
    return this->puntos;
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

void Serpiente::girar(float giro){//Gira la serpiente con un ángulo que se suma al que ya tiene
    int angulo = this->cabeza.getGiro() + giro;
    this->cabeza.setGiro(angulo % 360);
    this->ojoDerecho.setGiro(angulo % 360);
    this->ojoIzquierdo.setGiro(angulo % 360);
}


