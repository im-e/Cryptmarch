#pragma once
struct AABB {
    float x, y;          
    int width, height; 
};

class Collisions
{
public:
    inline static bool checkAABBCollision(const AABB& a, const AABB& b)
    {
        //check for overlap on both axes
        //x axis: a's left edge is left of b's right edge && a's right edge is right of b's left edge
        bool xOverlap = (a.x < b.x + b.width) && (a.x + a.width > b.x);
        //y axis: a's top edge is above b's bottom edge && a's bottom edge is below of b's top edge
        bool yOverlap = (a.y < b.y + b.height) && (a.y + a.height > b.y);

        //you can easily have horizontal overlap without a collision EG: they are at the same x position but with completely different y levels
        //so its importatnt that there is a 'collision' on both axis
        return xOverlap && yOverlap;
    }
};