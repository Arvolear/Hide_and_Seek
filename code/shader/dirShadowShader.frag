#version 330 core

in vec4 fragmentPos;

out vec4 fragmentColor;

void main()
{
    float depth = fragmentPos.z / fragmentPos.w;
    depth = depth * 0.5 + 0.5;

    float moment1 = depth;
    float moment2 = depth * depth;

    /* bias */
    float dx = dFdx(depth);
    float dy = dFdy(depth);

    moment2 += 0.25 * (dx * dx + dy * dy);

    fragmentColor = vec4(moment1, moment2, 0.0, 0.0);
}
