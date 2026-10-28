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
    float knockbackForce; //force for knockback of the melee attack
    float knockbackDuration; //duration of knockback for the melee attack


    ProjectileOwner owner; //who owns this projectile
    bool dead = false; //flag for deletion

    int width = 8;  //hitbox
    int height = 8;

    int tileSize = 32;
    GamesEngineeringBase::Image* image = nullptr; //image for projectile
    int tileX = 0;  //tilesheet x offset
    int tileY = 0;

    //helper to get the AABB for collisions
    AABB getAABB() const {
        return { x, y, width, height };
    }
};