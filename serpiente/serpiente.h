#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad.h"
#include "parte.h"
#include "fruta.h"

#define ARRIBA 0
#define ABAJO 1
#define IZQUIERDA 2
#define DERECHA 3
#define LIMITE 10
#define ESCALA 1
#define VELOCIDAD 0.3

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
        Serpiente(int longitud, GLuint* VAOCubo, int nTriangulosCubo, GLuint* VAOEsfera, int nTriangulosEsfera);
        void avanzar(class Fruta* comida);
        void setDireccion(int direccion);
        int getDireccion();
        void dibujar(GLuint shader);
        Parte getCabeza();
        int getLongitud();
        std::vector<Parte> getCuerpo();
        void texturizar(GLuint textura1, GLuint textura2, GLuint texturaOjo);
        void girar(float giro);
};