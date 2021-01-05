#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 Normals;

out vec3 FragPos;
out vec3 normal;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    gl_Position =  model * projection * view * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0f));
    normal = mat3(transpose(inverse(model))) * Normals;  
}