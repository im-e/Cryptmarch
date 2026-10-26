#include "Entities/Players/Archer.h"

void Archer::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
    if (isDashing()) //if dashing
    {
        drawSprite(window, cam, 180, 255, 255, 160); //draw with a strong blue tint
    }
    else { //draw normally

        drawSprite(window, cam);
    }

#ifdef _DEBUG

	AABB hitbox = getHitbox();

	Graphics::DrawRectOutline(window, hitbox.x - cam.getX(), hitbox.y - cam.getY(), hitboxWidth, hitboxHeight, 0, 255, 0);

#endif
}

void Archer::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//------------------- ATTACK ----------------------
	//PRIMARY ATTACK
    if (fireCooldownTimer > 0.0f) fireCooldownTimer -= deltaTime; //decrease the fire timer

    if (window.keyPressed(keyPrimaryAttack) && fireCooldownTimer <= 0.0f) //if fire timer is up and key is pressed
    {
        fireRequested = true;      //toggle fire projectile
        fireCooldownTimer = fireCooldown;  //fire timer = cooldown
    }

    //SECONDARY DASH
    if (dashCooldownTimer > 0.0f) dashCooldownTimer -= deltaTime; //reduce cooldown timer
    if (dashTimer > 0.0f) dashTimer -= deltaTime;

    bool secondaryPressed = window.keyPressed(keySecondaryAttack); //if key was just pressed mark as down

    if (secondaryPressed && keyWasReleased) //if it was pressed and key was released
    {
        if (dashCooldownTimer <= 0.0f) //off cooldown
        {       
            if (mana >= manaCost) //we have the mana to cast
            {
                if (impulseTimer <= 0.0f) //if were not already being moved
                {
                    mana -= manaCost; //take away the mana cost
                    applyImpulse(facingDirX, facingDirY, dashSpeed, dashDuration); //apply dash as a knockback
                    dashCooldownTimer = dashCooldown; //put it on cooldown

                    iFrameTimer = dashDuration; //invulnerable while dashing
                    dashTimer = dashDuration;
                    keyWasReleased = false;
                }
            }
        }
    }
    else if (!secondaryPressed) //if secondary is not pressed
    {
        keyWasReleased = true; //set key was released to true
    }

 
	//---------------------- MOVE ----------------
	move(window, map, deltaTime);

}

void Archer::processAttack(AttackContext& attackContext) {

    if (!fireRequested) return; //fire not requested, do nothing
    attackContext.projectiles.add(fireProjectile());
    fireRequested = false;

}

Projectile Archer::fireProjectile()
{
    //get player's hitbox to spawn the projectile just outside them
    AABB player = getHitbox();

    Projectile proj;
    proj.x = player.x + (player.width / 2.0f); 
    proj.y = player.y + (player.height / 2.0f);
    proj.dirX = facingDirX;
    proj.dirY = facingDirY;
    proj.speed = arrowSpeed;  
    proj.lifetime = arrowLifetime; 
    proj.damage = arrowDamage;
    proj.owner = ProjectileOwner::Player;

    proj.knockbackDuration = attackKnockbackDuration;
    proj.knockbackForce = attackKnockbackForce;

    proj.image = projectileImage;
    proj.tileX = projectileTileX;
    proj.tileY = projectileTileY;

    return proj;

}