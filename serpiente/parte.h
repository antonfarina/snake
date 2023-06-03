#pragma once

#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Parte {//Clase que representa cada una de las componentes de la serpiente
    protected:
        float x, y, z; //Coordenadas de posición
        GLuint textura, * geometria, nTriangulos; //Almacenan valores relativos a la textura o a cuestiones de la geometría de la parte.
        float escalado;
        int giro;
    public:
        Parte();
        Parte(float x, float y, float z, float escalado, int giro, GLuint* geometria, GLuint textura, GLuint n);
        //Getters y setter de todos los atributos
        void setX(float x);
        float getX();
        void setY(float y);
        float getY();
        void setZ(float z);
        float getZ();
        float getEscalado();
        void setEscalado(float escalado);
        void setGiro(int giro);
        int getGiro();
        GLuint* getGeometria();
        void setGeometria(GLuint* geom);
        GLuint getTextura();
        void setTextura(GLuint textura);
        GLuint getNTriangulos();
        void setNTriangulos(GLuint n);
        //Función que dibuja la parte
        void dibujar(GLuint shader);
};