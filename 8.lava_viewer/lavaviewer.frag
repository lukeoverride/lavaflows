#version 330 core

//in vec3 altitude;

in vec3 FragPos;  
in vec3 Normal;
in vec2 TextCoords;  

out vec4 color;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D terrainTexture;

void main()
{
	vec3 colorT = texture(terrainTexture, TextCoords).rgb;
	// Ambient
    float ambientStrength = 0.8f;
    vec3 ambient = ambientStrength * lightColor * colorT;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0f);
    
	//color = vec4(altitude.z/10, altitude.z/10, altitude.z/10, 1.0f);
}
