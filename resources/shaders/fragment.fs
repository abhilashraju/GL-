#ifdef GL_ES
#version 330 core
#endif

out vec4 FragColor;
uniform vec4 acolor;
uniform sampler2D texture1;
uniform sampler2D texture2;
in vec2 atextcord;
void main()
{
   FragColor = mix(texture(texture1,atextcord),texture(texture2,atextcord),0.1);
}
