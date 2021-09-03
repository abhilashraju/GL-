#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 projection;

void GenerateLine(vec4 Pos,vec3 N)
{
    gl_Position = projection * Pos;
    EmitVertex();
    gl_Position = projection * (Pos + vec4(N, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}
vec3 generateNormal(int v1, int v2,int v3){
    return normalize(cross(vec3(gl_in[v1].gl_Position-gl_in[v2].gl_Position),vec3(gl_in[v3].gl_Position-gl_in[v2].gl_Position)));
}
void main()
{
    GenerateLine(gl_in[0].gl_Position,vec3(normalize(gs_in[0].normal))); // first vertex normal
    GenerateLine(gl_in[1].gl_Position,vec3(normalize(gs_in[1].normal))); // second vertex normal
    GenerateLine(gl_in[2].gl_Position,vec3(normalize(gs_in[2].normal))); // third vertex normal  
    //  vec3 N= generateNormal(0,2,1);
    // GenerateLine(gl_in[0].gl_Position,N); // first vertex normal
    // GenerateLine(gl_in[1].gl_Position,N); // second vertex normal
    // GenerateLine(gl_in[2].gl_Position,N); // third vertex normal 
}