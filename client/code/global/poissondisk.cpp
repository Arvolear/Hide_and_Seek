#include "globaluse.hpp"
#include "poissondisk.hpp"

PoissonDisk::PoissonDisk()
{
    this->height = this->width = 0;

    this->radius = 0.0;

    this->sampleSize = 0.0;
    this->gridHeight = this->gridWidth = 0;
}
        
float PoissonDisk::distance(const GridInfo& GI0, const GridInfo& GI1)
{
    return sqrt((GI0.x - GI1.x) * (GI0.x - GI1.x) + (GI0.y - GI1.y) * (GI0.y - GI1.y));
}
        
void PoissonDisk::addSample(const GridInfo& GI)
{
    active.push_back(GI);

    grid[round(GI.y / sampleSize)][round(GI.x / sampleSize)] = GI; 
}

PoissonDisk::GridInfo PoissonDisk::generateAround(const GridInfo& GI)
{
    float angle = global.getRandomNumber() * 2 * 3.14159265;

    float newRadius = global.getRandomNumber() * radius + radius;

    float newX = GI.x + newRadius * cos(angle);
    float newY = GI.y + newRadius * sin(angle);

    return GridInfo(newX, newY);
}
        
bool PoissonDisk::withinExtent(const GridInfo& GI)
{
    if (GI.x < radius || GI.x > float(width) - radius)
    {
        return false;
    }
    
    if (GI.y < radius || GI.y > float(height) - radius)
    {
        return false;
    }

    return true;
}
        
bool PoissonDisk::near(const GridInfo& GI)
{
    int gridX = round(GI.x / sampleSize);
    int gridY = round(GI.y / sampleSize);
    
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
                GridInfo chosen = grid[i][j];

                if (distance(chosen, GI) < radius)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
        
void PoissonDisk::setRadius(float radius)
{
    this->radius = radius;
}

void PoissonDisk::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void PoissonDisk::setSize(vec2 size)
{
    setSize(size.x, size.y);
}

void PoissonDisk::generate()
{
    clear();

    this->sampleSize = radius / sqrt(2);

    this->gridHeight = ceil(height / sampleSize);
    this->gridWidth = ceil(width / sampleSize);

    grid.resize(gridHeight, vector < GridInfo >(gridWidth, GridInfo()));

    addSample(GridInfo(width / 2.0, height / 2.0));

    while (true)
    {
        unsigned int index = round(global.getRandomNumber() * (active.size() - 1));

        GridInfo chosen = active[index];

        bool fine = false;

        for (size_t i = 0; i < 100; i++)
        {
            GridInfo newGI = generateAround(chosen);

            if (withinExtent(newGI) && !near(newGI))
            {
                addSample(newGI); 
                fine = true;
                break;
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
        
vector < PoissonDisk::GridInfo > PoissonDisk::getDisk() const
{
    vector < GridInfo > res;

    for (int i = 0; i < gridHeight; i++)
    {
        for (int j = 0; j < gridWidth; j++)
        {
            if (grid[i][j].x != -1 && grid[i][j].y != -1)
            {
                res.push_back(grid[i][j]);
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
