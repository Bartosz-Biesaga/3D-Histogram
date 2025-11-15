#include "../include/pch.h"
#include "../include/events.h"
#include "../include/drawing.h"
#include "../include/tinyfiledialogs.h"

namespace Events {

	void rotateScreen(sf::RenderWindow const &window, sf::Event const &event, sf::Vector2i &mouseLastPosition) {
        float rotationAmplitude = 5.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            rotationAmplitude = 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            rotationAmplitude = 0.1f;
        }
        Drawing::camera.theta += rotationAmplitude / window.getSize().x * (event.mouseMove.x - mouseLastPosition.x);
        Drawing::camera.phi += rotationAmplitude / window.getSize().y * (event.mouseMove.y - mouseLastPosition.y);
        if (Drawing::camera.phi > std::numbers::pi / 2.0f) {
            Drawing::camera.phi = std::numbers::pi / 2.0f;
        }
        else if (Drawing::camera.phi < 0.0f) {
            Drawing::camera.phi = 0.0f;
        }
        mouseLastPosition.x = event.mouseMove.x;
		mouseLastPosition.y = event.mouseMove.y;
	}

    void zoomScreen(sf::RenderWindow const& window, sf::Event const& event) {
        float zoomFactor = 0.2f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            zoomFactor = 0.02f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            zoomFactor = 0.002f;
        }
        Drawing::camera.distance += (event.mouseWheelScroll.delta > 0) ? -zoomFactor : zoomFactor;
        if (Drawing::camera.distance < 0.1f) {
            Drawing::camera.distance = 0.1f;
        }
        else if (Drawing::camera.distance > 10.0f) {
            Drawing::camera.distance = 10.0f;
        }
        if (Drawing::perspectiveProjection == false) {
            Drawing::reshapeScreen(window.getSize());
        }
    }

    void saveScreen(sf::RenderWindow const& window) {
        constexpr char const* filePattern[1] = { "*.png" };
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);
        sf::Image screenshot = texture.copyToImage();
        const char* path = tinyfd_saveFileDialog(
            "Save Image",
            "image.png",
            1,
            filePattern,
            nullptr
        );
        if (path != nullptr) {
            screenshot.saveToFile(path);
        }
    }
}