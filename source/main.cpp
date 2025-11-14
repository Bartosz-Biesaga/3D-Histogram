#include "../include/pch.h"
#include "../include/Spherical.h"
#include "../include/drawing.h"

using sfe = sf::Event;
using sfk = sf::Keyboard;

int main() {
    bool running = true;
    sf::ContextSettings context(24, 0, 0, 4, 5);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "3D Histogram", 7U, context);
    sf::Clock deltaClock;
    ImGui::SFML::Init(window);
    window.setVerticalSyncEnabled(true);
    Drawing::reshapeScreen(window.getSize());
    Drawing::initOpenGL();

    while (running) {
        sfe event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sfe::Closed || (event.type == sfe::KeyPressed && event.key.code == sfk::Escape)) {
                running = false;
            }
            if (event.type == sfe::Resized) {
                Drawing::reshapeScreen(window.getSize());
            }
        }

        Drawing::drawScene();
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

