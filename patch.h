#pragma once
#include <stdlib.h>
#include <algorithm>
#include <QPoint>

#define RED 0
#define BLUE 1

class Patch : public QPoint
{
public:
  Patch();
  Patch(int xcenter, int ycenter, int xmin, int ymin, int xmax, int ymax, unsigned char color);
  
  int getCenter() const;
  unsigned int getRadius() const;
  unsigned char getColor() const;
  
  static bool compareByX (const Patch p1, const Patch p2);
  static bool compareByY (const Patch p1, const Patch p2);
  
private:
  QPoint _center;
  unsigned int _radius;
  unsigned char _color;
};
