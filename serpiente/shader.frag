#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 objectColor;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 luzDir;

uniform sampler2D texture1;

void main(){
	float ambientI = 3;
	vec3 ambient = ambientI * lightColor;
	vec3 ld = normalize(-luzDir);
	vec3 fd = normalize(vec3((-FragPos + lightPos)));

	if(acos(dot(fd,ld)) < radians(35)){
		//Difusa
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = 3 * diff * vec3(0.7f, 0.7f, .3f);

		//Especular
		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
		vec3 specular = specularStrength * spec * lightColor;

		//comprobamos si el objeto tiene textura
		if(texture2D(texture1, TexCoord)==vec4(0.0,0.0,0.0,1.0)){
			//si no tiene lo pintamos de su color
			vec3 result = ((ambient + diffuse + specular) * objectColor)/2;
			FragColor = vec4(result, 1.0);
		}else{
			vec3 result = ((ambient + diffuse + specular) * vec3(1.0, 1.0, 1.0))/2;
			FragColor = vec4(result, 1.0) * texture2D(texture1, TexCoord);
		}
	} else {
		//comprobamos si el objeto tiene textura
		if(texture2D(texture1, TexCoord)==vec4(0.0,0.0,0.0,1.0)){
			//si no tiene lo pintamos de su color
			vec3 result = ((ambient) * objectColor)/2;
			FragColor = vec4(result, 1.0);
		}else{
			vec3 result = ((ambient) * vec3(1.0, 1.0, 1.0))/2;
			FragColor = vec4(result, 1.0) * texture2D(texture1, TexCoord);
		}
	}
}