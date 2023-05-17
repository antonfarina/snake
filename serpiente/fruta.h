#pragma once

#include "glad.h"
#include "parte.h"
#include "serpiente.h"
#include <unordered_map>

//clase hija de Parte
class Fruta: public Parte {
	private:
		std::unordered_map<unsigned int, GLuint> texturas;
	public:
		void colocar(class Serpiente *s);
		Fruta(float escalado, float giro, GLuint* geometria, std::unordered_map<unsigned int, GLuint> texturas, GLuint n, class Serpiente s);
		void dibujar(GLuint shader);
};