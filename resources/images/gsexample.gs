
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
in Color{
  vec3 color;
}color[];

out FData{
 vec3 color;
 vec3 Normal;
}fdata;

uniform mat4 projection;
vec3 getNormal(vec3 first,vec3 second){
    return normalize(cross(first,second));
}
void makeLine(int v1,vec3  n){
    gl_Position =projection*  gl_in[v1].gl_Position; 
    fdata.color=color[v1].color;
    EmitVertex();
    gl_Position =projection* vec4(n *3,1) ; 
    fdata.color=color[v1].color;
    EmitVertex();
}
void main() {
    fdata.Normal=getNormal(vec3(gl_in[0].gl_Position-gl_in[1].gl_Position),vec3(gl_in[2].gl_Position-gl_in[1].gl_Position));
    makeLine(0,fdata.Normal);
    makeLine(1,fdata.Normal);
    makeLine(2,fdata.Normal);
   
}  