#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 objectColor;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;


uniform sampler2D texture1;

void main(){
	if(texture2D(texture1, TexCoord) == vec4(0.0,0.0,0.0,1.0)){
		FragColor=vec4(objectColor, 1.0f);
	}else{
		FragColor=vec4(1.0, 1.0, 1.0, 1.0f) * texture2D(texture1, TexCoord);
	}
	
}