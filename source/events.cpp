#include "../include/pch.h"
#include "../include/events.h"
#include "../include/drawing.h"
#include "../include/tinyfiledialogs.h"
#include "../Histogram3D.h"

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
            Drawing::camera.phi = static_cast<float>(std::numbers::pi / 2.0f);
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

    bool loadHistogramData(char* delimiter, char* column1, char* column2, char* boolColumn,
            bool skipFirstRow, bool convertToInt, int* binsNumber) {
        std::vector<std::string> missingFieldsErrors;
        if (!*delimiter) {
            missingFieldsErrors.push_back("Please provide delimiter field");
        }
        if (!*column1) {
			missingFieldsErrors.push_back("Please provide first column field");
        }
        if (!*column2) {
            missingFieldsErrors.push_back("Please provide second column field");
        }
        if (!*boolColumn) {
            missingFieldsErrors.push_back("Please provide boolean column field");
        }
        if (!missingFieldsErrors.empty()) {
            std::string errorMessage = "Cannot load data due to missing fields:\n";
            for (const auto& error : missingFieldsErrors) {
                errorMessage += "- " + error + "\n";
            }
            tinyfd_messageBox("Error", errorMessage.c_str(), "ok", "error", 1);
            return false;
		}
        if (binsNumber[0] <= 0 || binsNumber[1] <= 0) {
            tinyfd_messageBox("Error", "Bins number must be positive", "ok", "error", 1);
            return false;
        }
        constexpr char const* filePattern[1] = { "*.csv" };
        const char* path = tinyfd_openFileDialog(
            "Load csv file",
            nullptr,
            1,
            filePattern,
            nullptr,
            0
        );
        std::vector<std::tuple<double, double, bool>> data;
        if (path == nullptr) {
            return false;
        }
        else {
            try {
                if (convertToInt) {
                    int column1Number, column2Number, boolColumnNumber;
                    try {
                        column1Number = std::stoi(column1);
                        column2Number = std::stoi(column2);
                        boolColumnNumber = std::stoi(boolColumn);
                    }
                    catch (std::invalid_argument e) {
                        tinyfd_messageBox("Error", "Column index can not be converted to integer", "ok", "error", 1);
                        return false;
                    }
                    data = Histogram3D::loadData(path, *delimiter, column1Number, column2Number, boolColumnNumber, skipFirstRow);
                }
                else {
                    data = Histogram3D::loadData(path, *delimiter, column1, column2, boolColumn);
                }
            }
            catch (std::exception e) {
                tinyfd_messageBox("Error", e.what(), "ok", "error", 1);
                return false;
            }
            Drawing::histogram3D = Histogram3D(binsNumber[0], binsNumber[1], data);
            std::thread t(&Histogram3D::sortDataAndUpdateHistogramAndBins, &Drawing::histogram3D);
            t.detach();
            return true;
        }
    }
}