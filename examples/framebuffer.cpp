
#include "camwindow.h"
#include <fstream>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include "lighting.hpp"
#include "basic_types.hpp"
#include "debug.hpp"
#include "model.hpp"
#include <filesystem>
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
using namespace gl;
template<typename T>
auto& operator<<(T& os, const glm::vec3& vec) {
    os << "Vector " << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}\n";
    return os;
}
int main(int argc, char* argv[])
{
    auto apppath = std::filesystem::current_path();
    //Debug debug;
    CamWindow win(SCR_WIDTH, SCR_HEIGHT);
   
    const char* vshader = R"(
                        #version 330 core
                        #import core;
                        layout (location = 0) in vec3 aPos;
                        layout (location = 1) in vec3 aNormal;
                        layout (location = 2) in vec2 aTexCoords;
                
                        out vec2 TexCoords;
                        out vec3 Normal;
                        out vec3 FragPos;

                        uniform mat4 model;
                        uniform mat4 view;
                        uniform mat4 projection;
                      
                        void main()
                        {
                            TexCoords = aTexCoords;    
                            gl_Position = projection * view * model * vec4(aPos, 1.0);
                            Normal = AdjustNormal(model,aNormal);
                            FragPos= mat3(model) * aPos;
                        }
                        )";

    const char* objfshader = R"(
                    
                            #version 330 core
                            out vec4 FragColor;
                            in vec2 TexCoords; 
                            in vec3 FragPos;
                            uniform sampler2D texture_diffuse1;
                         
                            void main()
                            {   
                                FragColor = texture(texture_diffuse1, TexCoords);
                            }
                    )";
  
    auto on_failure = [](const std::error_code& code,auto log) {
        std::cout << code.message() << log << std::endl;
        
    };
   
    
    auto shader = make_programme(make_shader(vshader, GL_VERTEX_SHADER), make_shader(objfshader, GL_FRAGMENT_SHADER))(on_failure);
    const char* vertexSS = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;

        out vec2 TexCoords;
        uniform mat4 model;
                        uniform mat4 view;
                        uniform mat4 projection;
        
        void main()
        {
            TexCoords = aTexCoords;
            gl_Position = projection * view * model  *  vec4(aPos.x, aPos.y, 0, 1.0); 
        }  
        )";
    GProgramme screenShaders[] = {
        make_programme(make_shader(vertexSS, GL_VERTEX_SHADER),
        make_shader(R"(
        #version 330 core
        
        #import ./images/fragshader.fs;
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
            FragColor = vec4(apply_inverted(screenTexture,TexCoords), 1.0);
        } 
        )", GL_FRAGMENT_SHADER))(on_failure),
        make_programme(make_shader(vertexSS, GL_VERTEX_SHADER),
        make_shader(R"(
        #version 330 core
        #import ./images/fragshader.fs;
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
            FragColor = vec4(apply_sharpness(screenTexture,TexCoords), 1.0);
        } 
        )", GL_FRAGMENT_SHADER))(on_failure),
        make_programme(make_shader(vertexSS, GL_VERTEX_SHADER),
        make_shader(R"(
        #version 330 core
        #import ./images/fragshader.fs;
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
           FragColor = vec4(apply_smoothness(screenTexture,TexCoords), 1.0);
        } 
        )", GL_FRAGMENT_SHADER))(on_failure),
        make_programme(make_shader(vertexSS, GL_VERTEX_SHADER),
        make_shader(R"(
        #version 330 core
        #import ./images/fragshader.fs;
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
           FragColor = vec4(apply_edgefilter(screenTexture,TexCoords), 1.0);
           
        } 
        )", GL_FRAGMENT_SHADER))(on_failure),
        make_programme(make_shader(vertexSS, GL_VERTEX_SHADER),
        make_shader(R"(
        #version 330 core
        #import ./images/fragshader.fs;
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
           FragColor = vec4(apply_greyscale(screenTexture,TexCoords), 1.0);
           
        } 
        )", GL_FRAGMENT_SHADER))(on_failure),
        make_programme(make_shader(vertexSS, GL_VERTEX_SHADER),
        make_shader(R"(
        #version 330 core
        #import ./images/fragshader.fs;
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
           FragColor = vec4(apply_emboss(screenTexture,TexCoords), 1.0);
           
        } 
        )", GL_FRAGMENT_SHADER))(on_failure),

    };
   

 
    float vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    ArrayView quadVertices(vertices, sizeof(vertices) / sizeof(float));
    stbi_set_flip_vertically_on_load(true);
     Model ourModel("C:/Users/rabhil/work/GLplus/build/Debug/images/monkey.obj");
    VAOS<1> quadVAO;
    VBOS<1> quadVBO(GL_ARRAY_BUFFER);
    quadVAO.bind(0).execute([&](auto& vao) {
        auto b = quadVBO.bind(0);
        b.glBufferData(quadVertices, GL_STATIC_DRAW);
        vao.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4, 0);
        vao.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4, 2);
    });
    
    FBO vfos;
    float picwd = 600;
    float picht = 600;
    auto error = vfos.prepare( picwd, picht);
    if (error) {
        std::cout << error.message()<<std::endl;
    }
    Camera innercam{ glm::vec3(0.0f, 0.0f,2.0f) };
    vfos.bind().execute([&](auto& fb) {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = innercam.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(innercam.Zoom), picwd / picht, 0.1f, 100.0f);
        shader.setUniformMatrix4("view", 1, false, glm::value_ptr(view));
        shader.setUniformMatrix4("projection", 1, false, glm::value_ptr(projection));
        shader.setUniformMatrix4("model", 1, false, glm::value_ptr(model));
        ourModel.draw(shader);
     });
    float lastFrame;
    std::array<glm::vec3, 6> previewpos{
        glm::vec3( -4, -1,-1),
        glm::vec3(  0,-1,-1),
        glm::vec3(  4,-1,-1),
        glm::vec3( -4, 2,-1),
        glm::vec3(  0,2,-1),
        glm::vec3(  4,2,-1),
        
    };
    win.cam.setPosition(0, 0, 8);
   
    win.setRenderCallback([&]() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(quadVAO[0]);
        glBindTexture(GL_TEXTURE_2D, vfos.texture());	// use the color attachment texture as the texture of the quad plane
        
       
        glm::mat4 view = win.cam.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width / win.height, 0.1f, 100.0f);
        int i = 0;
        int maxShaders = sizeof(screenShaders) / sizeof(GProgramme);
        for (auto p : previewpos) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model,p);
            screenShaders[i % maxShaders].use();
            screenShaders[i % maxShaders].setUniformMatrix4("view", 1, false, glm::value_ptr(view));
            screenShaders[i % maxShaders].setUniformMatrix4("projection", 1, false, glm::value_ptr(projection));
            model = glm::rotate(model, glm::radians(currentFrame * 15.0f), glm::vec3(0, 1, 0));
            screenShaders[i % maxShaders].setUniformMatrix4("model", 1, false, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            i++;
        }
       

     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



