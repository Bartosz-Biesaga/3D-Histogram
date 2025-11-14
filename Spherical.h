#pragma once

class Spherical {
public:
    float distance, theta, phi;

    Spherical(float gdistance, float gtheta, float gphi) : distance(gdistance), theta(gtheta), phi(gphi) {}

    float getX() const {
        return distance * cos(phi) * cos(theta);
    }

    float getY() const { 
        return distance * sin(phi);
    }

    float getZ() const { 
        return distance * cos(phi) * sin(theta);
    }
};