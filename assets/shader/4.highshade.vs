#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 v_position;
out vec2 v_TexCoord0;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position =  model * projection * view * vec4(aPos, 1.0);
    v_position = vec3(model * vec4(aPos, 1.0f));
    v_TexCoord0 = aTexCoords;  
}