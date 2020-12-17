/*#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
}

   */




#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// texture samplers
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture_diffuse, TexCoords), texture(texture_specular, TexCoords), 0.2);
	//FragColor = texture(texture_specular, TexCoords);
	//FragColor = texture(texture_diffuse,TexCoords);
}