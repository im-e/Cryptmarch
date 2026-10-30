#include "Entities/Players/Wizard.h"
#include "Utilities/Targeting.h"
#include <cmath>

void Wizard::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
    drawSprite(window, cam);

    if (novaVisualTimer > 0.0f)
    {
        float progress = 1 - (novaVisualTimer / novaVisualDuration); //create a progress multiplier to have a nice visual for the spell
        float radius = novaRadius * progress; //apply the multiplier to the radius to give it a growing effect
        
        Vector2f middle = getCentrePoint();
        
        Graphics::DrawCircleOutline(window, cam.worldToScreenX(middle.x), cam.worldToScreenY(middle.y), radius, 255, 255, 0);
    }

    if (secondaryVisualTimer > 0.0f)
    {
        Vector2f middle = getCentrePoint();
        
        Graphics::DrawCircleOutline(window, cam.worldToScreenX(middle.x), cam.worldToScreenY(middle.y), selectRange, 0, 255, 255);
    }
  
#ifdef _DEBUG
    AABB hitbox = getHitbox();
    Graphics::DrawRectOutline(window, cam.worldToScreenX(hitbox.x), cam.worldToScreenY(hitbox.y), hitboxWidth, hitboxHeight, 0, 255, 0);
#endif
}

void Wizard::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
    //------------------- ATTACK ----------------------
    //PRIMARY ATTACK --- wave nova
    if (novaCooldownTimer > 0.0f) novaCooldownTimer -= deltaTime; //decrease the cooldown timer
    if (novaVisualTimer > 0.0f) novaVisualTimer -= deltaTime; //decrease the visual duration
    if (secondaryVisualTimer > 0.0f) secondaryVisualTimer -= deltaTime; //decrease the visual duration
    
    //if nova is off cooldown and key is pressed
    if (window.keyPressed(keyPrimaryAttack) && novaCooldownTimer <= 0.0f) 
    {
        novaRequested = true; //request the nova
        novaVisualTimer = novaVisualDuration; //set the visual timer to the visual duration
        novaCooldownTimer = novaCooldown;  //put nova on cooldown
    }

    //SECONDARY 
    if (secondaryCooldownTimer > 0.0f) secondaryCooldownTimer -= deltaTime; //reduce cooldown timer

    bool secondaryPressed = window.keyPressed(keySecondaryAttack); //if key was just pressed mark as down

    if (secondaryPressed && keyWasReleased) //if it was pressed and key was released
    {
        keyWasReleased = false; //this press is used

        if (secondaryCooldownTimer <= 0.0f) //off cooldown
        {
            if (mana >= manaCost) //we have the mana to cast
            {
               selectRequested = true; //select spell requested
               mana -= manaCost; //take away the mana cost
               secondaryCooldownTimer = secondaryCooldown; //put it on cooldown  
               secondaryVisualTimer = secondaryVisualDuration;
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

void Wizard::processAttack(AttackContext& attackContext) {

    if (!novaRequested && !selectRequested) return; //if nova or select not requested return
    
    if (novaRequested)
    {
        Vector2f centre = getCentrePoint(); //find wizard centre
        float radSq = novaRadius * novaRadius; //work out rad squared
        //eg: 96 * 96 = 9216 - is the distance

        for (int i = 0; i < attackContext.enemies.getSize(); i++)
        {
            if (attackContext.enemies[i]->isDead()) continue; //skip dead enemy
            Vector2f enemyCentre = attackContext.enemies[i]->getCentrePoint(); //find centre of enemy
            float distanceX = enemyCentre.x - centre.x; //find the difference of the two points
            float distanceY = enemyCentre.y - centre.y; 
            float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY); //(dx² + dy²)
            //if dx = 60 and dy = 70 dx² + dy² = 8500 which is less than 9216 which means its a hit
            if (distanceSquared <= radSq) //if its less than our radius squared = hit
            {
                attackContext.enemies[i]->takeDamage(novaDamage); //take damage
       
                //figure out direction vector
                float length = sqrt(distanceSquared);
                //normalise to give us a direction vector
                if (length > 0.0f) //if the length is greater than 0
                {
                    distanceX /= length; //divide the components by it 
                    distanceY /= length;
                }
                attackContext.enemies[i]->applyKnockback(distanceX, distanceY, novaKnockbackForce, novaKnockbackDuration);
            }
        }
        novaRequested = false;
    }

    if (selectRequested)
    {
        const int selectTargets = 3; //amount of targets the selection spell will hit

        Enemy* targets[selectTargets]; //create the array thats passed to targeting

        Vector2f centre = getCentrePoint(); //get the centre point

        int found = Targeting::findHighestHPEnemies(attackContext.enemies, centre, selectRange, targets, selectTargets); //returns count 

        if (found == 0) mana += manaCost; //nothing to hit, refund mana

        for (int i = 0; i < found; i++) //for enemies in the count
        {
            targets[i]->takeDamage(selectDamage); //take damage
        }

        selectRequested = false;
    }
    


}