/*
MIT License

Copyright (c) 2021 abhilashraju

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
                    )";
  
    auto on_failure = [](const std::error_code& code,auto log) {
        std::cout << code.message() << log << std::endl;
        
    };
   
    
    auto shaderProgram = make_programme(make_shader(vshader, GL_VERTEX_SHADER), make_shader(objfshader, GL_FRAGMENT_SHADER))(on_failure);

    auto commonshaderProgram = make_programme(make_shader(CommonVertexShader::to_shader_str().data(), GL_VERTEX_SHADER), make_shader(CommonFragmentShader::to_shader_str(true).data(), GL_FRAGMENT_SHADER))(on_failure);
    stbi_set_flip_vertically_on_load(true);
    Model ourModel("C:/Users/rabhil/work/GLplus/build/Debug/images/backpack.obj");
    Model lightModel("C:/Users/rabhil/work/GLplus/build/Debug/images/light.obj");

    int distance{ 10 };
    int number_of_lights = 0;
    win.setKeyCallback(decorate(win.keycallback,[&](int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_L && action == GLFW_PRESS) {
            distance += distance; distance = (distance > maxattenuationcoeff) ? maxattenuationcoeff : distance;
        }
        if (key == GLFW_KEY_K && action == GLFW_PRESS) {
            distance -= (distance >> 2); distance = (distance < 10) ? 10 : distance;
        }
        if (key == GLFW_KEY_M && action == GLFW_PRESS) {
            number_of_lights++; number_of_lights = (number_of_lights > 4) ? 4 : number_of_lights;
        }
        if (key == GLFW_KEY_N && action == GLFW_PRESS) {
            number_of_lights--; number_of_lights = (number_of_lights < 0) ? 0 : number_of_lights;
        }
           
        
    }));
    LightMesh lightObject;
    float lastFrame=0;
    win.setRenderCallback([&]() {
        float currentFrame = glfwGetTime();
        auto deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
      
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        shaderProgram.use();
         
        glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width/  win.height, 0.1f, 100.0f);
        glm::mat4 view = win.cam.GetViewMatrix();
        shaderProgram.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
        shaderProgram.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        //model = glm::rotate(model, glm::radians(currentFrame *30.f), glm::vec3{ 0,1,0 });
        shaderProgram.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
        shaderProgram.setUniform("nooflights", number_of_lights);
        std::array<SpotLight, 4> spotlights;
        using LightData = std::tuple<glm::vec3, float, glm::vec3>;
        std::array<LightData, 4> spotLightPositions = {
            LightData{glm::vec3{-4.,0.,4.},60, glm::vec3{ 0,1,0 }},
            LightData{glm::vec3{3.,0.,5.},-30, glm::vec3{ 0,1,0 }},
            LightData{glm::vec3{0.,5.,5.},90, glm::vec3{ 1,0,0 }},
            LightData{glm::vec3{0.,-3.,6.},-20, glm::vec3{ 1,0,0 }},
        };
        for (int i = 0; i < number_of_lights; i++) {
            auto& dirlight = spotlights[i];
            
            dirlight.rotate(glm::radians(currentFrame * std::get<1>(spotLightPositions[i])), std::get<2>(spotLightPositions[i])).translate(std::get<0>(spotLightPositions[i]));
            dirlight.direction = glm::vec3(model * glm::vec4(0, 0, 0, 1)) - dirlight.position();
            dirlight.cutOff = 6.5f;
            dirlight.outerCutOff = 12.5f;
            dirlight.setDistanceRange(distance);
            dirlight.setMaterial("sun");
        }
        
        apply_from_array("dirlights", spotlights, shaderProgram);
        shaderProgram.setUniform3v("camDir", 1, glm::value_ptr(win.cam.Position));
        ourModel.draw(shaderProgram);
        for (int i = 0; i < number_of_lights;i++) {
            auto& l = spotlights[i];
            auto m= glm::rotate(l.model(), glm::radians(0.f), glm::vec3{ 1,0,0 });
            lightModel.draw(commonshaderProgram, m, view, projection);
        }
        
     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



