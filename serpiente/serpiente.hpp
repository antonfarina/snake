#include<vector>
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ARRIBA 0
#define ABAJO 1
#define IZQUIERDA 2
#define DERECHA 3
#define LIMITE 10

class Parte {
    private:
        float x, y;
        GLuint textura, *geometria, nTriangulos;
        float escalado;
    public:
        Parte();
        Parte(float x, float y, float escalado, GLuint* geometria, GLuint textura, GLuint n);
        void setX(float x);
        void setY(float y);
        float getX();
        float getY();
        float getEscalado();
        GLuint* getGeometria();
        void setGeometria(GLuint* geom);
        GLuint getTextura();
        void setTextura(GLuint textura);
        GLuint getNTriangulos();
        void setNTriangulos(GLuint n);
};

class Serpiente {
    private:
        std::vector<Parte> cuerpo;
        Parte cabeza;
        int longitud, direccion;
        double anterior;
        double velocidad;
        Parte comida;
    public:
        Serpiente(int longitud, GLuint* VAO, int ntriangulos);
        void avanzar();
        void setDireccion(int direccion);
        int getDireccion();
        void dibujar(GLuint shader, glm::mat4 model);
        void estableceVAO(GLuint VAOcabeza, GLuint ncabeza, GLuint VAOcuerpo, GLuint ncuerpo);
        Parte getCabeza();
        int getLongitud();
        std::vector<Parte> getCuerpo();
        void colocarComida();
};