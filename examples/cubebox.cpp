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
#include <fstream>
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
using namespace gl;
template<typename T>
auto& operator<<(T& os, const glm::vec3& vec) {
    os << "Vector " << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}\n";
    return os;
}
VTO  loadTexture(const std::string& path)
{
    VTO vto(GL_TEXTURE_2D);
    vto.bind().execute([&](auto& to)
    {
         to.glTexImage2D(0,GL_UNSIGNED_BYTE, path);
 
    });
    return std::move(vto);

}

int main(int argc, char* argv[])
{
    namespace fs = std::filesystem;
   
    //Debug debug;
    CamWindow win(SCR_WIDTH, SCR_HEIGHT);
  
    auto on_failure = [](const std::error_code& code,auto log) {
        std::cout << code.message() << log << std::endl;
        
    };
    std::ifstream vfile;
    vfile.open(fs::current_path().generic_string() + "/resources/images/cube.vs");
    std::ifstream ffile;
    ffile.open(fs::current_path().generic_string() + "/resources/images/cube.fs");

    std::ifstream skyvfile;
    skyvfile.open(fs::current_path().generic_string() + "/resources/images/skybox.vs");
    std::ifstream skyffile;
    skyffile.open(fs::current_path().generic_string() + "/resources/images/skybox.fs");
    
    auto shader = make_programme(make_shader(vfile, GL_VERTEX_SHADER), make_shader(ffile, GL_FRAGMENT_SHADER))(on_failure);
    auto skyboxShader = make_programme(make_shader(skyvfile, GL_VERTEX_SHADER), make_shader(skyffile, GL_FRAGMENT_SHADER))(on_failure);
    VAO cubeVAO;
    VBO cubevbo(GL_ARRAY_BUFFER);
    cubeVAO.bind().execute([&](auto &vao){
        std::ifstream file;
        file.open(fs::current_path().generic_string() + "/resources/images/cubedata.data");
        std::stringstream s;
        s<< file.rdbuf();
        auto bo = cubevbo.bind();
        bo.loadFromStream(s);     
        vao.glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6,0);
        vao.glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6,3);
    });
    VAO skyboxVAO;
    VBO skyvbo(GL_ARRAY_BUFFER);
    skyboxVAO.bind().execute([&](auto &vao){
        auto bo = skyvbo.bind();
        std::ifstream file;
        file.open(fs::current_path().generic_string() + "/resources/images/skybox.data");
        std::stringstream s;
        s<< file.rdbuf();
        bo.loadFromStream(s);
        vao.glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3,0);      
    });
    stbi_set_flip_vertically_on_load(true);
    VTO cubetexture =loadTexture(fs::current_path().generic_string()+"/resources/images/container.png");
    
    std::vector<std::string> faces
    {
        fs::current_path().generic_string()+"/resources/images/skybox/right.jpg",
        fs::current_path().generic_string()+"/resources/images/skybox/left.jpg",
        fs::current_path().generic_string()+"/resources/images/skybox/top.jpg",
        fs::current_path().generic_string()+"/resources/images/skybox/bottom.jpg",
        fs::current_path().generic_string()+"/resources/images/skybox/front.jpg",
        fs::current_path().generic_string()+"/resources/images/skybox/back.jpg"
    };
    stbi_set_flip_vertically_on_load(false);
    VTO cubemapTexture =VTO::fromCubemap(faces);
    shader.use();
    //shader.setUniform("texture1", 0);
    shader.setUniform("skybox", 0);
    skyboxShader.use();
    skyboxShader.setUniform("skybox", 0);
    float lastFrame;
     
    win.setRenderCallback([&]() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // draw scene as normal
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model,glm::radians(currentFrame*30),glm::vec3(0,1,1));
        glm::mat4 view = win.cam.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width / win.height, 0.1f, 100.0f);
        shader.setUniformMatrix4("model",1, GL_FALSE,glm::value_ptr(model));
        shader.setUniformMatrix4("view",1, GL_FALSE, glm::value_ptr(view));
        shader.setUniformMatrix4("projection",1, GL_FALSE, glm::value_ptr(projection));
        shader.setUniformv("cameraPos",win.cam.Position);
        // cubes
        cubeVAO.bind().execute([&](auto& ao){
            //auto to =cubetexture.glActiveTexture(0);
            auto to = cubemapTexture.glActiveTexture(0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        });
       

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(win.cam.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
        skyboxShader.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
        // skybox cube
        skyboxVAO.bind().execute([&](auto& ao){
            auto to=cubemapTexture.glActiveTexture(0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        });
        glDepthFunc(GL_LESS); // set depth function back to default
     });
  
    win.exec();
    glfwTerminate();
    return 0;
}




