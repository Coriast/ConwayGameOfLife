#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

using namespace std;

#define GRID_S 4
#define SCR_WIDTH	600
#define SCR_HEIGHT	600

int grid[] =
{
	0, 0, 0, 1,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Conway", sf::Style::Titlebar | sf::Style::Close);
	// sincronizar os updates da tela com a atualização de frames do monitor.
	window.setVerticalSyncEnabled(true);

	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::RectangleShape square(sf::Vector2f(SCR_WIDTH / GRID_S, SCR_HEIGHT / GRID_S));
		vector<sf::RectangleShape> squares;

		for (int i = 0; i < GRID_S; i++)
		{
			for (int j = 0; j < GRID_S; j++)
			{
				int grid_position = (i + j) + (j * (GRID_S - 1));
				square.setPosition(sf::Vector2f((square.getSize().x * i), (square.getSize().y * j)));
				if (grid[grid_position] == 1)
					square.setFillColor(sf::Color(255, 255, 255));
				else
					square.setFillColor(sf::Color(0, 0, 0));
				squares.push_back(square);
			}
		}
		
		window.clear(sf::Color(72, 72, 72));
		{
			
			for (auto it = squares.begin(); it != squares.end(); it++)
			{
				window.draw(*it);
				//cout << it->getPosition().x << " | " << it->getPosition().y << endl;
			}

		}
		window.display();
	}

	return 0;
}
