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
namespace fs = std::filesystem;
template<typename T>
auto& operator<<(T& os, const glm::vec3& vec) {
    os << "Vector " << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}\n";
    return os;
}
int main(int argc, char* argv[])
{
    
    //Debug debug;
    CamWindow win(SCR_WIDTH, SCR_HEIGHT);
   
    
    auto on_failure = [](const std::error_code& code,auto log) {
        std::cout << code.message() << log << std::endl;
        
    };
    auto apppath = std::filesystem::current_path().generic_string();
    auto openfile = [&](auto arg) {
        std::ifstream  f;
        f.open(apppath + arg);
        return f;
    };
    
    GProgramme screenShaders[] = {
        make_programme(make_shader(openfile("/resources/images/gsexample.vs"), GL_VERTEX_SHADER),
                       make_shader(openfile("/resources/images/gsexample.fs"), GL_FRAGMENT_SHADER))(on_failure),
        
    };
   
    stbi_set_flip_vertically_on_load(true);
    Model ourModel(apppath+"/resources/images/backpack.obj");
   
    
  
    float lastFrame;
    win.cam.setPosition(0, 0, 8);
   
    win.setRenderCallback([&]() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_DEPTH_TEST);
        glm::mat4 view = win.cam.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width / win.height, 0.1f, 100.0f);
       
        ourModel.draw(screenShaders[0], glm::rotate(glm::mat4(1), glm::radians(currentFrame * 15), glm::vec3(0, 1, 0)), view, projection);
        
        ourModel.drawNormals(glm::rotate(glm::mat4(1), glm::radians(currentFrame * 15), glm::vec3(0, 1, 0)),view,projection);
       

        
       

     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



