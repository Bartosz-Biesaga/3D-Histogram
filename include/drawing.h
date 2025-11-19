#pragma once
#include "Spherical.h"

namespace Drawing {

	extern Spherical camera;
	extern bool perspectiveProjection;
	extern bool drawLoadDataInputs;
	extern bool drawUserGuide;

	void initOpenGL();

	void reshapeScreen(sf::Vector2u size);

    void drawScene();

	void drawFileLoadUserInputs();

	void drawUserGuideBox();
}
