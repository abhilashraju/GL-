#version 330 core
#import lights;
#import core;
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;


uniform vec3 camDir;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform int nooflights;
uniform SpotLight dirlights[4];
void main()
{   
    LightResults res;
    for(int i=0;i<nooflights;i++){
        LightResults cres=CalcSpotLight(dirlights[i],Normal,FragPos,camDir,0.25); 
        res.ambient+=cres.ambient;
        res.diffuse+=cres.diffuse;
        res.specular+=cres.specular;
    }
    

    vec3 color=(res.ambient+res.diffuse) * vec3(texture(texture_diffuse1, TexCoords)) +  res.specular*vec3(texture(texture_specular1, TexCoords));
    FragColor = vec4(color, 1.0);
}