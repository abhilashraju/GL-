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
#include "gl.hpp"
#include "camera.hpp"
#include "gwindow.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl;
struct CamWindow:public GWindow
{

    CamWindow(int wid,int ht):GWindow(wid,ht){
        width = wid; height = ht;
        setResizeCallback([&](int w, int h) {
            glViewport(0, 0, w, h);
            width = w; height = h;
        });
    
        setKeyCallback([&](int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_E && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window(), true);

            const float cameraSpeed = 0.05f; // adjust accordingly
            if (key == GLFW_KEY_W && action == GLFW_PRESS)
                cam.ProcessKeyboard(FORWARD, .1);
            if (key == GLFW_KEY_S && action == GLFW_PRESS)
                cam.ProcessKeyboard(BACKWARD, .1);
            if (key == GLFW_KEY_A && action == GLFW_PRESS)
                cam.ProcessKeyboard(LEFT, .1);
            if (key == GLFW_KEY_D  && action == GLFW_PRESS)
                cam.ProcessKeyboard(RIGHT, .1);
        });
        
        setMouseCallback([&](const MouseContext& mc,double xpos,double ypos) {
            
            if (mc.mousebutton == GLFW_MOUSE_BUTTON_LEFT && mc.pressedstate == GLFW_PRESS) {
               
                    cam.ProcessMouseMovement(xpos - prevx, prevy - ypos);
  
            }
            prevx = xpos;
            prevy = ypos;
                
            });
        setScrollCallback([&](double xpos, double ypos) {


            cam.ProcessMouseScroll(ypos);

            });
    }
    bool firsttime {true};
    double prevx{0};
    double prevy{0};
    Camera cam{ glm::vec3(0.0f, 0.0f,17.0f) };
    float width, height;
};



