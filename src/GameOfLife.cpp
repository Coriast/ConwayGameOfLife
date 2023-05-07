#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

using namespace std;

#define GRID_S	4
#define SCR_S	600

std::vector<bool> grid;

void createNewCell(sf::Vector2i mouse_position);

int main()
{
	for (int i = 0; i < (GRID_S * GRID_S); i++)
		grid.push_back(0);

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
					if(event.mouseButton.button == sf::Mouse::Left)
						createNewCell(sf::Mouse::getPosition(window));
					break;
				default:
					break;
			}
		}

		sf::RectangleShape square(sf::Vector2f(SCR_S / GRID_S, SCR_S / GRID_S));
		vector<sf::RectangleShape> squares;

		auto it = grid.cbegin();
		for (int i = 0; i < GRID_S; i++)
		{
			for (int j = 0; j < GRID_S; j++)
			{
				if (*it == 1)
					square.setFillColor(sf::Color(255, 255, 255));
				else
					square.setFillColor(sf::Color(0, 0, 0));

				square.setPosition(sf::Vector2f((square.getSize().x * i), (square.getSize().y * j)));
				squares.push_back(square);
				
				it++;
			}
		}
		/*
		for (int i = 0; i < GRID_S; i++)
		{
			for (int j = 0; j < GRID_S; j++)
			{
				int grid_position = (i + j) + (j * (GRID_S - 1));
				if (grid[grid_position] == 1)
					square.setFillColor(sf::Color(255, 255, 255));
				else
					square.setFillColor(sf::Color(0, 0, 0));
			}
		}
		*/
		window.clear(sf::Color(72, 72, 72));
			
		for (auto it = squares.begin(); it != squares.end(); it++)
		{
			window.draw(*it);
		}


		window.display();
	}

	return 0;
}

void createNewCell(sf::Vector2i mouse_position)
{
	float cell_size = SCR_S / GRID_S;

	int cell_x = mouse_position.x / cell_size;
	int cell_y = mouse_position.y / cell_size;
	int position = (cell_x + cell_y) + (cell_x * (GRID_S - 1));
	grid[position] = 1;

	cout << cell_x << " | " << cell_y << " | " << position << endl;
}


