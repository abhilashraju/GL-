
#include "camwindow.h"
#include <fstream>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
using namespace gl;
int main(int argc, char* argv[])
{
  
   
    CamWindow win(SCR_WIDTH, SCR_HEIGHT);
   
    const char* vsource = R"(#version 330 core
                                        layout (location = 0) in vec3 aPos;
                                        layout (location = 1) in vec3 acolor;
                                        layout (location = 2) in vec2 texCord;
                                        out vec3 fcolor;
                                        out vec2 atextcord;
                                        uniform mat4 projection;
                                        uniform mat4 view;
                                        uniform mat4 model;
                                        void main()
                                        {
                                           gl_Position =  projection* view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
                                           fcolor=acolor;
                                           atextcord=texCord;
                                        }
                                    )";

    const char* fsource = R"(#version 330 core
                                out vec4 FragColor;
                                uniform vec4 acolor;
                                uniform sampler2D  texture1;
                                uniform sampler2D  texture2;
                                in vec2 atextcord;
                                void main()
                                {
                                   FragColor = mix(texture(texture1,atextcord),texture(texture2,atextcord),0.5);
                                }
                            )";
    const char* fsource2 = R"(#version 330 core
                                out vec4 FragColor;
                                in vec3 fcolor;
                                void main()
                                {
                                   FragColor = vec4(fcolor,1.0f);
                                }
                            )";
    auto on_failure = [](const std::error_code& code,auto log) {
        std::cout << code.message() << log << std::endl;
        
    };
   
    
    auto shaderProgram = make_programme(make_shader(vsource, GL_VERTEX_SHADER), make_shader(fsource, GL_FRAGMENT_SHADER))(on_failure);
    auto shaderProgram2 = make_programme(make_shader(vsource, GL_VERTEX_SHADER), make_shader(fsource2, GL_FRAGMENT_SHADER))(on_failure);
  
     std::array<float,288> vertices = {
         -0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,
          0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
          0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
          0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,

         -0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,
          0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
          0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,

         -0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,

          0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
          0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
          0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
          0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
          0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,
          0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,

         -0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
          0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
          0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
          0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,

         -0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f,
          0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 1.0f, 1.0f,
          0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f, 1.f,0.f,0.f, 0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f, 1.f,0.f,0.f, 0.0f, 1.0f
    };
    std::array<GLuint, 3> indices = {  // note that we start from 0!
        0, 1, 3,   // first triangle
      
    };
    std::array<GLuint, 3> indices2 = {  // note that we start from 0!
        1, 2, 3    // second triangle
    };
    VBOS<2> vbo(GL_ARRAY_BUFFER);
    VBOS<2> ebo(GL_ELEMENT_ARRAY_BUFFER);
    VAOS<2> vao;
    VTOS<2> vto(GL_TEXTURE_2D);
    {
        
        
        auto v = vao.bind(0);
        auto b = vbo.bind(0);
        auto t = vto.bind(0);
        t.glTexParameteri( GL_TEXTURE_WRAP_S, GL_REPEAT);
        t.glTexParameteri( GL_TEXTURE_WRAP_T, GL_REPEAT);
        t.glTexParameteri( GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        t.glTexParameteri( GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t.glTexImage2D(0, GL_RGB,GL_RGB, GL_UNSIGNED_BYTE,"./hand.jpg");

        auto t2 = vto.bind(1);
        t2.glTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
        t2.glTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
        t2.glTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        t2.glTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t2.glTexImage2D(0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, "./nature.jpg");

        b.glBufferData(vertices, GL_STATIC_DRAW);
        
        auto e = ebo.bind(0);
        e.glBufferData(indices, GL_STATIC_DRAW);
        v.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);
        v.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8, 6);

   
    }
   
    win.setRenderCallback([&]() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        
        std::array<glm::vec3, 5> cubes = {
            glm::vec3{1,0,0},
            glm::vec3{1,0,-2},
            glm::vec3{2,-2,-3},
            glm::vec3{-2,-4,-10},
            glm::vec3{-1,-4,-7},

        };
    
        vao.bind(0).execute([&] {
           
                glActiveTexture(GL_TEXTURE0);
                auto t = vto.bind(0);
                glActiveTexture(GL_TEXTURE1);
                auto t2 = vto.bind(1);
                shaderProgram.use();
                shaderProgram.setUniform("acolor", 1.f, .1f,1.f,0.6f);
                shaderProgram.setUniform("texture1", 0);
                shaderProgram.setUniform("texture2", 1);
                const float radius = 10.f;
                float camX = sin(glfwGetTime()) * radius;
                float camZ = cos(glfwGetTime()) * radius;
                glm::mat4 view= win.cam.GetViewMatrix();
               
                shaderProgram.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));

                glm::mat4 projection;
                projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
                shaderProgram.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
                for (auto& v : cubes) {
                    auto model = glm::mat4(1.0f);
                    
                    //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.f, 1.f, 1.f));
                    model = glm::translate(model, v);
                    shaderProgram.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
                    //glEnable(GL_DEPTH_TEST);
                    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                
                
            });
        
     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



