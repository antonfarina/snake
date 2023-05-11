#pragma once

#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Parte {
    protected:
        float x, y;
        GLuint textura, * geometria, nTriangulos;
        float escalado;
        int giro;
    public:
        Parte();
        Parte(float x, float y, float escalado, int giro, GLuint* geometria, GLuint textura, GLuint n);
        void setX(float x);
        float getX();
        void setY(float y);
        float getY();
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
        void dibujar(GLuint shader);
};