#pragma once
#include "glad/glad.h" // holds all OpenGL type declarations

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl.hpp"
#include <string>
#include <vector>

namespace gl{
struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};
template< template<typename T> typename  Container=std::vector >
class MeshImpl {
public:
    // mesh Data
    Container<Vertex>       vertices;
    Container<unsigned int> indices;
    Container<Texture>      textures;
    VAOS<1> vao;
    VBOS<1> vbo{GL_ARRAY_BUFFER};
    VBOS<1> veo{GL_ELEMENT_ARRAY_BUFFER};
    
    // constructor
    MeshImpl(Container<Vertex> vertices, Container<unsigned int> indices, Container<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    MeshImpl(MeshImpl&& o):vao(std::move(o.vao)),
        vbo(std::move(o.vbo)),
        veo(std::move(o.veo)),
        vertices(std::move(o.vertices)),
        indices(std::move(o.indices)),
        textures(std::move(o.textures)) { 
        
    }
    ~MeshImpl()
    {
    }
    // render the mesh
    void Draw(GProgramme &shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
           glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
           // retrieve texture number (the N in diffuse_textureN)
           std::string number;
           std::string name = textures[i].type;
           if(name == "texture_diffuse")
               number = std::to_string(diffuseNr++);
           else if(name == "texture_specular")
               number = std::to_string(specularNr++); // transfer unsigned int to stream
           else if(name == "texture_normal")
               number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_height")
               number = std::to_string(heightNr++); // transfer unsigned int to stream

           // now set the sampler to the correct texture unit
           // glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
           shader.setUniform((name + number).c_str(), i);
           // and finally bind the texture
           glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        vao.bind(0).execute([&]{
           auto e = veo.bind(0);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        });
   
    }

private:
   
    // initializes all the buffer objects/arrays
    void setupMesh()
    {
   
        auto v=vao.bind(0);
        // load data into vertex buffers
        auto buff=vbo.bind(0);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        buff.glBufferData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);  
        auto elebuff=veo.bind(0);
        elebuff.glBufferData(indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        // set the vertex attribute pointers
        // vertex Positions
        v.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex)/sizeof(float), 0);
        // vertex normals
        v.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex)/sizeof(float), offsetof(Vertex, Normal)/sizeof(float));
        // vertex texture coords
        v.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex)/sizeof(float), offsetof(Vertex, TexCoords)/sizeof(float));
        // vertex tangent
        v.glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex)/sizeof(float), offsetof(Vertex, Tangent)/sizeof(float));
        // vertex bitangent
        v.glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex)/sizeof(float), offsetof(Vertex, Bitangent)/sizeof(float));

    }
};
using Mesh = MeshImpl<>;

struct LightMesh
{
    VAOS<1> vao;
    VBOS<1> vbo{GL_ARRAY_BUFFER};
    LightMesh(){
            float verticesdata[] = {
                // positions          // normals           // texture coords
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            };
            ArrayView<float> vertices{verticesdata,sizeof(verticesdata)/sizeof(float)};
            auto v = vao.bind(0);
            auto b = vbo.bind(0); 
            b.glBufferData(vertices, GL_STATIC_DRAW);
            v.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);
            v.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 , 3);
            v.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8, 6);
           
    }
    template<typename ShaderPgm>
    void draw(ShaderPgm& pgm,const glm::mat4& model,const glm::mat4& view, const glm::mat4& projection){
        vao.bind(0).execute([&] {
            pgm.use();
            pgm.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
            pgm.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
            pgm.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            });
    }
};
}
