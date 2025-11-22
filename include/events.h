#pragma once

namespace Events {

	void rotateScreen(sf::RenderWindow const& window, sf::Event const& event, sf::Vector2i& mouseLastPosition);

	void zoomScreen(sf::RenderWindow const& window, sf::Event const& event);

	void moveScene(sf::Event const& event);

	void saveScreen(sf::RenderWindow const& window);

	bool loadHistogramData(char* delimiter, char* column1, char* column2, char* boolColumn,
		bool skipFirstRow, bool convertToInt, int* binsNumber);

}