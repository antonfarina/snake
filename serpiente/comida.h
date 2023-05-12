#pragma once

#include "glad.h"
#include "parte.h"
#include "serpiente.h"

//clase hija de Parte
class Comida: public Parte {
	private:
		int tipo;
	public:
		void colocar(class Serpiente *s);
		Comida(float escalado, float giro, GLuint* geometria, GLuint textura, GLuint n, int tipo, class Serpiente s);
		void dibujar(GLuint shader);
};