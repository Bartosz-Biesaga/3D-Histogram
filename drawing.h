#pragma once
#include "Spherical.h"

namespace Drawing {

	extern Spherical camera;
	extern bool perspectiveProjection;

	void initOpenGL();

	void reshapeScreen(sf::Vector2u size);

    void drawScene();
}
