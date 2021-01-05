#version 330 core
out vec4 FragColor;



uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in float flag;  
uniform vec3 viewPos;
uniform Light light;


void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(texture_diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse, TexCoords).rgb;  
    
    // specular
   // vec3 viewDir = normalize(viewPos - FragPos);
    vec3 viewDir = normalize(vec3(0.0f,0.0f,3.0f)- FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = light.specular * spec * texture(texture_specular, TexCoords).rgb;  
        
    vec3 result = ambient + diffuse + specular;
	
	if(flag>0)
	{
		
		result=mix(result,vec3(1.0f,1.0f,1.0f),0.6);
	}

	
    FragColor = vec4(result, 1.0);
} 	



