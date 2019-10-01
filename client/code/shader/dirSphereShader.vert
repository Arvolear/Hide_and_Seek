#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 localTransform;
uniform mat4 view;
uniform mat4 projection;

out vec4 polyColor;

void main()
{
    vec4 pos = localTransform * vec4(position, 1.0);
    
    if (pos.y > 1.0)
    {
        polyColor = vec4(color, 1.0);
    }
    else if (pos.y > 0.0)
    {
        polyColor = vec4(color / 4.0, 1.0);
    }
    else
    {
        polyColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    pos = projection * view * pos;

    gl_Position = pos.xyww;
}
