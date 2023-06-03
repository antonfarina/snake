#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 objectColor;

out vec4 FragColor;

uniform vec3 colorLuz;
uniform vec3 posicionLuzSerpiente;
uniform vec3 direccionLuzSerpiente;
uniform vec3 posicionLuzFruta;
uniform vec3 observador;
uniform sampler2D texture1;

void main(){
	//luz ambiente
	vec3 ambient = 0.75 * colorLuz;

	//luz difusa en la cabeza de la serpiente
	vec3 ld = normalize(-direccionLuzSerpiente);
	vec3 fd = normalize(vec3((-FragPos + posicionLuzSerpiente)));
	vec3 norm; 
	vec3 luzDir;
	float diff; 
	vec3 difusaCabeza;
	if(acos(dot(fd,ld)) < radians(35)){
		//Difusa
		norm = normalize(Normal);
		luzDir = normalize(posicionLuzSerpiente - FragPos);
		diff = max(dot(norm, luzDir), 0.0);
		difusaCabeza = 1.5 * diff * vec3(1.0f, 1.0f, 0.2f);
	} else {
		difusaCabeza = vec3(0.0, 0.0, 0.0);
	}
	//luz difusa en la fruta
	norm = normalize(Normal);
	luzDir = normalize(posicionLuzFruta - FragPos);
	diff = max(dot(norm, luzDir), 0.0);
	vec3 difusaFruta = 1.1 * diff * vec3(1.0f, 0.7f, 1.0f);

	//luz especular
	vec3 viewDir = normalize(observador - FragPos);
	vec3 reflectDir = reflect(-luzDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 especular = spec * colorLuz;

	//comprobamos si el objeto tiene textura
	if(texture2D(texture1, TexCoord)==vec4(0.0,0.0,0.0,1.0)){
		//si no tiene lo pintamos de su color
		vec3 result = ((ambient + difusaCabeza + difusaFruta + especular) * objectColor);
		FragColor = vec4(result, 1.0);
	}else{
		vec3 result = ((ambient + difusaCabeza + difusaFruta) * vec3(1.0, 1.0, 1.0))/2;
		FragColor = vec4(result, 1.0) * texture2D(texture1, TexCoord);
	}
}