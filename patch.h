#pragma once
#include <stdlib.h>
#include <algorithm>

#define RED 0
#define BLUE 1

class Patch
{
public:
  Patch();
  Patch(int xcenter, int ycenter, int xmin, int ymin, int xmax, int ymax, unsigned char color);
  
  int getX() const;
  int getY() const;
  unsigned int getRadius() const;
  unsigned char getColor() const;
  
  static bool compareByX (const Patch p1, const Patch p2);
  static bool compareByY (const Patch p1, const Patch p2);
  
private:
  int _x;
  int _y;
  unsigned int _radius;
  unsigned char _color;
};
