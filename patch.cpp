#include "patch.h"

Patch::Patch(int xcenter, int ycenter, int xmin, int ymin, int xmax, int ymax, unsigned char color) 
:_x(xcenter), _y(ycenter), _color(color)
{
    this->_radius = std::min(std::min(abs(xcenter - xmin), abs(xcenter - xmax)), std::min(abs(ycenter - ymin), abs(ycenter - ymax)));
}

int Patch::getX() const {
  return this->_x;
}
int Patch::getY() const {
  return this->_y;
}

unsigned int Patch::getRadius() const {
  return this->_radius;
}

unsigned char Patch::getColor() const {
  return this->_color;
}

bool Patch::compareByX (const Patch p1, const Patch p2) {
  return p1.getX() < p2.getX();
}
bool Patch::compareByY (const Patch p1, const Patch p2) {
  return p1.getY() < p2.getY();
}
