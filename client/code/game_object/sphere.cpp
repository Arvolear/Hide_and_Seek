#include "../shader/shader.hpp"
#include "sphere.hpp"

Sphere::Sphere()
{
    VAO = VBO = 0;
    localTransform = mat4(1.0);
    color = vec3(0.0);
}

void Sphere::setUpSphere()
{
    glGenVertexArrays(1, &VAO); //generate VAO
    glGenBuffers(1, &VBO); //generate VBO

    glBindVertexArray(VAO); //bind VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW); //insert vertex data into VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0); //0 layout for position
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1); //1 layout for color

    glBindVertexArray(0); //unbind 
}

void Sphere::constructOcta(vec3 center, double radius)
{
    double dist = sqrt(radius * radius / 2.0);

    vec3 pos;
    
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
    
    //third up left
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
        
void Sphere::constructSphere(vector < Vertex > curVertices, int curDepth, int maxDepth)
{
    vector < Vertex > localVertices;

    if (curDepth == maxDepth)
    {
        vertices = curVertices;
        return;
    }

    for (size_t i = 0; i < curVertices.size(); i += 3)
    {
        vec3 pos;

        //first vertex
        localVertices.push_back(curVertices[i]);

        //second vertex
        pos = (curVertices[i].position + curVertices[i + 1].position) / float(2.0);
        localVertices.push_back({pos, color});

        //third vertex
        pos = (curVertices[i].position + curVertices[i + 2].position) / float(2.0);
        localVertices.push_back({pos, color});

        //fourth vertex
        localVertices.push_back(localVertices[localVertices.size() - 2]);

        //fifth vertex
        localVertices.push_back(curVertices[i + 1]);

        //sixth vertex
        pos = (curVertices[i + 1].position + curVertices[i + 2].position) / float(2.0);
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

void Sphere::roundComputedSphere(vec3 center, double radius)
{
    //calculate points that are the given distance (radius) away from the center
    for (size_t i = 0; i < vertices.size(); i++)
    {
        vec3 diff = vertices[i].position - center;

        diff *= float(radius) / distance(vertices[i].position, center);

        vertices[i].position = center + diff;
    }
}

void Sphere::construct(vec3 center, double radius, int depth)
{
    this->center = center;
    this->radius = radius;

    constructOcta(center, radius);
    constructSphere(vertices, 0, depth);
    roundComputedSphere(center, radius);

    setUpSphere();
}
        
void Sphere::construct(int depth)
{
    construct(center, radius, depth);
}
        
void Sphere::setCenter(vec3 center)
{
    if (vertices.empty())
    {
        this->center = center;
        return;
    }

    vec3 translation = center - this->center;

    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= scale(sc);
    localTransform *= translate(translation);
}

void Sphere::setColor(vec3 color)
{
    this->color = color;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].color = color;
    }
}

void Sphere::render(Shader* shader) const
{
    shader->setMat4("localTransform", localTransform);

    glBindVertexArray(VAO); //bind VAO
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glBindVertexArray(0); //unbind VAO
}

vec3 Sphere::getCenter() const
{
    return vec3(localTransform * vec4(center, 1.0));
}

double Sphere::getRadius() const
{
    return radius;
}

Sphere::~Sphere()
{
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
    }
    
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
    }
}
