#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;//法线向量
out vec3 FragPos;//片段位置
out float flag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float f;
void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0f) );
	flag=f;
}