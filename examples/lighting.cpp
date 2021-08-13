
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
   
    const char* vshader = R"(
                        #version 330 core
                        layout (location = 0) in vec3 aPos;

                        uniform mat4 model;
                        uniform mat4 view;
                        uniform mat4 projection;

                        void main()
                        {
                            gl_Position = projection * view * model * vec4(aPos, 1.0);
                        } 
                        )";

    const char* objfshader = R"(
                    
                        #version 330 core
                        out vec4 FragColor;
                        
                        uniform vec3 objectColor;
                        uniform vec3 lightColor;

                        void main()
                        {
                            FragColor = vec4(lightColor * objectColor, 1.0);
                        }
                    )";
    const char* lsfshader = R"(
                                #version 330 core
                                out vec4 FragColor;
                                void main()
                                {
                                   FragColor = vec4(1.0f);
                                }
                            )";
    auto on_failure = [](auto log) {
        std::cout << "ERROR::Prgramme::Link Failed\n" << log << std::endl;
        
    };
   
    
    auto shaderProgram = make_programme(make_shader(vshader, GL_VERTEX_SHADER), make_shader(objfshader, GL_FRAGMENT_SHADER))(on_failure);
    auto shaderProgram2 = make_programme(make_shader(vshader, GL_VERTEX_SHADER), make_shader(lsfshader, GL_FRAGMENT_SHADER))(on_failure);
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // add a new set of vertices to form a second triangle (a total of 6 vertices); the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
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
   
    VBOS<2> vbo(GL_ARRAY_BUFFER);
    VAOS<2> vao;
    {
          
        auto v = vao.bind(0);
        auto b = vbo.bind(0); 
        b.glBufferData(vertices, GL_STATIC_DRAW);
        v.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);

        auto v2 = vao.bind(1);
        v2.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);
    }
   
    win.setRenderCallback([&]() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        
        std::array<glm::vec3, 5> cubes = {
            glm::vec3{1,0,0},
            glm::vec3{5,0,-2},

        };
        //std::array<decltype(shaderProgram),2> pgms={std::move(shaderProgram),std::move(shaderProgram2)};
        vao.bind(0).execute([&] {
            glm::mat4 projection;
            projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
                shaderProgram.use();
                shaderProgram.setUniform("objectColor", 1.0f, 0.5f, 0.31f);
                shaderProgram.setUniform("lightColor", 1.0f, 1.0f, 1.0f);

                glm::mat4 view = win.cam.GetViewMatrix();

                shaderProgram.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
                auto model = glm::mat4(1.0f);
                shaderProgram.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
                //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.f, 1.f, 1.f));
                model = glm::translate(model, cubes[0]);
                shaderProgram.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
                //glEnable(GL_DEPTH_TEST);
                //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDrawArrays(GL_TRIANGLES, 0, 36);
               
            
            });

        vao.bind(1).execute([&] {
            glm::mat4 projection;
            projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
            shaderProgram2.use();
          

            glm::mat4 view = win.cam.GetViewMatrix();

            shaderProgram2.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
            auto model = glm::mat4(1.0f);
            shaderProgram2.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
            //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.f, 1.f, 1.f));
            model = glm::translate(model, cubes[1]);
            shaderProgram2.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
            //glEnable(GL_DEPTH_TEST);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            });
        
     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



