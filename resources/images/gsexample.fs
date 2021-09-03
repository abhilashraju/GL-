#version 330 core
out vec4 FragColor;

in Data{
    vec2 TexCoords;
    vec3 Color;
    vec3 Normal;
}data;

uniform sampler2D texture_diffuse1;

void main()
{
     FragColor = texture(texture_diffuse1, data.TexCoords);
   // FragColor =vec4(data.Color,1);
}
