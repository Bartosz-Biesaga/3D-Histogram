#pragma once
#include "Spherical.h"
#include "../include/Histogram3D.h"

namespace Drawing {

	extern Spherical camera;
	extern sf::Vector3f scenePosition;
	extern bool perspectiveProjection;
	extern bool drawLoadDataInputs;
	extern bool drawUserGuide;
	extern Histogram3D histogram3D;

	void initOpenGL();

	void reshapeScreen(sf::Vector2u size);

    void drawScene();

	void drawFileLoadUserInputs();

	void drawUserGuideBox();

	void drawDummyScene();

	void drawHistogram();

	void drawBar(sf::Vector3f const& nearLeftBottomPoint, sf::Vector3f const& farRightTopPoint, sf::Vector3f const& color, bool drawTopFace);
}
