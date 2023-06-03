#pragma once

#include "glad.h"
#include "parte.h"
#include "serpiente.h"
#include <unordered_map>
#include "suelo.h"

//clase hija de Parte
class Fruta: public Parte {
	private:
		std::unordered_map<unsigned int, GLuint> texturas; //Mapa de las texturas de las diferentes frutas
	public:
		void colocar(class Serpiente *s); //Coloca de forma aleatoria la fruta en una posición no ocupada por la serpiente
		Fruta(float escalado, float giro, GLuint* geometria, std::unordered_map<unsigned int, GLuint> texturas, GLuint n, class Serpiente s);
		void dibujar(GLuint shader);//Dibuja la fruta con el shader elegido
};