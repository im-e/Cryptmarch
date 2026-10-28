#pragma once
#include "Vector.h"
#include "Entities/Enemies/Enemy.h"

class Targeting
{
public:

	static int findHighestHPEnemies(Vector<Enemy*>& enemies, Vector2f centre, float range, Enemy* board[], int boardSize) 
	{
		int count = 0; //how many slots in the leaderboard are filled so far

		for (int i = 0; i < enemies.getSize(); i++) //i is which enemy in the list im looking at
		{
			Enemy* enemy = enemies[i]; //get current enemy
			int position = 0; //which leaderboard slot the new enemy is currently “standing in” while it shuffles upward

			//skip anything that shouldnt be targetted
			if (enemy->isDead()) //if enemy is dead
			{
				continue; //skip to the next element
			}
			//if enemy is in range
			if (!isInRange(enemy, centre, range)) 
			{
				continue; //too far away
			}

			//work out where to start inserting
			if (count < boardSize) //board isnt full yet
			{
				position = count;
				count++;
			}
			else //board is full
			{ 
				//compare the current enemy against the weakest enemy on the board (the last slot)
				if (enemy->getHealth() <= board[boardSize - 1]->getHealth())
				{
					continue; //if its weaker, skip
				}
				position = boardSize - 1; //stronger, so start in the last slot (overwrites the weakest)
			}
			
			//slide weaker enemies down to make room
			//keep moving up while we're not at the top and the enemy in the slot above has less health
			while (position > 0 && board[position - 1]->getHealth() < enemy->getHealth())
			{
				board[position] = board[position - 1]; //move weaker one down one slot
				position--; //step up one slot towards the top
			}

			board[position] = enemy; //place the current enemy in the slot where it stopped

		}

		return count;
	}

	static bool isInRange(Enemy* enemy, Vector2f centre, float range)
	{
		Vector2f enemyCentre = enemy->getCentrePoint(); //get the centre point of the enemy
		float distanceX = enemyCentre.x - centre.x; //compare the distances
		float distanceY = enemyCentre.y - centre.y;
		float distance = distanceX * distanceX + distanceY * distanceY; 

		return (distance <= range * range);
	}


};