#pragma once
#include "Utilities/Collisions.h"

enum class ProjectileOwner { Player, Enemy };

struct Projectile {
    float x; // position
    float y;          
    float dirX; //direction vector
    float dirY; 
    float speed; //movespeed
    float lifetime; //how long the projectile lasts
    int damage; //damage dealt on hit
    ProjectileOwner owner; //who owns this projectile
    bool dead = false; //flag for deletion

    int width = 8.0f;  //hitbox
    int height = 8.0f;

    //helper to get the AABB for collisions
    AABB getAABB() const {
        return { x, y, width, height };
    }
};