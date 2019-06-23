#include "../shader/shader.hpp"
#include "debugsphere.hpp"

DebugSphere::DebugSphere()
{
    transform = mat4(1);
}

void DebugSphere::setUpSphere()
{
    glGenVertexArrays(1, &VAO); //generate VAO
    glGenBuffers(1, &VBO); //generate VBO

    glBindVertexArray(VAO); //bind VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(DebugVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW); //insert vertex data into VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)0);
    glEnableVertexAttribArray(0); //0 layout for position
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));
    glEnableVertexAttribArray(1); //1 layout for color

    glBindVertexArray(0); //unbind 
}

void DebugSphere::constructOcta(vec3 center, double radius)
{
    double dist = sqrt(radius * radius / 2.0);

    vec3 pos;
    vec3 color = vec3(0.1, 0.1, 0.1);
    
    //first up right
    pos = vec3(center.x + dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x + dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y + radius, center.z);
    vertices.push_back({pos, color});
    
    //second up front
    pos = vec3(center.x + dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x - dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y + radius, center.z);
    vertices.push_back({pos, color});
    
    //thrird up left
    pos = vec3(center.x - dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x - dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y + radius, center.z);
    vertices.push_back({pos, color});

    //fourth up back
    pos = vec3(center.x - dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x + dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y + radius, center.z);
    vertices.push_back({pos, color});
    
    //fifth down right
    pos = vec3(center.x + dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x + dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y - radius, center.z);
    vertices.push_back({pos, color});
    
    //sixth down front
    pos = vec3(center.x - dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x + dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y - radius, center.z);
    vertices.push_back({pos, color});
    
    //seventh down left
    pos = vec3(center.x - dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x - dist, center.y, center.z - dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y - radius, center.z);
    vertices.push_back({pos, color});
    
    //eights down back
    pos = vec3(center.x + dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x - dist, center.y, center.z + dist);
    vertices.push_back({pos, color});
    
    pos = vec3(center.x, center.y - radius, center.z);
    vertices.push_back({pos, color});
}
        
void DebugSphere::constructSphere(vector < DebugVertex > curVertices, int curDepth, int maxDepth)
{
    vector < DebugVertex > localVertices;

    if (curDepth == maxDepth)
    {
        vertices = curVertices;
        return;
    }

    for (size_t i = 0; i < curVertices.size(); i += 3)
    {
        vec3 pos;
        vec3 color;

        //first vertex
        localVertices.push_back(curVertices[i]);

        //second vertex
        pos = (curVertices[i].position + curVertices[i + 1].position) / float(2.0);
        color = vec3(0.3, 0.0, 0.0);
        localVertices.push_back({pos, color});

        //third vertex
        pos = (curVertices[i].position + curVertices[i + 2].position) / float(2.0);
        color = vec3(0.5, 0.3, 0.0);
        localVertices.push_back({pos, color});

        //fourth vertex
        localVertices.push_back(localVertices[localVertices.size() - 2]);

        //fifth vertex
        localVertices.push_back(curVertices[i + 1]);

        //sixth vertex
        pos = (curVertices[i + 1].position + curVertices[i + 2].position) / float(2.0);
        color = vec3(0.0, 0.5, 0.3);
        localVertices.push_back({pos, color});

        //seventh vertex
        localVertices.push_back(localVertices[localVertices.size() - 4]);

        //eighth vertex
        localVertices.push_back(localVertices[localVertices.size() - 2]);

        //nineth vertex
        localVertices.push_back(curVertices[i + 2]);

        //tenth vertex
        localVertices.push_back(localVertices[localVertices.size() - 8]);

        //eleventh vertex
        localVertices.push_back(localVertices[localVertices.size() - 5]);

        //twelfth
        localVertices.push_back(localVertices[localVertices.size() - 9]);
    }

    constructSphere(localVertices, ++curDepth, maxDepth);
}

void DebugSphere::roundComputedSphere(vec3 center, double radius)
{
    //calculate points that are the given distance (radius) away from the center
    for (size_t i = 0; i < vertices.size(); i++)
    {
        vec3 diff = vertices[i].position - center;

        diff *= float(radius) / distance(vertices[i].position, center);

        vertices[i].position = center + diff;
    }
}

void DebugSphere::construct(vec3 center, double radius, int depth)
{
    constructOcta(center, radius);
    constructSphere(vertices, 0, depth);
    roundComputedSphere(center, radius);

    setUpSphere();
}

void DebugSphere::applyTransform(mat4 transform)
{
    this->transform = transform;
}

void DebugSphere::render(Shader* shader) const
{
    shader->setMat4("transform", transform);
    shader->setMat4("view", mat4(1.0));
    shader->setMat4("projection", mat4(1.0));

    glBindVertexArray(VAO); //bind VAO
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glBindVertexArray(0); //unbind VAO
}

DebugSphere::~DebugSphere(){}
