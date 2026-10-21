#include "Entity.h"

bool Entity::loadImage(const std::string& path)
{
	return image.load(path);
}

void Entity::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	int screenX = cam.worldToScreenX(worldX);
	int screenY = cam.worldToScreenY(worldY);

	Graphics::DrawTilemapImage(window, image, tileSize, tileXOffset, tileYOffset, screenX, screenY);

}