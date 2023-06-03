#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad.h"
#include "parte.h"
#include "fruta.h"
#include "suelo.h"

#define ARRIBA 0
#define ABAJO 1
#define IZQUIERDA 2
#define DERECHA 3

class Serpiente {
    private:
        std::vector<Parte> cuerpo;
        Parte cabeza;
        Parte ojoIzquierdo;
        Parte ojoDerecho;
        int longitud, direccion;
        double anterior;
        double velocidad;
        int puntos;
    public:
        Serpiente();
        Serpiente(int longitud, GLuint* VAOCubo, int nTriangulosCubo, GLuint* VAOEsfera, int nTriangulosEsfera, GLuint textura1, GLuint textura2, GLuint texturaOjos);
        bool avanzar(class Fruta* comida);//Función que avanza la serpiente, come la fruta y detecta colisiones
        //Getters y setter de los atributos
        void setDireccion(int direccion);
        int getDireccion();
        void dibujar(GLuint shader);
        Parte getCabeza();
        int getLongitud();
        int getPuntos();
        std::vector<Parte> getCuerpo();
        void girar(float giro);//Función que gira el cuerpo de la serpiente
};