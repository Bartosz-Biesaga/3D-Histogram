namespace Events {

	void rotateScreen(sf::RenderWindow const& window, sf::Event const& event, sf::Vector2i& mouseLastPosition);

	void zoomScreen(sf::RenderWindow const& window, sf::Event const& event);

	void saveScreen(sf::RenderWindow const& window);
}