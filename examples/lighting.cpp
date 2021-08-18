
#include "camwindow.h"
#include <fstream>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include "lighting.hpp"
#include "basic_types.hpp"
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
  
   
    CamWindow win(SCR_WIDTH, SCR_HEIGHT);
   
    const char* vshader = R"(
                        #version 330 core
                        layout (location = 0) in vec3 aPos;
                        layout (location = 1) in vec3 aNormal;
                        layout (location = 2) in vec2 atextcord;

                        uniform mat4 model;
                        uniform mat4 view;
                        uniform mat4 projection;
                        out vec3 onorm;
                        out vec3 Fragpos;
                        out vec2 TextCord;
                        void main()
                        {
                            gl_Position = projection * view * model * vec4(aPos, 1.0);
                            Fragpos=vec3(model*vec4(aPos,1));
                            onorm=mat3(inverse(transpose(model)))*aNormal;
                            TextCord=atextcord;
                        } 
                        )";

    const char* objfshader = R"(
                    
                        #version 330 core
                        struct Meterial{
                            sampler2D  diffuse;
                            sampler2D specular;
                            sampler2D emission;
                            float shininess;
                        };
                        struct Light{
                            vec3 ambient;
                            vec3 diffuse;
                            vec3 specular;
                            
                            float constant;
                            float linear;
                            float qaundratic;

                            vec3 position;
                            vec3 direction;
                            float innercutoff;
                            float outercutoff;
                        };
                        out vec4 FragColor;
                        
                        uniform Light light;
                        uniform Meterial meterial;
                        uniform vec3 campos;
                       
                        in vec3 onorm;
                        in vec3 Fragpos;
                        in vec2 TextCord;
                        void main()
                        {
                            vec3 ambientcolor = light.ambient * vec3(texture(meterial.diffuse,TextCord));
                            vec3 Normal = normalize(onorm);
                            vec3 lightdir=normalize(light.position-Fragpos);
                            vec3 diffusecolor =max(dot(Normal,lightdir),0)*light.diffuse * vec3(texture(meterial.diffuse,TextCord));
                            vec3 re = reflect(-lightdir,Normal);
                            vec3 eyedir = normalize(campos-Fragpos);
                            float spec= pow(max(dot(eyedir,re),0),meterial.shininess);
                            vec3 specular= spec*light.specular*vec3(texture(meterial.specular,TextCord));
                            vec3 emission=vec3(texture(meterial.emission,TextCord));
                            float lengthformlight = length(light.position-Fragpos);
                            float attenuation = 1/(light.constant+light.linear * lengthformlight + light.qaundratic*pow(lengthformlight,2)); 

                            vec3 fragdir = normalize(light.position-Fragpos);
                            float theta = dot(fragdir,light.direction);
                            float epsilone = light.innercutoff-light.outercutoff;
                            float intensity = clamp((theta-light.outercutoff)/epsilone ,0,1);
                            FragColor = vec4((ambientcolor+diffusecolor+specular)*attenuation * intensity, 1.0);
                            
                        }
                    )";
    const char* lsfshader = R"(
                                #version 330 core
                                struct Light{
                                    vec3 ambient;
                                    vec3 diffuse;
                                    vec3 specular;
                                    
                                    vec3 position;
                                    vec3 direction;
                                };
                                out vec4 FragColor;
                                uniform Light light;
                                void main()
                                {
                                   FragColor = vec4(light.ambient,1.0f);
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
    win.setKeyCallback(decorate(win.keycallback,[&](int key, int scancode, int action, int mods) {
        
            if (key == GLFW_KEY_L && action == GLFW_PRESS) {
                lightintesity+=lightintesity;
                if(lightintesity > std::numeric_limits< unsigned int>::max()) lightintesity=std::numeric_limits< unsigned int>::max();
            }
            if (key == GLFW_KEY_K && action == GLFW_PRESS) {
                lightintesity-=(lightintesity>>2);
                if (lightintesity < 0) lightintesity = 1;
            }
        }));
    
    auto lightmeterial = "gold";
    win.setRenderCallback([&]() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        
        std::array<glm::vec3, 5> cubes = {
            glm::vec3{0.,0.,-1.},
            glm::vec3{1.,0.,-1.},
            glm::vec3{2.,2.,-3.},
            glm::vec3{3.,0.,-2.},
            

        };
        
      
       
                    //light = light * (float)glfwGetTime();
        auto lightsourcemodel = glm::mat4(1.0f);
        lightsourcemodel = glm::rotate(lightsourcemodel, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.f, 0.f, 0.f));
        lightsourcemodel = glm::translate(lightsourcemodel, glm::vec3{ -5.,2.,1. });
        auto lightsrc = glm::vec3(lightsourcemodel * glm::vec4(0., 0., 0., 1));
        glm::mat4 projection = glm::perspective(glm::radians(win.cam.Zoom), win.width /win.height, 0.1f, 100.0f);
        glm::mat4 view = win.cam.GetViewMatrix();

        

        

        vao.bind(0).execute([&] {
            
                shaderProgram.use();
                shaderProgram.setUniform("meterial.diffuse", 0);
                shaderProgram.setUniform("meterial.specular", 1);
                shaderProgram.setUniform("meterial.emission", 2);
                setMeterial("meterial", "gold", shaderProgram);
                setMeterial("light", lightmeterial, shaderProgram);
                setAttenuations("light", lightintesity, shaderProgram);
                shaderProgram.setUniform3v("light.position", 1, glm::value_ptr(lightsrc));
                shaderProgram.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
                shaderProgram.setUniform3v("campos", 1, glm::value_ptr(win.cam.Position));
                shaderProgram.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
                auto t1=vto.glActiveTexture(0, GL_TEXTURE0);
                auto t2=vto.glActiveTexture(1, GL_TEXTURE1);    
                auto t3=vto.glActiveTexture(2, GL_TEXTURE2);

                //set light source pointing to one of cube;
                auto lightdir = glm::normalize(lightsrc - cubes[2]);
                shaderProgram.setUniform3v("light.direction", 1, glm::value_ptr(lightdir));
                shaderProgram.setUniform("light.innercutoff", 1, glm::cos(glm::radians(12.5f)));
                shaderProgram.setUniform("light.outercutoff", 1, glm::cos(glm::radians(25.f)));
                
                auto model = glm::mat4(1.0f);
                for (auto m : cubes) {
                    model = glm::translate(model, m);
                    //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(25.0f), glm::vec3(0.f, 1.f, 0.f));
                    model = glm::scale(model, { 2,2,2 });

                    shaderProgram.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                
               
            
            });

        vao.bind(1).execute([&] {
           
            shaderProgram2.use();
            shaderProgram2.setUniformMatrix4("view", 1, GL_FALSE, glm::value_ptr(view));
            shaderProgram2.setUniformMatrix4("projection", 1, GL_FALSE, glm::value_ptr(projection));
            
            shaderProgram2.setUniformMatrix4("model", 1, GL_FALSE, glm::value_ptr(lightsourcemodel));
            setMeterial("light", lightmeterial, shaderProgram2);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            });
        
     });
  
    win.exec();
    glfwTerminate();
    return 0;
}



