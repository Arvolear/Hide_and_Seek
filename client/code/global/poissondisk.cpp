#include "globaluse.hpp"
#include "poissondisk.hpp"

PoissonDisk::PoissonDisk()
{
    this->height = this->width = 0;

    this->radius = 0.0;

    this->sampleSize = 0.0;
    this->gridHeight = this->gridWidth = 0;
}
        
void PoissonDisk::addSample(vec2 sample)
{
    active.push_back(sample);

    grid[round(sample.y / sampleSize)][round(sample.x / sampleSize)] = sample; 
}

vec2 PoissonDisk::generateAround(vec2 sample)
{
    float angle = global.getRandomNumber() * 2 * 3.14159265;

    float newRadius = global.getRandomNumber() * radius + radius;

    float newX = sample.x + newRadius * cos(angle);
    float newY = sample.y + newRadius * sin(angle);

    return vec2(newX, newY);
}
        
bool PoissonDisk::withinExtent(vec2 sample)
{
    if (sample.x < radius || sample.x > float(width) - radius)
    {
        return false;
    }
    
    if (sample.y < radius || sample.y > float(height) - radius)
    {
        return false;
    }

    return true;
}
        
bool PoissonDisk::near(vec2 sample)
{
    int gridX = round(sample.x / sampleSize);
    int gridY = round(sample.y / sampleSize);
    
    int checkBox = 2;

    int x0 = std::max(int(gridX - checkBox), 0);
    int y0 = std::max(int(gridY - checkBox), 0);
    int x1 = std::min(gridX + checkBox + 1, gridWidth);
    int y1 = std::min(gridY + checkBox + 1, gridHeight);

    for (int i = y0; i < y1; i++)
    {
        for (int j = x0; j < x1; j++)
        {
            if (grid[i][j].x != -1 && grid[i][j].y != -1)
            {
                vec2 chosen = grid[i][j];

                if (distance(chosen, sample) < radius)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
        
bool PoissonDisk::onSegment(vec2 p0, vec2 q, vec2 p1)
{
    if (q.x <= std::max(p0.x, p1.x) && q.x >= std::min(p0.x, p1.x) && q.y <= std::max(p0.y, p1.y) && q.y >= std::min(p0.y, p1.y))
    {
        return true;
    }

    return false;
}

int PoissonDisk::orientation(vec2 p0, vec2 p1, vec2 p2)
{
    float val = (p1.y - p0.y) * (p2.x - p1.x) - (p1.x - p0.x) * (p2.y - p1.y);

    if (abs(val) - 0.0001 < 0)
    {
        return 0;
    }

    return val > 0 ? 1 : 2;
}

bool PoissonDisk::intersects(vec2 p0, vec2 q0, vec2 p1, vec2 q1)
{
    int o0 = orientation(p0, q0, p1);
    int o1 = orientation(p0, q0, q1);
    int o2 = orientation(p1, q1, p0);
    int o3 = orientation(p1, q1, q0);

    if (o0 != o1 && o2 != o3)
    {
        return true;
    }

    if (!o0 && onSegment(p0, p1, q0))
    {
        return true;
    }

    if (!o1 && onSegment(p0, q1, q0))
    {
        return true;
    }
    
    if (!o2 && onSegment(p1, p0, q1))
    {
        return true;
    }
    
    if (!o3 && onSegment(p1, q0, q1))
    {
        return true;
    }

    return false;
}
        
bool PoissonDisk::inside(vector < vec2 > polygon, vec2 p)
{
    if (polygon.size() < 3)
    {
        return false;
    }
    
    float xMin = 9999.0, yMin = 9999.0; 
    float xMax = -9999.0, yMax = -9999.0;

    for (size_t i = 0; i < polygon.size(); i++)
    {
        vec2 curPoint = polygon[i] - leftTop;

        if (curPoint.x < xMin)
        {
            xMin = curPoint.x;
        }
        
        if (curPoint.x > xMax)
        {
            xMax = curPoint.x;
        }
        
        if (curPoint.y < yMin)
        {
            yMin = curPoint.y;
        }
        
        if (curPoint.y > yMax)
        {
            yMax = curPoint.y;
        }
    }

    if (p.x < xMin || p.x > xMax || p.y < yMin || p.y > yMax)
    {
        return false;
    }

    vec2 extreme = vec2(-9999.0, p.y);

    int counter = 0;
    for (size_t i = 0; i < polygon.size(); i++)
    {
        int cur = i % polygon.size();
        int next = (i + 1) % polygon.size();

        vec2 curPoint = polygon[cur] - leftTop;
        vec2 nextPoint = polygon[next] - leftTop;

        if (intersects(curPoint, nextPoint, p, extreme))
        {
            /* collinear */
            if (!orientation(curPoint, p, nextPoint))
            {
                return onSegment(curPoint, p, nextPoint);
            }

            counter++;
        }
    }

    if (counter % 2 == 1)
    {
        return true;
    }

    return false;
}
        
void PoissonDisk::setRadius(float radius)
{
    this->radius = radius;
}

void PoissonDisk::setBorders(vec2 leftTop, vec2 rightBottom)
{
    this->leftTop = leftTop;
    this->rightBottom = rightBottom;

    this->width = abs(rightBottom.x - leftTop.x);
    this->height = abs(leftTop.y - rightBottom.y);

    initSamples.push_back(vec2(0, 0));
    initSamples.push_back(vec2(width - 1, 0));
    initSamples.push_back(vec2(width / 2.0, height / 2.0));
    initSamples.push_back(vec2(0, height - 1));
    initSamples.push_back(vec2(width - 1, height - 1));
}
        
void PoissonDisk::setWithoutPolygons(vector < vector < vec2 > > &without)
{
    this->without = without;
}

void PoissonDisk::generate()
{
    clear();

    this->sampleSize = radius / sqrt(2);

    this->gridHeight = ceil(height / sampleSize);
    this->gridWidth = ceil(width / sampleSize);

    grid.resize(gridHeight, vector < vec2 >(gridWidth, vec2(-1.0)));

    for (size_t i = 0; i < initSamples.size(); i++)
    {
        /* init inside polygons? */
        bool in = false;
        for (size_t j = 0; j < without.size(); j++)
        {
            if (inside(without[j], initSamples[i]))
            {
                in = true;
                break;
            }
        }

        if (!in)
        {
            addSample(initSamples[i]);
        }
    }

    while (true)
    {
        unsigned int index = round(global.getRandomNumber() * (active.size() - 1));

        vec2 chosen = active[index];

        bool fine = false;

        for (size_t i = 0; i < 100; i++)
        {
            vec2 newSample = generateAround(chosen);

            if (withinExtent(newSample) && !near(newSample))
            {
                /* inside polygons? */
                bool in = false;
                for (size_t j = 0; j < without.size(); j++)
                {
                    if (inside(without[j], newSample))
                    {
                        in = true;
                        break;
                    }
                }

                if (!in)
                {
                    addSample(newSample); 
                    fine = true;
                    break;
                }
            }
        }

        if (!fine)
        {
            swap(active[index], active[active.size() - 1]);
            active.pop_back();
        }

        if (active.empty())
        {
            break;
        }
    }
}

vector < vec2 > PoissonDisk::getDisk() const
{
    vector < vec2 > res;

    for (int i = 0; i < gridHeight; i++)
    {
        for (int j = 0; j < gridWidth; j++)
        {
            if (grid[i][j].x != -1 && grid[i][j].y != -1)
            {
                res.push_back(grid[i][j]);

                res[res.size() - 1].x += leftTop.x;
                res[res.size() - 1].y += leftTop.y;
            }
        }
    }

    return move(res);
}

void PoissonDisk::clear()
{   
    grid.clear();
    active.clear();
}

PoissonDisk::~PoissonDisk() {}
