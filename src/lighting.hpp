
struct metrials
{
    const char* name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};
constexpr std::array<metrials,25> Meterials={
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
template <typename Shader>
inline auto setMeterial(const std::string& targetname, const std::string& meterianame, Shader& pgm)
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
    float qaundratic;

   
};
constexpr std::array<AttenuationCoeffs,13> LightDistanceCoeffs ={
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
template <typename Shader>
inline void setAttenuations(const std::string& targetname,unsigned  maxdistance, Shader& pgm) {
    auto iter = std::lower_bound(begin(LightDistanceCoeffs), end(LightDistanceCoeffs), maxdistance, [](const auto& mid, auto& val) {return mid.distance < val; });
    if(iter != end(LightDistanceCoeffs)){
        pgm.setUniform(targetname + ".constant", iter->constant);
        pgm.setUniform(targetname + ".linear", iter->linear);
        pgm.setUniform(targetname + ".quadratic", iter->qaundratic);
    }
    
    
}

