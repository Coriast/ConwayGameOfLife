#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

using namespace std;

#define GRID_S	20
#define SCR_S	600

bool grid[GRID_S][GRID_S];

void interactWithCell(sf::Vector2i mouse_position, sf::Event mouse_event);
void applyingRules();

int main()
{
	for (auto& v : grid)
		for (auto& e : v)
			e = 0;

	sf::RenderWindow window(sf::VideoMode(SCR_S, SCR_S), "Conway", sf::Style::Titlebar | sf::Style::Close);
	
	window.setVerticalSyncEnabled(true);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::MouseButtonPressed: 
					interactWithCell(sf::Mouse::getPosition(window), event);
					break;
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Space)
						applyingRules();
					break;
				default:
					break;
			}
		}

		sf::RectangleShape square(sf::Vector2f(SCR_S / GRID_S, SCR_S / GRID_S));
		vector<sf::RectangleShape> squares;

		for (int i = 0; i < GRID_S; i++)
		{
			for (int j = 0; j < GRID_S; j++)
			{
				if (grid[i][j] == 1)
					square.setFillColor(sf::Color(255, 255, 255));
				else
					square.setFillColor(sf::Color(0, 0, 0));

				square.setPosition(sf::Vector2f((square.getSize().x * i), (square.getSize().y * j)));
				squares.push_back(square);
			}
		}

		window.clear(sf::Color(72, 72, 72));
			
		for (auto it = squares.begin(); it != squares.end(); it++)
		{
			window.draw(*it);
		}


		window.display();
	}

	return 0;
}

void interactWithCell(sf::Vector2i mouse_position, sf::Event mouse_event)
{
	float cell_size = SCR_S / GRID_S;
	int cell_x = mouse_position.x / cell_size;
	int cell_y = mouse_position.y / cell_size;

	if (mouse_event.mouseButton.button == sf::Mouse::Left)
		grid[cell_x][cell_y] = 1;
	else if (mouse_event.mouseButton.button == sf::Mouse::Right)
		grid[cell_x][cell_y] = 0;
}

void applyingRules()
{
	bool next_grid[GRID_S][GRID_S];
	std::fill(*next_grid, *next_grid + (GRID_S * GRID_S), 0);
	for (int x = 0; x < GRID_S; x++)
	{
		for (int y = 0; y < GRID_S; y++)
		{
			int neighbors[8];
			if (x == 0)
			{
				if (y == 0)
				{
					int neighbors_p[8] = { 0, grid[x + 1][y], 0, grid[x][y + 1],
							0, 0, grid[x + 1][y + 1], 0 };
					std::copy(neighbors_p, neighbors_p + 8, neighbors);
				}
				else
				{
					int neighbors_p[8] = { 0, grid[x + 1][y], grid[x][y - 1], grid[x][y + 1],
							0, 0, grid[x + 1][y + 1], grid[x + 1][y - 1] };
					std::copy(neighbors_p, neighbors_p + 8, neighbors);
				}
			}
			else if (y == 0)
			{
				int neighbors_p[8] = { grid[x - 1][y], grid[x + 1][y], 0, grid[x][y + 1],
							0, grid[x - 1][y + 1], grid[x + 1][y + 1], 0 };
				std::copy(neighbors_p, neighbors_p + 8, neighbors);
			}
			else
			{
				int neighbors_p[8] = { grid[x - 1][y], grid[x + 1][y], grid[x][y - 1], grid[x][y + 1],
							grid[x - 1][y - 1], grid[x - 1][y + 1], grid[x + 1][y + 1], grid[x + 1][y - 1] };
				std::copy(neighbors_p, neighbors_p + 8, neighbors);
			}

			int count_neighbors = 0;
			for (int k = 0; k < 8; k++)
				count_neighbors += neighbors[k];

			if (grid[x][y] == 1)
			{
				if (count_neighbors > 3)
					next_grid[x][y] = 0;
				else if (count_neighbors < 2)
					next_grid[x][y] = 0;
				else
					next_grid[x][y] = 1;
			}
			else
			{
				if (count_neighbors == 3)
					next_grid[x][y] = 1;
			}
		}
	}
	std::copy(&next_grid[0][0], &next_grid[0][0] + (GRID_S * GRID_S), &grid[0][0]);
}

