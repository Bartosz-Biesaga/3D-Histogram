#pragma once
#include "Spherical.h"
#include "../include/Histogram3D.h"

namespace Drawing {

	extern Spherical camera;
	extern sf::Vector3f scenePosition;
	extern bool perspectiveProjection;
	extern bool drawLoadDataInputs;
	extern bool drawUserGuide;
	extern bool drawHistogramInputs;
	extern Histogram3D histogram3D;
	extern ImFont* bigFont;
	extern sf::Shader dummySceneShader;
	extern std::function<void()> drawingFunction;

	struct ScreenPoint {
		sf::Vector2f position;
		bool isInFrontOfCamera;
	};

	void initOpenGL();

	void reshapeScreen(sf::Vector2u size);

    void drawScene();

	void drawFileLoadUserInputs();

	void drawUserGuideWindow();

	void drawHistogramInputsWindow();

	void drawDummyScene(sf::RenderWindow& window, sf::Time& time);

	void drawHistogram();

	void drawBar(sf::Vector3f const& nearLeftBottomPoint, sf::Vector3f const& farRightTopPoint, sf::Vector3f const& color, bool drawTopFace);

	void drawGrid3D();

	ScreenPoint mapWorldCoordinatesToScreen(sf::Vector3f worldPoint, GLdouble* modelMatrix, GLdouble* projectionMatrix, GLint* viewport);

	void drawGridTicksValues();

	void drawGridTickValue(ImDrawList* drawList, double value, sf::Vector3f worldPoint, GLdouble* modelMatrix, GLdouble* projectionMatrix, GLint* viewport);

	void drawGridLabel(ImDrawList* drawList, const char* label, ScreenPoint screenPoint, float angleRadians);
}
