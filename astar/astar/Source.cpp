#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <algorithm>
#include <cstdlib>
#include "olcConsoleGameEngine.h"
#include "AStar.h"

class OneLoneCoder_Example : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Example() {}

	bool OnUserCreate() override
	{
		//CREATE FOR WALLS DEMO

		//fill this block with white
		this->Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, COLOUR::FG_WHITE);

		// create for ASTAR, will have to be redone
		astar.addEmptyWeightedGraphDiagonal(this->ScreenWidth(), this->ScreenHeight());

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if (wallBool)
		{
			if (m_keys[L'S'].bPressed)
			{
				spressed = true;
			}

			if (spressed)
			{   // pressed for start or end
				if (m_mouse[0].bPressed)
				{
					int x = GetMouseX();
					int y = GetMouseY();
					if (x >= 1 && x <= ScreenWidth() && y >= 1 && y <= ScreenHeight())
					{
						if (walls.find({ x,y }) == walls.end())
						{
							if (!xpressed)
							{
								this->start = { x,y };
								this->Draw(x, y, PIXEL_SOLID, FG_MAGENTA);
								xpressed = true;
							}
							else
							{
								this->end = { x,y };
								this->Draw(x, y, PIXEL_SOLID, FG_GREEN);
								spressed = !spressed;
								xpressed = !xpressed;

								//assign the walls to astar.
								astar.forrestGridDiagonal.walls = this->walls;
								//assign start and end
								wallBool = !wallBool;
								// i wanna end the app here
								return true;
							}
						}
					}
				}
				return true;
			}
			//this happens first = filling the walls
			if (!spressed)
			{
				if (m_mouse[0].bPressed || m_mouse[0].bHeld)
				{
					int x = GetMouseX();
					int y = GetMouseY();
					if (x >= 0 && x < ScreenWidth() && y >= 0 && y < ScreenHeight())
					{
						if (walls.find({ x,y }) == walls.end())
						{
							walls.insert({ x,y });
						}
					}
				}
				//and draw the walls;
				for (auto& element : walls)
				{
					this->Draw(element.x, element.y, PIXEL_SOLID, FG_BLACK);
				}
				return true;
			}

			//this->Fill(ScreenWidth() / 2 - 1, ScreenHeight() / 2 - 1, ScreenWidth() / 2, ScreenHeight() / 2, PIXEL_SOLID, COLOUR::FG_BLACK);
		}
		else
		{
			//initial app is done, now we display the result.

			//add the walls
			astar.addWallCollectionToGraphDiagonal(walls);

			//search the path.
			auto walkedPath = astar.searchForWholePathDiagonal(start, end);

			// START OF DRAWING LOGIC

			//fill this block with yellow
			this->Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, COLOUR::FG_WHITE);

			//draw path
			for (const auto& element : walkedPath)
			{
				this->Fill(element.x, element.y, element.x + 1, element.y + 1, PIXEL_SOLID, COLOUR::FG_RED);
			}

			//draw walls
			for (const auto& element : astar.forrestGridDiagonal.walls)
			{
				this->Fill(element.x, element.y, element.x + 1, element.y + 1, PIXEL_SOLID, COLOUR::FG_BLUE);
			}

			//draw goal and beginning
			this->Draw(start.x, start.y, PIXEL_SOLID, FG_MAGENTA);
			this->Draw(end.x, end.y, PIXEL_SOLID, FG_MAGENTA);

			return true;
		}
	}

private:
	AStar astar;
	bool wallBool = true;

	// walls parameters
	std::unordered_set<GridLocation> walls;
	GridLocation start;
	GridLocation end;

	bool spressed = false;
	bool xpressed = false;
};

int main()
{
	int width;
	int height;
	std::cout << "Please input a width for a graph (10-200)" << std::endl;
	std::cin >> width;
	std::cout << "Please input a height for a graph (10-200)" << std::endl;
	std::cin >> height;
	std::cout << "The grid will now be displayed, please click on the tiles to make them an impassable walls..." << std::endl;
	std::system("pause");
	std::cout << "Once you are done with your walls, press 'S' on your keyboard, click on one tile to choose start, and then click on another tile to choose end. Thats it!" << std::endl;
	std::system("pause");

	OneLoneCoder_Example game;
	game.ConstructConsole(width, height, 12, 12);
	game.Start();
	return 0;
}
