
#include "gl.hpp"
#include "camera.hpp"
#include "gwindow.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl;
struct CamWindow:public GWindow
{

    CamWindow(int wid,int height):GWindow(wid,height){
        setResizeCallback([](int w, int h) {
            glViewport(0, 0, w, h);
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
        
        setMouseCallback([&](double xpos,double ypos) {
            
            
            if (!firsttime) {
                cam.ProcessMouseMovement(xpos - prevx, prevy - ypos);
            }
            prevx = xpos;
            prevy = ypos;
            firsttime = false;
                
            });
    }
    bool firsttime {true};
    double prevx{0};
    double prevy{0};
    Camera cam;
};



