#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad.h"
#include "parte.h"
#include "comida.h"

#define ARRIBA 0
#define ABAJO 1
#define IZQUIERDA 2
#define DERECHA 3
#define LIMITE 10
#define ESCALA 0.3

class Serpiente {
    private:
        std::vector<Parte> cuerpo;
        Parte cabeza;
        int longitud, direccion;
        double anterior;
        double velocidad;
    public:
        Serpiente(int longitud, GLuint* VAOserpiente, int ntriangulos);
        void avanzar(class Comida* comida);
        void setDireccion(int direccion);
        int getDireccion();
        void dibujar(GLuint shader);
        Parte getCabeza();
        int getLongitud();
        std::vector<Parte> getCuerpo();
        void texturizar(GLuint textura1, GLuint textura2);
        void girar(float giro);
};