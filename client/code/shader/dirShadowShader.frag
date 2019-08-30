#version 330 core

in vec4 fragmentPos;

out vec4 fragmentColor;

void main()
{
    float depth = fragmentPos.z / fragmentPos.w;
    depth = depth * 0.5 + 0.5;

    float esmFactor = 80.0;
    fragmentColor = vec4(depth, esmFactor * depth, 0.0, 0.0);
}
