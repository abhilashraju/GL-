
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

    auto screenShader = make_programme(make_shader(R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;

        out vec2 TexCoords;

        void main()
        {
            TexCoords = aTexCoords;
            gl_Position = vec4(aPos.x, aPos.y, 0, 1.0); 
        }  
        )", GL_VERTEX_SHADER), 
        make_shader(R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoords;

        uniform sampler2D screenTexture;

        void main()
        {
            vec3 col = texture(screenTexture, TexCoords).rgb;
            FragColor = vec4(col, 1.0);
            //FragColor=vec4(1,0,0,1);
        } 
        )", GL_FRAGMENT_SHADER))(on_failure);

 
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
    //Model ourModel("C:/Users/rabhil/work/GLplus/build/Debug/images/backpack.obj");
    Model ourModel("C:/Users/rabhil/work/GLplus/build/Debug/images/light.obj");
    VAOS<1> quadVAO;
    VBOS<1> quadVBO(GL_ARRAY_BUFFER);
    quadVAO.bind(0).execute([&](auto& vao) {
        auto b = quadVBO.bind(0);
        b.glBufferData(quadVertices, GL_STATIC_DRAW);
        vao.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4, 0);
        vao.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4, 2);
    });
    
    VFOS<1> vfos;
 
    auto error = vfos.prepare(0, win.width, win.height);
    if (error) {
        std::cout << error.message()<<std::endl;
    }
    float lastFrame;
    win.setRenderCallback([&]() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        vfos.bind(0).execute([&](auto& fb) {
            
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            shader.use();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = win.cam.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width / win.height, 0.1f, 100.0f);
            shader.setUniformMatrix4("view", 1, false, glm::value_ptr(view));
            shader.setUniformMatrix4("projection", 1, false, glm::value_ptr(projection));
            model = glm::translate(model,glm::vec3(0, 0, -1));
            model = glm::rotate(model, glm::radians(currentFrame * 15.0f), glm::vec3(0, 1, 0));
            shader.setUniformMatrix4("model", 1, false, glm::value_ptr(model));
            ourModel.draw(shader);
         });
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();

       
        //
        glBindVertexArray(quadVAO[0]);
        glBindTexture(GL_TEXTURE_2D, vfos.texture(0));	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



