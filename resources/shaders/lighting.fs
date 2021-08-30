#version 330 core
float refindices[5]=float[](
    1.00, //Air
    1.33, //Water
    1.309, //Ice
    1.52,  //Glass
    2.42  //Diamond
);

vec3 calculate_reflect(vec3 position,vec3 campos,vec3 N)
{
 vec3 dirvec=normalize(position-campos);
 return reflect(dirvec,normalize(N));
}
vec3 calculate_refract(vec3 position,vec3 campos,vec3 N,float frindx,float torndx)
{
    vec3 dirvec=normalize(position-campos);
    return refract(dirvec,normalize(N),frindx/torndx);
}