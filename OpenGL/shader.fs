#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D skin1;
uniform sampler2D skin2;

void main()
{
    FragColor = mix(texture(skin1, TexCoord), texture(skin2, TexCoord), 0.2);
}