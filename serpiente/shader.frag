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
	FragColor=vec4(objectColor, 1.0f);
}