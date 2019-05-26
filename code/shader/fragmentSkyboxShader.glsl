#version 330 core

in vec3 textureCoords;

uniform samplerCube skybox;
//uniform sampler2D skybox;

out vec4 color;

void main()
{
    color = texture(skybox, textureCoords);    
}
