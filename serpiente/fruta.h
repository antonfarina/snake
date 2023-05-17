#pragma once

#include "glad.h"
#include "parte.h"
#include "serpiente.h"

#define MANZANA 1
#define NARANJA 2
#define SANDIA 3
#define MELOCOTON 4

//clase hija de Parte
class Fruta: public Parte {
	private:
		int tipo;
	public:
		void colocar(class Serpiente *s);
		Fruta(float escalado, float giro, GLuint* geometria, GLuint textura, GLuint n, int tipo, class Serpiente s);
		void dibujar(GLuint shader);
};