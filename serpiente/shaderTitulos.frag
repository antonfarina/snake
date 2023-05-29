#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 objectColor;

out vec4 FragColor;

uniform sampler2D texture1;

void main(){
	//comprobamos si el objeto tiene textura
	if(texture2D(texture1, TexCoord)==vec4(0.0,0.0,0.0,1.0)){
		//si no tiene lo pintamos de su color
		vec3 result = objectColor;
		FragColor = vec4(result, 1.0);
	}else{
		vec3 result = vec3(1.0, 1.0, 1.0);
		FragColor = vec4(result, 1.0) * texture2D(texture1, TexCoord);
	}
}