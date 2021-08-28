
#include "gl.hpp"
#include "gwindow.hpp"
#include <fstream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
using namespace gl;
int main(int argc, char* argv[])
{
  
    
    GWindow win(SCR_WIDTH, SCR_HEIGHT);
   
    const char* vsource = R"(#version 330 core
                                        layout (location = 0) in vec3 aPos;
                                        layout (location = 1) in vec3 acolor;
                                        layout (location = 2) in vec2 texCord;
                                        out vec3 fcolor;
                                        out vec2 atextcord;
                                        void main()
                                        {
                                           gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
                                           fcolor=acolor;
                                           atextcord=texCord;
                                        }
                                    )";

    const char* fsource = R"(#version 330 core
                                out vec4 FragColor;
                                uniform vec4 acolor;
                                uniform sampler2D  ourTexture;
                                in vec2 atextcord;
                                void main()
                                {
                                   FragColor = texture(ourTexture,atextcord);
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
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open("./vertex.v");
        fShaderFile.open("./fragment.f");
        

    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }
    
    auto shaderProgram = make_programme(make_shader(vShaderFile, GL_VERTEX_SHADER), make_shader(fShaderFile, GL_FRAGMENT_SHADER))(on_failure);
    auto shaderProgram2 = make_programme(make_shader(vsource, GL_VERTEX_SHADER), make_shader(fsource2, GL_FRAGMENT_SHADER))(on_failure);
  
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // add a new set of vertices to form a second triangle (a total of 6 vertices); the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
    std::array<float,44> vertices= {
        0.5f,  0.5f, 0.0f,  0.5f,0.6f,0.1f,1.0f,1.0f,// top right
         0.5f, -0.5f, 0.0f,  0.1f,0.9f,0.1f,1.0f,0.0f,// bottom right
        -0.5f, -0.5f, 0.0f,  0.5f,0.6f,0.8f,.0f,.0f,// bottom left
        -0.5f,  0.5f, 0.0f,   0.8f,0.6f,0.1f,.0f,1.0f,// top left 
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
        t.glTexImage2D(0,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE, "./hand.jpg");

        auto t2 = vto.bind(1);
        t2.glTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
        t2.glTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
        t2.glTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        t2.glTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t2.glTexImage2D(0,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE,"./nature.jpg");

        b.glBufferData(vertices, GL_STATIC_DRAW);
        
        auto e = ebo.bind(0);
        e.glBufferData(indices, GL_STATIC_DRAW);
        v.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 , 0);
        v.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8, 6);

        auto v2 = vao.bind(1);
        auto b2 = vbo.bind(1);
        b.glBufferData(vertices, GL_STATIC_DRAW);

        auto e2 = ebo.bind(1);
        e2.glBufferData(indices2, GL_STATIC_DRAW);
        v2.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 ,0);
        v2.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 , 3);
   
    }
    win.setResizeCallback([](int w, int h) {
            glViewport(0, 0, w, h);
        });

    win.setRenderCallback([&]() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);




        vao.bind(0).execute([&](auto&) {
            ebo.bind(0).execute([&](auto&) {
                glActiveTexture(GL_TEXTURE0);
                auto t = vto.bind(0);
                glActiveTexture(GL_TEXTURE1);
                auto t2 = vto.bind(1);
                shaderProgram.use();
                shaderProgram.setUniform("acolor", 1.f, .1f,1.f,0.6f);
                shaderProgram.setUniform("texture1", 0);
                shaderProgram.setUniform("texture2", 1);
                //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
                });
            });


        
        vao.bind(1).execute([&](auto&) {
            ebo.bind(1).execute([&](auto&) {
                shaderProgram2.use();
                //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
                });
            
        });
        
       
        
     });
    win.setKeyCallback([&](int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_E && action == GLFW_PRESS)
            glfwSetWindowShouldClose(win.window(), true);
    });
    win.exec();
    glfwTerminate();
    return 0;
}



