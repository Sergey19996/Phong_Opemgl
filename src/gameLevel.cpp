#include "gameLevel.h"

#include <sstream> //предоставляет классы для работы со строками как с потоками ввода-вывода.
#include <fstream> // для чтения/записи файлов

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight){
	//clear old data
	this->Bricks.clear();
	//load from file 
	unsigned int tileCode;
	GameLevel level;
	std::string line;
	std::ifstream  fstream(file);
	std::vector<std::vector<unsigned int>> tileData;
	if (fstream){

		while (std::getline(fstream,line)) // read each line from level file
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode) // read each word separated by space
				row.push_back(tileCode);
			tileData.push_back(row);	
		}
		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);

	}

}

void GameLevel::Draw(SpriteRenderer& render)
{
	for (GameObject& tile : this->Bricks)
		if (!tile.Destroyed)
			tile.Draw(render);
}

bool GameLevel::IsCompleted()
{
	for(GameObject& tile : this->Bricks)  // если мы находима хоть один тайл не разрушеный 
		if(!tile.Destroyed && !tile.IsSolid)   // игра не закончится 
	return false;

	return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	//calculate dimesions
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();

	//берём пропорцию от ширины экрана и кол-ва элементов что бы весь уровень поместился на экране (обычно мы сами определяли ширину тайла и делили на экран)
	float unit_Width = levelWidth / static_cast<float>(width);
	float unit_Height = levelHeight / static_cast<float>(height);
	//initialize level tiles based on tileData
	for (unsigned int y = 0; y < height; ++y) {

		for (unsigned int x = 0; x < width; ++x) {

			//check block type from level data (2D level array)
			if (tileData[y][x] == 1) // solid 
			{
				glm::vec2 pos(unit_Width * x, unit_Height * y);
				glm::vec2 size(unit_Width, unit_Height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"),
					glm::vec3(0.8f, 0.8f, 0.7f)); // color
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1){

				glm::vec3 color = glm::vec3(1.0f); // original:white
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_Width * x, unit_Height * y);
				glm::vec2 size(unit_Width, unit_Height);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));

			}

		}

	}


}
