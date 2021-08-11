#version 330 core
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