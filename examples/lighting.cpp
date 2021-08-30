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
    
    //Debug debug;
    CamWindow win(SCR_WIDTH, SCR_HEIGHT);
   
    const char* vshader = R"(
                        #version 330 core
                        layout (location = 0) in vec3 aPos;
                        layout (location = 1) in vec3 aNormal;
                        layout (location = 2) in vec2 atextcord;

                        uniform mat4 model;
                        uniform mat4 view;
                        uniform mat4 projection;
                        out vec3 Normal;
                        out vec3 FragPos;
                        out vec2 TexCoords;
                        void main()
                        {
                            gl_Position = projection * view * model * vec4(aPos, 1.0);
                            FragPos=vec3(model*vec4(aPos,1));
                            Normal=mat3(inverse(transpose(model)))*aNormal;
                            TexCoords=atextcord;
                        } 
                        )";

    const char* objfshader = R"(
                    
                        #version 330 core
                        #import lights;
                       
                        out vec4 FragColor;
                        
                        uniform DirLight dirLight;
                        #define NR_POINT_LIGHTS 4  
                        uniform PointLight pointLights[NR_POINT_LIGHTS];
                        uniform SpotLight spotLights[NR_POINT_LIGHTS];
                        uniform TextureMeterial material;
                        uniform vec3 viewPos;
                        uniform int noofpointlights;
                        uniform int noofspotlights;
                        in vec3 Normal;
                        in vec3 FragPos;
                        in vec2 TexCoords;
                                           
                        void main()
                        {
                             // properties
                            vec3 norm = normalize(Normal);
                            vec3 viewDir = normalize(viewPos - FragPos);

                            // phase 1: Directional lighting
                            LightResults result= CalcDirLight(dirLight, norm, viewDir,material.shininess);
                         
                            // phase 2: Point lights
                            for(int i = 0; i < noofpointlights; i++){
                                 LightResults pointResult=CalcPointLight(pointLights[i], norm, FragPos, viewDir,material.shininess); 
                                 result.ambient += pointResult.ambient;
                                 result.diffuse += pointResult.diffuse;
                                 result.specular += pointResult.specular;
                            } 
                            for(int i = 0; i < noofspotlights; i++){
                                    LightResults spotResult=CalcSpotLight(spotLights[i], norm, FragPos, viewDir,material.shininess); 
                                    result.ambient += spotResult.ambient;
                                    result.diffuse += spotResult.diffuse;
                                    result.specular += spotResult.specular;
                            }     
                            // phase 3: Spot light
                            //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
                            vec3 color=(result.ambient+result.diffuse) * vec3(texture(material.diffuse, TexCoords)) +  result.specular*vec3(texture(material.specular, TexCoords));
                            FragColor = vec4(color, 1.0);
                            
                        }
                    )";
    const char* lsfshader = R"(
                                #version 330 core
                                #import lights;
                                out vec4 FragColor;
                                uniform DirLight light;
                                void main()
                                {
                                   FragColor = vec4(light.ambient,1.0f);
                                }
                            )";
    auto on_failure = [](const std::error_code& code,auto log) {
        std::cout << code.message() << log << std::endl;
        
    };
   
    
    auto shaderProgram = make_programme(make_shader(vshader, GL_VERTEX_SHADER), make_shader(objfshader, GL_FRAGMENT_SHADER))(on_failure);
    auto shaderProgram2 = make_programme(make_shader(vshader, GL_VERTEX_SHADER), make_shader(lsfshader, GL_FRAGMENT_SHADER))(on_failure);
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // add a new set of vertices to form a second triangle (a total of 6 vertices); the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
    float verticesdata[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    ArrayView<float> vertices{verticesdata,sizeof(verticesdata)/sizeof(float)};
    VBOS<2> vbo(GL_ARRAY_BUFFER);
    VAOS<2> vao;
    VTOS<3> vto(GL_TEXTURE_2D);
    {
          
        auto v = vao.bind(0);
        auto b = vbo.bind(0); 
        b.glBufferData(vertices, GL_STATIC_DRAW);
        v.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);
        v.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 , 3);
        v.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8, 6);
        auto t = vto.bind(0); 
        t.glTexImage2D(0,GL_RGB,GL_RGBA,GL_UNSIGNED_BYTE,"./container.png");

        auto t2 = vto.bind(1);
        t2.glTexImage2D(0, GL_RGB, GL_RGBA, GL_UNSIGNED_BYTE, "./container_specular.png");

        auto t3 = vto.bind(2);
        t3.glTexImage2D(0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, "./matrix.jpg");

        
        auto v2 = vao.bind(1);
        v2.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);
    }
    unsigned int lightintesity = 7;
    int nooflights = 0;
    int noofspotlights = 0;
    win.setKeyCallback(decorate(win.keycallback,[&](int key, int scancode, int action, int mods) {
        
            if (key == GLFW_KEY_L && action == GLFW_PRESS) {
                lightintesity+=lightintesity;
                if(lightintesity > maxattenuationcoeff) lightintesity= maxattenuationcoeff;
            }
            if (key == GLFW_KEY_K && action == GLFW_PRESS) {
                lightintesity-=(lightintesity>>2);
                if (lightintesity < 0) lightintesity = 1;
            }
            if (key == GLFW_KEY_N && action == GLFW_PRESS) {
                nooflights +=1;
                if (nooflights > 4) nooflights = 4;
                
            }
            if (key == GLFW_KEY_M && action == GLFW_PRESS) {
                nooflights -= 1;
                if (nooflights < 0) nooflights = 0;

            }
            if (key == GLFW_KEY_B && action == GLFW_PRESS) {
                noofspotlights += 1;
                if (noofspotlights > 4) noofspotlights = 4;

            }
            if (key == GLFW_KEY_V && action == GLFW_PRESS) {
                noofspotlights -= 1;
                if (noofspotlights < 0) noofspotlights = 0;

            }
        }));
    
    auto lightmeterial = "gold";
    auto spotlighmaterial="white rubber";
    win.setRenderCallback([&]() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        
        std::array<glm::vec3, 5> cubes = {
            glm::vec3{0.,0.,-1.},
            glm::vec3{-1.,0.,-1.},
            glm::vec3{-2.,2.,-3.},
            glm::vec3{-3.,0.,-2.},
            

        };
        
      
       
                    //light = light * (float)glfwGetTime();
       
      
        glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width /win.height, 0.1f, 100.0f);
        glm::mat4 view = win.cam.GetViewMatrix();

        DirLight light;
        light.setMaterial("brass");
        light.direction = glm::vec3{ 0,0,-1 };

        std::array<glm::vec3, 4> pointLightPositions = {
            glm::vec3{0.,0.,1.},
            glm::vec3{-1.,0.,1.},
            glm::vec3{-2.,2.,3.},
            glm::vec3{-3.,0.,2.},
        };
        std::array<PointLight, 4> pointlights;
        
        for (int i = 0; i < 4;i++) {
            auto& light = pointlights[i];
            light.translate(pointLightPositions[i]);
            light.setMaterial(lightmeterial);
            light.setDistanceRange(lightintesity);
        }
        std::array<SpotLight, 4> spotlights;
        std::array<glm::vec3, 4> spotLightPositions = {
            glm::vec3{1.,0.,1.},
            glm::vec3{-3.,3.,6.},
            glm::vec3{-4.,2.,2.},
            glm::vec3{-3.,-2.,5.},
        };
        for (int i = 0; i < 4; i++) {
            auto& light = spotlights[i];
            light.translate(spotLightPositions[i]);
            light.cutOff = glm::cos(glm::radians(12.5f));
            light.outerCutOff = glm::cos(glm::radians(24.5f));
            auto model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(25.0f), glm::vec3(0.f, 1.f, 0.f));
            model = glm::translate(model, light.position());
           
            light.direction = cubes[i]- glm::vec3(model*glm::vec4(0,0,0,1));
            light.setMaterial(spotlighmaterial);
            light.setDistanceRange(lightintesity);
        }
        vao.bind(0).execute([&](auto& va) {
                auto t1=vto.glActiveTexture(0);
                auto t2=vto.glActiveTexture(1);    
                auto t3=vto.glActiveTexture(2);
                shaderProgram.use();
                shaderProgram.setUniform("meterial.diffuse", 0);
                shaderProgram.setUniform("meterial.specular", 1);
                shaderProgram.setUniform("meterial.emission", 2);
                setMeterial("meterial", "gold", shaderProgram);
                light.apply("dirLight",  shaderProgram);
                apply_from_array("pointLights", pointlights, shaderProgram);
                shaderProgram.setUniform("noofpointlights", nooflights);
                apply_from_array("spotLights", spotlights, shaderProgram);
                shaderProgram.setUniform("noofspotlights", noofspotlights);
                
                shaderProgram.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
                auto viewpos= glm::vec3{-3,3,3};
                shaderProgram.setUniform3v("viewPos", 1, glm::value_ptr(viewpos));
                shaderProgram.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
               

                //set light source pointing to one of cube;
              
                /*shaderProgram.setUniform("light.innercutoff", 1, glm::cos(glm::radians(12.5f)));
                shaderProgram.setUniform("light.outercutoff", 1, glm::cos(glm::radians(25.f)));*/
                
                auto model = glm::mat4(1.0f);
                for (auto m : cubes) {
                    model = glm::translate(model, m);
                    //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(25.0f), glm::vec3(0.f, 1.f, 0.f));
                    //model = glm::scale(model, { 2,2,2 });

                    shaderProgram.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                
               
            
            });

        vao.bind(1).execute([&](auto&) {
           
            shaderProgram2.use();
            shaderProgram2.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
            shaderProgram2.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
            for (int i = 0; i < nooflights;i++) {
                auto& light = pointLightPositions[i];
                auto model = glm::mat4(1.0f);
                model = glm::translate(model, light);
                shaderProgram2.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
                setMeterial("light", lightmeterial, shaderProgram2);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            for (int i = 0; i < noofspotlights; i++) {
                auto& light = spotLightPositions[i];
                auto model = glm::mat4(1.0f);
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(25.0f), glm::vec3(0.f, 1.f, 0.f));
                model = glm::translate(model, light);
                
                shaderProgram2.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));
                setMeterial("light", spotlighmaterial, shaderProgram2);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            
           


            });
        
     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



