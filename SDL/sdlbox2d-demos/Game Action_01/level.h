#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "TinyXML/tinyxml2.h"

using namespace tinyxml2;

struct Object
{
    int GetPropertyInt(std::string name)//возвращаем номер свойства в нашем списке
    {
        return atoi(properties[name].c_str());
    }
	float GetPropertyFloat(std::string name)
{
	return strtod(properties[name].c_str(), NULL);
}

std::string GetPropertyString(std::string name)//получить имя в виде строки.вроде понятно
{
	return properties[name];
}



	std::string name;//объявили переменную name типа string
	std::string type;//а здесь переменную type типа string
	SDL_Rect rect;//тип Rect с нецелыми значениями
	std::map<std::string, std::string> properties;//создаём ассоциатиный массив. ключ - строковый тип, значение - строковый

	SDL_game sprite;//объявили спрайт
};

struct Layer//слои
{
	int opacity;//непрозрачность слоя
	std::vector<SDL_game> tiles;//закидываем в вектор тайлы
};

class Level//главный класс - уровень
{
public:
	bool LoadFromFile(std::string filename);//возвращает false если не получилось загрузить
	Object GetObject(std::string name);
	std::vector<Object> GetObjects(std::string name);//выдаем объект в наш уровень
	std::vector<Object> GetAllObjects();//выдаем все объекты в наш уровень
	void Draw(SDL_Renderer    *renderer);//рисуем в окно                                  /**  **/
	SDL_Point GetTileSize();//получаем размер тайла

private:
	int width, height, tileWidth, tileHeight;//в tmx файле width height в начале,затем размер тайла
	int firstTileID;//получаем айди первого тайла
	SDL_Rect drawingBounds;//размер части карты которую рисуем
	SDL_Texture* tilesetImage;//текстура карты
	std::vector<Object> objects;//массив типа Объекты, который мы создали
	std::vector<Layer> layers;
};

