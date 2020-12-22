#version 330 core
out vec4 FragColor;

in vec3 v_position;
in vec2 v_TexCoord0;
uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;


void main()
{
     if(v_position.y > 15)
	 {
			FragColor=texture(u_Texture0, v_TexCoord0)*0.9;
	}
	else
    {
		float delta =  v_position.y/15;
		FragColor = mix(texture(u_Texture1, v_TexCoord0), texture(u_Texture0, v_TexCoord0), delta);
    //	FragColor=texture(u_Texture1, v_TexCoord0)*0.9;
	}

} 