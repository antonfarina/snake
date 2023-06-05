#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad.h"

#define LIMITE 10 //Tamaño del suelo
#define ESCALA 1 //Escalado del suelo

class Suelo {
private:
    int limite;
    float escala;
    GLuint textura1, textura2;
    GLuint VAO;
public:
    Suelo(GLuint VAO, GLuint textura1, GLuint textura2);
    void dibujar(GLuint shader);
    int getLimite();
    void setLimite(int limite);
    float getEscala();
    void setEscala(float escala);
};