#include <patch.h>

Patch::Patch()
    : QPoint(0,0), _color(0), _radius(0)
{

}

Patch::Patch(int xcenter, int ycenter, int xmin, int ymin, int xmax, int ymax, unsigned char color) 
:QPoint(xcenter, ycenter), _color(color)
{
    this->_radius = std::min(std::min(abs(xcenter - xmin), abs(xcenter - xmax)), std::min(abs(ycenter - ymin), abs(ycenter - ymax)));
}

unsigned int Patch::getRadius() const {
  return this->_radius;
}

void Patch::setRadius(int radius) {
  this->_radius = radius;
}

void Patch::setColor(unsigned char color) {
  this->_color = color;
}

unsigned char Patch::getColor() const {
  return this->_color;
}

bool Patch::compareByX (const Patch p1, const Patch p2) {
  return p1.x() < p2.x();
}
bool Patch::compareByY (const Patch p1, const Patch p2) {
  return p1.y() < p2.y();
}
