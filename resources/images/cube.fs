#version 330 core
#import ../resources/shaders/lighting.fs;
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    
    FragColor = vec4(texture(skybox, calculate_refract(Position,cameraPos,Normal,0,3)).rgb, 1.0);
}