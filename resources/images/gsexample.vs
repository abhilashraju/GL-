#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 Normal;
layout (location = 3) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;
out Data{
    vec2 TexCoords;
    vec3 Color;
    vec3 Normal;
}data;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    data.TexCoords = aTexCoords;
    data.Normal = Normal;
    data.Color = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
}