bool Level::LoadFromFile(std::string filename)//двоеточия-обращение к методам класса вне класса
{
	XMLDocument levelFile;//загружаем файл в TiXmlDocument

	// загружаем XML-карту
	if (levelFile.LoadFile(filename.c_str()) != 0)//если не удалось загрузить карту
	{
		std::cout << "Loading level \"" << filename << "\" failed." << std::endl;//выдаем ошибку
		return false;
	}

	// работаем с контейнером map
	XMLElement *map;
	map = levelFile.FirstChildElement("map");

	// пример карты: <map version="1.0" orientation="orthogonal"
	// width="10" height="10" tilewidth="34" tileheight="34">
	width = atoi(map->Attribute("width"));//извлекаем из нашей карты ее свойства
	height = atoi(map->Attribute("height"));//те свойства, которые задавали при работе в
	tileWidth = atoi(map->Attribute("tilewidth"));//тайлмап редакторе
	tileHeight = atoi(map->Attribute("tileheight"));

	// Берем описание тайлсета и идентификатор первого тайла
	XMLElement *tilesetElement;
	tilesetElement = map->FirstChildElement("tileset");
	firstTileID = atoi(tilesetElement->Attribute("firstgid"));

	// source - путь до картинки в контейнере image
	XMLElement *image;
	image = tilesetElement->FirstChildElement("image");
	std::string imagepath = image -> Attribute("source");

	// пытаемся загрузить тайлсет

     SDL_Surface *loadedImage = 0;

     loadedImage = IMG_Load (imagepath.c_str());   //SDL_LoadBMP
     if (loadedImage){
       tilesetImage = SDL_CreateTextureFromSurface(renderer, loadedImage);
       SDL_FreeSurface(loadedImage);
       }
   else
       std::cout << SDL_GetError() << std::endl;


    int w, h;
    SDL_QueryTexture(tilesetImage, NULL, NULL, &w, &h);

	// получаем количество столбцов и строк тайлсета
	int columns = w / tileWidth;
	int rows = h / tileHeight;

	// вектор из прямоугольников изображений (TextureRect)
	std::vector<SDL_Rect> subRects;

	for (int y = 0; y < rows; y++)
	for (int x = 0; x < columns; x++)
	{
		SDL_Rect rect;
		rect.y = y * tileHeight;
		rect.h = tileHeight;
		rect.x = x * tileWidth;
		rect.w = tileWidth;
		subRects.push_back(rect);
	}

	// работа со слоями
	XMLElement *layerElement;
	layerElement = map->FirstChildElement("layer");
	while (layerElement)
	{
		Layer layer;

		// если присутствует opacity, то задаем прозрачность слоя, иначе он полностью непрозрачен
		if (layerElement->Attribute("opacity") != NULL)
		{
			float opacity = strtod(layerElement->Attribute("opacity"), NULL);
			layer.opacity = 255 * opacity;
		}
		else
		{
			layer.opacity = 255;
		}

		//  контейнер <data>
		XMLElement *layerDataElement;
		layerDataElement = layerElement->FirstChildElement("data");

		if (layerDataElement == NULL)
		{
			std::cout << "Bad map. No layer information found." << std::endl;
		}

		//  контейнер <tile> - описание тайлов каждого слоя
		XMLElement *tileElement;
		tileElement = layerDataElement->FirstChildElement("tile");

		if (tileElement == NULL)
		{
			std::cout << "Bad map. No tile information found." << std::endl;
			return false;
		}

		int x = 0;
		int y = 0;

		while (tileElement)
		{
			int tileGID = atoi(tileElement->Attribute("gid"));
			int subRectToUse = tileGID - firstTileID;

			// Устанавливаем TextureRect каждого тайла
			if (subRectToUse >= 0)
			{
				SDL_game sprite;
				sprite.setTexture(tilesetImage);
				sprite.setTextureRect(subRects[subRectToUse]);
				sprite.setPosition(x * tileWidth, y * tileHeight);
				sprite.setColor(255, 255, 255, layer.opacity);
				layer.tiles.push_back(sprite);//закидываем в слой спрайты тайлов
			}
			tileElement = tileElement->NextSiblingElement("tile");
			x++;
			if (x >= width)
			{
				x = 0;
				y++;
				if (y >= height) y = 0;
			}
		}
		layers.push_back(layer);
		layerElement = layerElement->NextSiblingElement("layer");
	}

	// работа с объектами
	XMLElement *objectGroupElement;

	// если есть слои объектов
	if (map->FirstChildElement("objectgroup") != NULL)
	{
		objectGroupElement = map->FirstChildElement("objectgroup");
		while (objectGroupElement)
		{
			//  контейнер <object>
			XMLElement *objectElement;
			objectElement = objectGroupElement->FirstChildElement("object");

			while (objectElement)
			{
				// получаем все данные - тип, имя, позиция, и тд
				std::string objectType;
				if (objectElement->Attribute("type") != NULL)
				{
					objectType = objectElement->Attribute("type");
				}
				std::string objectName;
				if (objectElement->Attribute("name") != NULL)
				{
					objectName = objectElement->Attribute("name");
				}
				int x = atoi(objectElement->Attribute("x"));
				int y = atoi(objectElement->Attribute("y"));

				int width, height;

				SDL_game sprite;
				sprite.setTexture(tilesetImage);
				sprite.setTextureRect(0, 0, 0, 0);
				sprite.setPosition(x, y);

				if (objectElement->Attribute("width") != NULL)
				{
					width = atoi(objectElement->Attribute("width"));
					height = atoi(objectElement->Attribute("height"));
				}
				else
				{
					width = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].w;
					height = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].h;
					sprite.setTextureRect(subRects[atoi(objectElement->Attribute("gid")) - firstTileID]);
				}

				// экземпляр объекта
				Object object;
				object.name = objectName;
				object.type = objectType;
				object.sprite = sprite;

				SDL_Rect  objectRect;
				objectRect.y = y;
				objectRect.x = x;
				objectRect.h = height;
				objectRect.w = width;
				object.rect = objectRect;

				// "переменные" объекта
				XMLElement *properties;
				properties = objectElement->FirstChildElement("properties");
				if (properties != NULL)
				{
					XMLElement *prop;
					prop = properties->FirstChildElement("property");
					if (prop != NULL)
					{
						while (prop)
						{
							std::string propertyName = prop->Attribute("name");
							std::string propertyValue = prop->Attribute("value");
							object.properties[propertyName] = propertyValue;
							prop = prop->NextSiblingElement("property");
						}
					}
				}


				objects.push_back(object);

				objectElement = objectElement->NextSiblingElement("object");
			}
			objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
		}
	}
	else
	{
		std::cout << "No object layers found..." << std::endl;
	}

	return true;
}

Object Level::GetObject(std::string name)
{
	// только первый объект с заданным именем
	for (int i = 0; i < objects.size(); i++)
	if (objects[i].name == name)
		return objects[i];
}

std::vector<Object> Level::GetObjects(std::string name)
{
	// все объекты с заданным именем
	std::vector<Object> vec;
	for (int i = 0; i < objects.size(); i++)
	if (objects[i].name == name)
		vec.push_back(objects[i]);

	return vec;
}


std::vector<Object> Level::GetAllObjects()
{
	return objects;
};


SDL_Point Level::GetTileSize()
{
	SDL_Point r;
	r.x = tileWidth;
	r.y = tileHeight;
	return r;
}

void Level::Draw (SDL_Renderer *render)
{
	// рисуем все тайлы (объекты не рисуем!)
	for (int layer = 0; layer < layers.size(); layer++)
	for (int tile = 0; tile < layers[layer].tiles.size(); tile++)
	{
	layers[layer].tiles[tile].Draw (render);
	}
}

#endif
