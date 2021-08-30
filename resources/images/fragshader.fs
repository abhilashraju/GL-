#version 330 core
const float offset = 1.0 / 300.0;  
vec2[9] get_offsets(){
    vec2 offsets[9] = vec2[](
                    vec2(-offset,  offset), // top-left
                    vec2( 0.0f,    offset), // top-center
                    vec2( offset,  offset), // top-right
                    vec2(-offset,  0.0f),   // center-left
                    vec2( 0.0f,    0.0f),   // center-center
                    vec2( offset,  0.0f),   // center-right
                    vec2(-offset, -offset), // bottom-left
                    vec2( 0.0f,   -offset), // bottom-center
                    vec2( offset, -offset)  // bottom-right    
                );
    return offsets;
}            
vec3 apply_kernal(sampler2D tex,vec2 texCord, float[9] kernel){
    vec3 sampleTex[9];
    vec2[9] offsets=get_offsets();
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(tex, texCord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}

vec3 apply_sharpness(sampler2D tex,vec2 texCord){
float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
return apply_kernal(tex,texCord,kernel);
}
vec3 apply_smoothness(sampler2D tex,vec2 texCord){
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    return apply_kernal(tex,texCord,kernel);
}
vec3 apply_emboss(sampler2D tex,vec2 texCord){
    float kernel[9] = float[](
       -2, -1, 0,
       -1,  1, 1,
        0, -1, 2  
    );
    return apply_kernal(tex,texCord,kernel);
}
vec3 apply_edgefilter(sampler2D tex,vec2 texCord){
    float kernel[9] = float[](
       1, 1, 1,
       1, -8, 1,
       1, 1, 1  
    );
    return apply_kernal(tex,texCord,kernel);
}
vec3 apply_greyscale(sampler2D tex,vec2 texCord){
    vec3 color = vec3(texture(tex, texCord));
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec3(average,average,average);
}
vec3 apply_inverted(sampler2D tex,vec2 texCord){
    return 1-vec3(texture(tex, texCord));
}
    