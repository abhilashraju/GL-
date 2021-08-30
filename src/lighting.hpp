#pragma once
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
#include "basic_types.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <regex>
#include <lighting.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
namespace gl {
    struct GProgramme;
   
    struct CommonVertexShader {
        static std::string to_shader_str() {
            return  R"(
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
        }
    };
    struct CommonFragmentShader {
        static std::string to_shader_str(bool model=false) {
           
            return  !model ? R"(
                    
                            #version 330 core
                            #import lights;
                            #import core;
                            out vec4 FragColor;

                            in vec2 TexCoords;
                            in vec3 Normal;
                            in vec3 FragPos;
                            void main()
                            {   
                               FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
                            }
                    )":
                    R"(
                    
                                    #version 330 core
                                    #import lights;
                                    #import core;
                                    out vec4 FragColor;

                                    in vec2 TexCoords;
                                    in vec3 Normal;
                                    in vec3 FragPos;
                                    uniform sampler2D texture_diffuse1;
                                    void main()
                                    {   
                                       
                                        vec3 color=vec3(texture(texture_diffuse1, TexCoords));
                                        FragColor = vec4(color, 1.0);
                                    }
                            )";
        }
    };
    struct metrials
    {
        const char* name;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    };
    constexpr std::array<metrials, 25> Meterials = {
    metrials{"emerald"	            ,{0.0215	,0.1745	        ,0.0215         },	{0.07568	,0.61424	,0.07568	},{0.633	    ,0.727811	,0.633	    },0.6},
    metrials{"jade"	                ,{0.135	    ,0.2225	        ,0.1575         },	{0.54	    ,0.89	    ,0.63	    },{0.316228	    ,0.316228	,0.316228	},0.1},
    metrials{"obsidian"             ,{0.05375   ,0.05	        ,0.06625        },	{0.18275	,0.17	    ,0.22525	},{0.332741	    ,0.328634	,0.346435	},0.3},
    metrials{"pearl"	            ,{0.25	    ,0.20725	    ,0.20725        },	{1	        ,0.829	    ,0.829	    },{0.296648  	,0.296648	,0.296648	},0.088},
    metrials{"ruby"	                ,{0.1745    ,0.01175	    ,0.01175        },	{0.61424	,0.04136	,0.04136	},{0.727811	    ,0.626959	,0.626959	},0.6},
    metrials{"turquoise"	        ,{0.1	    ,0.18725	    ,0.1745         },	{0.396	    ,0.74151	,0.69102	},{0.297254	    ,0.30829	,0.306678	},0.1},
    metrials{"brass"	            ,{0.329412	,0.223529	    ,0.027451       },	{0.780392	,0.568627	,0.113725	},{0.992157	    ,0.941176	,0.807843	},0.21794872},
    metrials{"bronze"	            ,{0.2125	,0.1275	        ,0.054          },	{0.714	    ,0.4284	    ,0.18144	},{0.393548	    ,0.271906	,0.166721	},0.2},
    metrials{"chrome"	            ,{0.25	    ,0.25	        ,0.25           },	{0.4	    ,0.4	    ,0.4	    },{0.774597	    ,0.774597	,0.774597	},0.6},
    metrials{"copper"	            ,{0.19125	,0.0735	        ,0.0225         },	{0.7038	    ,0.27048	,0.0828	    },{0.256777	    ,0.137622	,0.086014	},0.1},
    metrials{"gold"                 ,{0.24725	,0.1995	        ,0.0745         },	{0.75164	,0.60648	,0.22648	},{0.628281	    ,0.555802	,0.366065	},0.4},
    metrials{"silver"	            ,{0.19225	,0.19225	    ,0.19225        },	{0.50754	,0.50754	,0.50754	},{0.508273	    ,0.508273	,0.508273	},0.4},
    metrials{"black plastic"	    ,{0.0	    ,0.0	        ,0.0            },	{0.01	    ,0.01	    ,0.01	    },{0.50	        ,0.50	    ,0.50	    },.25},
    metrials{"cyan plastic"         ,{0.0	    ,0.1	        ,0.06           },	{0.0	    ,0.50980392	,0.50980392	},{0.50196078	,0.50196078	,0.50196078	},.25},
    metrials{"green plastic"	    ,{0.0	    ,0.0	        ,0.0            },	{0.1	    ,0.35	    ,0.1	    },{0.45	        ,0.55	    ,0.45	    },.25},
    metrials{"red plastic"	        ,{0.0	    ,0.0	        ,0.0	        },  { 0.5	    ,0.0	    ,0.0	    },{0.7	        ,0.6	    ,0.6	    },.25},
    metrials{"white plastic"	    ,{0.0	    ,0.0	        ,0.0            },	{0.55	    ,0.55	    ,0.55	    },{0.70	        ,0.70	    ,0.70	    },.25},
    metrials{"yellow plastic"	    ,{0.0	    ,0.0	        ,0.0            },	{0.5	    ,0.5	    ,0.0	    },{0.60	        ,0.60	    ,0.50	    },.25},
    metrials{"black rubber"	        ,{0.02	    ,0.02	        ,0.02           },	{0.01	    ,0.01	    ,0.01	    },{0.4	        ,0.4	    ,0.4	    },.078125},
    metrials{"cyan rubber"	        ,{0.0	    ,0.05	        ,0.05           },	{0.4	    ,0.5	    ,0.5	    },{0.04	        ,0.7	    ,0.7	    },.078125},
    metrials{"green rubber"	        ,{0.0	    ,0.05	        ,0.0            },	{0.4	    ,0.5	    ,0.4	    },{0.04	        ,0.7	    ,0.04	    },.078125},
    metrials{"red rubber"	        ,{0.05	    ,0.0	        ,0.0            },	{0.5	    ,0.4	    ,0.4	    },{0.7	        ,0.04	    ,0.04	    },.078125},
    metrials{"white rubber"	        ,{0.05	    ,0.05	        ,0.05           },	{0.5	    ,0.5	    ,0.5	    },{0.7	        ,0.7	    ,0.7	    },.078125},
    metrials{"yellow rubber"	    ,{0.05	    ,0.05	        ,0.0            },	{0.5	    ,0.5	    ,0.4	    },{0.7	        ,0.7	    ,0.04	    },.078125},
    metrials{"sun"	                ,{1.	    ,1	            ,1              },	{1.	        ,1	        ,1          },{1.	        ,1	        ,1          },.078125},
    };
    template<typename GProgramme>
    inline auto setMeterial(const std::string& targetname, const std::string& meterianame, GProgramme& pgm)
    {
        auto iter = std::find_if(begin(Meterials), end(Meterials), [&](auto& v) {return meterianame == v.name; });
        if (iter != end(Meterials)) {
            pgm.setUniform3v(targetname + ".ambient", 1, glm::value_ptr(iter->ambient));
            pgm.setUniform3v(targetname + ".diffuse", 1, glm::value_ptr(iter->diffuse));
            pgm.setUniform3v(targetname + ".specular", 1, glm::value_ptr(iter->specular));
            pgm.setUniform(targetname + ".shininess", iter->shininess);
        }

    };

    struct AttenuationCoeffs
    {
        unsigned distance;
        float constant;
        float linear;
        float quadratic;


    };
    constexpr int maxattenuationcoeff = 3250;
    constexpr std::array<AttenuationCoeffs, 13> LightDistanceCoeffs = {
        AttenuationCoeffs{0	        ,1.0	,0.7	    ,1.8},
        AttenuationCoeffs{7	        ,1.0	,0.7	    ,1.8},
        AttenuationCoeffs{13	    ,1.0	,0.35	    ,0.44},
        AttenuationCoeffs{20	    ,1.0	,0.22	    ,0.20},
        AttenuationCoeffs{32	    ,1.0	,0.14	    ,0.07},
        AttenuationCoeffs{50	    ,1.0	,0.09	    ,0.032},
        AttenuationCoeffs{65	    ,1.0	,0.07	    ,0.017},
        AttenuationCoeffs{100	    ,1.0	,0.045	    ,0.0075},
        AttenuationCoeffs{160	    ,1.0	,0.027	    ,0.0028},
        AttenuationCoeffs{200	    ,1.0	,0.022	    ,0.0019},
        AttenuationCoeffs{325	    ,1.0	,0.014	    ,0.0007},
        AttenuationCoeffs{600	    ,1.0	,0.007	    ,0.0002},
        AttenuationCoeffs{3250	    ,1.0	,0.0014	    ,0.000007},
    };
    template<typename GProgramme>
    inline void setAttenuations(const std::string& targetname, unsigned  maxdistance, GProgramme& pgm) {
        maxdistance = (maxdistance > maxattenuationcoeff) ? maxattenuationcoeff : maxdistance;
        auto iter = std::lower_bound(begin(LightDistanceCoeffs), end(LightDistanceCoeffs), maxdistance, [](const auto& mid, auto& val) {return mid.distance < val; });
        if (iter != end(LightDistanceCoeffs)) {
            pgm.setUniform(targetname + ".constant", iter->constant);
            pgm.setUniform(targetname + ".linear", iter->linear);
            pgm.setUniform(targetname + ".quadratic", iter->quadratic);
        }


    }
    struct BaseLight {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        void setMaterial(const std::string& meterianame) {
            auto iter = std::find_if(begin(Meterials), end(Meterials), [&](auto& v) {return meterianame == v.name; });
            if (iter != end(Meterials)) {
                ambient = iter->ambient;
                diffuse = iter->diffuse;
                specular = iter->specular;
            }
        }
        template<typename GProgramme>
        void apply(const std::string& name, GProgramme& pgm)const {
            pgm.setUniform3v(name + ".ambient", 1, glm::value_ptr(ambient));
            pgm.setUniform3v(name + ".diffuse", 1, glm::value_ptr(diffuse));
            pgm.setUniform3v(name + ".specular", 1, glm::value_ptr(specular));
        }
    };
    struct DirLight:public BaseLight {
        glm::vec3 direction;

       
        static std::string to_shader_str() {
            return R"(
                struct DirLight {
                vec3 direction;

                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
            };
            LightResults CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,float shininess)
            {
                vec3 lightDir = normalize(-light.direction);
                // diffuse shading
                float diff = max(dot(normal, lightDir), 0.0);
                // specular shading
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                // combine results
                vec3 ambient  = light.ambient;
                vec3 diffuse  = light.diffuse  * diff ;
                vec3 specular = light.specular * spec ;
                LightResults result;
                result.ambient=ambient;
                result.diffuse=diffuse;
                result.specular=specular;
                return result;
            } 
        )";
        }
        std::string name()const {
            return "DirLight";
        }
        template<typename GProgramme>
        void apply(const std::string& name, GProgramme& pgm)const {
            pgm.setUniform3v(name + ".direction",1, glm::value_ptr(direction));
            BaseLight::apply(name, pgm);
        }

         
        
    };

    
    struct PointLight:BaseLight {
    private:
        glm::vec3 pos{ glm::vec3{0,0,0} };

        float constant;
        float linear;
        float quadratic;
        glm::mat4 m{ glm::mat4(1.0f) };
    public:
        PointLight& rotate(float angle, const glm::vec3& axis) {
            m =glm::rotate(m, angle, axis);
            return *this;
        }
        PointLight& translate(const glm::vec3& newpos) {
            m = glm::translate(m, newpos);
            return *this;
        }
        void setPosition(const glm::vec3& p) {  pos=p; }
        glm::vec3 position()const { return glm::vec3(m * glm::vec4(pos, 1)); }
        glm::mat4 model()const { return m; }
        static std::string to_shader_str() {
            return R"(
            struct PointLight {    
                vec3 position;
    
                float constant;
                float linear;
                float quadratic;  

                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
            }; 
            LightResults CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,float shininess)
            {
                vec3 lightDir = normalize(light.position - fragPos);
                // diffuse shading
                float diff = max(dot(normal, lightDir), 0.0);
                // specular shading
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                // attenuation
                float distance    = length(light.position - fragPos);
                float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			                    light.quadratic * (distance * distance));    
                // combine results
                vec3 ambient  = light.ambient;
                vec3 diffuse  = light.diffuse  * diff;
                vec3 specular = light.specular * spec;
                ambient  *= attenuation;
                diffuse  *= attenuation;
                specular *= attenuation;
                LightResults result;
                result.ambient=ambient;
                result.diffuse=diffuse;
                result.specular=specular;
                return result;
            }  

        )";
        }
        std::string name()const {
            return "PointLight";
        }
        template<typename GProgramme>
        void apply(const std::string& name, GProgramme& pgm)const {
            BaseLight::apply(name, pgm);
            
            pgm.setUniform3v(name + ".position", 1, glm::value_ptr(position()));
            pgm.setUniform(name + ".constant", constant);
            pgm.setUniform(name + ".linear", linear);
            pgm.setUniform(name + ".quadratic", quadratic);
        }
       
        void setDistanceRange(unsigned int distance) {
            distance = (distance > maxattenuationcoeff) ? maxattenuationcoeff : distance;
            auto iter = std::lower_bound(begin(LightDistanceCoeffs), end(LightDistanceCoeffs), distance, [](const auto& mid, auto& val) {return mid.distance < val; });
            if (iter != end(LightDistanceCoeffs)) {
                constant = iter->constant;
                linear= iter->linear;
                quadratic=iter->quadratic;
            }
        }

    };
    struct SpotLight:PointLight {
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;
        static std::string to_shader_str() {
               return R"(
                struct SpotLight {
                    vec3 position;
                    vec3 direction;
                    float cutOff;
                    float outerCutOff;

                    float constant;
                    float linear;
                    float quadratic;

                    vec3 ambient;
                    vec3 diffuse;
                    vec3 specular;
                };
                LightResults CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,float shininess)
                {
                    vec3 lightDir = normalize(light.position - fragPos);
                    // diffuse shading
                    float diff = max(dot(normal, lightDir), 0.0);
                    // specular shading
                    vec3 reflectDir = reflect(-lightDir, normal);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                    // attenuation
                    float distance = length(light.position - fragPos);
                    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
                    // spotlight intensity
                    float theta = dot(lightDir, normalize(-light.direction)); 
                    float epsilon = light.cutOff - light.outerCutOff;
                    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
                    // combine results
                    vec3 ambient = light.ambient;
                    vec3 diffuse = light.diffuse * diff;
                    vec3 specular = light.specular * spec;
                    ambient *= attenuation * intensity;
                    diffuse *= attenuation * intensity;
                    specular *= attenuation * intensity;
                    LightResults result;
                    result.ambient=ambient;
                    result.diffuse=diffuse;
                    result.specular=specular;
                    return result;
                }
            )";
        }
        template<typename GProgramme>
        void apply(const std::string& name, GProgramme& pgm)const {
            PointLight::apply(name, pgm);
            pgm.setUniform3v(name + ".direction", 1, glm::value_ptr(direction));
            pgm.setUniform(name + ".cutOff", cutOff);
            pgm.setUniform(name + ".outerCutOff", outerCutOff);
        }
    };
    struct LightResults {
        static std::string to_shader_str() {
            return  R"(

            struct LightResults
                {
                    vec3 ambient;
                    vec3 diffuse;
                    vec3 specular;
                };

             )";
        }
    };
    struct TextuteMaterial {
        static std::string to_shader_str() {
            return  R"(

            struct TextureMeterial {
                sampler2D  diffuse;
                sampler2D specular;
                sampler2D emission;
                float shininess;
            };

            )";
        }
    };
    struct Kernels {
        static std::string to_shader_str() {
            return  R"(
            const float offset = 1.0 / 300.0;  
            vec2[9] get_offsets(){
                vec2 offsets[9] = vec2[](
                                vec2(-offset,  offset), // top-left
                                vec2( 0.0f,    offset), // top-center
                                vec2( offset,  offset), // top-right
                                vec2(-offset,  0.0f),   // center-left
                                vec2( 0.0f,    0.0f),   // center-center
                                vec2( offset,  0.0f),   // center-right
                                vec2(-offset, -offset), // bottom-left
                                vec2( 0.0f,   -offset), // bottom-center
                                vec2( offset, -offset)  // bottom-right    
                            );
                return offsets;
            }            
            vec3 apply_kernal(sampler2D tex,vec2 texCord, float[9] kernel){
                vec3 sampleTex[9];
                vec2[9] offsets=get_offsets();
                for(int i = 0; i < 9; i++)
                {
                    sampleTex[i] = vec3(texture(tex, texCord.st + offsets[i]));
                }
                vec3 col = vec3(0.0);
                for(int i = 0; i < 9; i++)
                    col += sampleTex[i] * kernel[i];
                return col;
                }

                vec3 apply_sharpness(sampler2D tex,vec2 texCord){
                float kernel[9] = float[](
                        -1, -1, -1,
                        -1,  9, -1,
                        -1, -1, -1
                    );
                vec2 offsets[9] = vec2[](
                                vec2(-offset,  offset), // top-left
                                vec2( 0.0f,    offset), // top-center
                                vec2( offset,  offset), // top-right
                                vec2(-offset,  0.0f),   // center-left
                                vec2( 0.0f,    0.0f),   // center-center
                                vec2( offset,  0.0f),   // center-right
                                vec2(-offset, -offset), // bottom-left
                                vec2( 0.0f,   -offset), // bottom-center
                                vec2( offset, -offset)  // bottom-right    
                            );
                return apply_kernal(tex,texCord,kernel);
                }
             
            )";
        }
    };
    struct UtilityFunctions {
        static std::string to_shader_str() {
            return  R"(

            vec3 AdjustNormal(mat4 model ,vec3 normal){
                return normalize(mat3(inverse(transpose(model))) * normal);
            }

            )";
        }
    };
    
    template<typename ArrayView , typename GProgramme>
    inline void apply_from_array(const std::string& name, const ArrayView& view, GProgramme& pgm) {
        for (size_t i = 0; i < view.size(); i++) {
            std::stringstream str;
            str << name << "[" << i << "]";
            view.at(i).apply(str.str(), pgm);
        }
    }
    template<typename... T>
    inline std::string make_shader_defs_from() {
        std::string str;
        str = (T::to_shader_str() + ...);
        return str;
    };
    inline std::string get_light_defs() {
        return make_shader_defs_from<LightResults, TextuteMaterial, DirLight, PointLight, SpotLight>();
    }
    inline std::string get_core_defs() {
        return make_shader_defs_from<UtilityFunctions>(); 
    }
    inline std::string get_kernel_defs() {
        return make_shader_defs_from<Kernels>();
    }
    inline auto replaced_str(const std::string& str, const std::string& first, const std::string& replaced) {
        std::regex ampreg(first);
        return  std::regex_replace(str, ampreg, replaced);
    }
    template <typename Func>
    std::string replace(std::string s, std::regex  reg, Func func) {
        std::smatch match;
        bool stop = false;
        while (!stop && std::regex_search(s, match, reg)) {
            std::string param = match.str();
            std::string formatter = "$`";
            formatter += func(std::string_view(param).substr(8,param.length()-9), stop);
            formatter += "$'";
            s = match.format(
                formatter);
        }
        return s;
    }
    std::string replace_imports(const std::string& str) {
    std::vector<std::string> imported;
        return replace(str, std::regex("#import.*;"), [&](auto v,bool& stop) {
            if (std::find_if(std::begin(imported), std::end(imported), [&](auto val) {return v == val; }) == end(imported)) {
                imported.push_back(std::string(v.data(),v.length()));
                if (v == "lights") {
                  return get_light_defs();
                }
                if (v == "core") {
                    return get_core_defs();
                }
                if (v == "kernels") {
                    return get_kernel_defs();
                }
                if (std::find_if(std::begin(v), std::end(v), [](auto c) {return c == '/'; }) != std::end(v)) {
                    std::ifstream vShaderFile;
                    vShaderFile.open(v);
                    std::stringstream str;
                    str << vShaderFile.rdbuf();
                    return replaced_str(str.str(),"#version 330 core","");
                }
            }
           
            return std::string();
         });

    }
   
   
}
