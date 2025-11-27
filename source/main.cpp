#include "../include/pch.h"
#include "../include/Spherical.h"
#include "../include/drawing.h"
#include "../include/events.h"

using sfe = sf::Event;
using sfk = sf::Keyboard;
using sfm = sf::Mouse;

int main() {
    bool running = true;
    sf::ContextSettings context(24, 0, 0, 4, 5);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "3D Histogram", 7U, context);
    sf::Clock deltaClock;
    ImGui::SFML::Init(window);
    sfe event;
    window.setVerticalSyncEnabled(true);
    Drawing::reshapeScreen(window.getSize());
    Drawing::initOpenGL();
	sf::Vector2i mouseLastPosition{ 0, 0 };
	bool mouseLastPositionInitialized = false;
    bool willSaveScreen = false;
    while (running) {
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sfe::Closed) {
                running = false;
            }
            if (event.type == sfe::Resized) {
                Drawing::reshapeScreen(window.getSize());
            }
            if (event.type == sfe::MouseButtonPressed && event.mouseButton.button == sfm::Left) {
                mouseLastPosition = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
				mouseLastPositionInitialized = true;
            }
            if (sfm::isButtonPressed(sfm::Left) && mouseLastPositionInitialized && event.type == sfe::MouseMoved) {
				Events::rotateScreen(window, event, mouseLastPosition);
            }
            if (event.type == sfe::MouseButtonReleased && event.mouseButton.button == sfm::Left) {
				mouseLastPositionInitialized = false;
            }
			if (event.type == sfe::MouseWheelScrolled) {
                Events::zoomScreen(window, event);
            }
			if (event.type == sfe::KeyPressed) {
                if(event.key.code == sfk::P) {
                    Drawing::perspectiveProjection = !Drawing::perspectiveProjection;
					Drawing::reshapeScreen(window.getSize());
                }
                if (event.key.code == sfk::S && sfk::isKeyPressed(sfk::LControl)) {
                    window.clear(sf::Color::White);
                    willSaveScreen = true;
                }
                if (event.key.code == sfk::L && sfk::isKeyPressed(sfk::LControl)) {
                    Drawing::drawLoadDataInputs = !Drawing::drawLoadDataInputs;
                }
                if (event.key.code == sfk::H && sfk::isKeyPressed(sfk::LControl)) {
                    Drawing::drawUserGuide = !Drawing::drawUserGuide;
                }
                if (event.key.code == sfk::M && sfk::isKeyPressed(sfk::LControl)) {
                    Drawing::drawHistogramInputs = !Drawing::drawHistogramInputs;
                }
                Events::moveScene(event);
            }
        }
		
        Drawing::drawScene();
        ImGui::SFML::Update(window, deltaClock.restart());
        if (Drawing::drawLoadDataInputs) {
            Drawing::drawFileLoadUserInputs();
        }
        if (Drawing::drawUserGuide) {
            Drawing::drawUserGuideWindow();
        }
        if (Drawing::drawHistogramInputs) {
            Drawing::drawHistogramInputsWindow();
        }
        if (Drawing::histogram3D.drawingReady && Drawing::histogram3D.isGridWanted) {
            Drawing::drawGridTicksValues();
        }
        ImGui::SFML::Render(window);
        if (willSaveScreen) {
            Events::saveScreen(window);
            willSaveScreen = false;
        }
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}

