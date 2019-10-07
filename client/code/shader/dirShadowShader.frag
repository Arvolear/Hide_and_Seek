#version 330 core

layout (location = 0) out float fragDepth;

in vec4 fragmentPos;

void main() 
{
    fragDepth = fragmentPos.z;
}
