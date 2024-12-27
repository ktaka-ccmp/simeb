#include "simeb.h"

TShape * TShape::Parse(TParser &parser)
{
  TShape * shape = NULL;
  char s_token[MAX_TOKEN];

  parser >> s_token;
  if (!strcmp(s_token, "Point")) shape = new TShapePoint();
  if (!strcmp(s_token, "Circle")) shape = new TShapeCircle();
  if (!strcmp(s_token, "Bagel")) shape = new TShapeBagel();
  if (!strcmp(s_token, "Square")) shape = new TShapeSquare();
  if (!strcmp(s_token, "Rectangle")) shape = new TShapeRectangle();
  if (!shape) parser.Error();

  parser >> s_token;
  if (strcmp(s_token, "{")) parser.Error();

  for (;;) {
    parser >> s_token;
    if (parser.Eof()) parser.Error();
    if (!strcmp(s_token, "}")) break;
    shape->Parse(parser, s_token);
  }
  return shape;
}

void TShape::Parse(TParser &parser, char *s_token) {
  if (!strcmp(s_token, "Offset")) parser >> xyOffset;
  else parser.Error();
}

TShapePoint::TShapePoint(void) : TShape() {return;}
void TShapePoint::Parse(TParser &parser, char *s_token)
  {TShape::Parse(parser, s_token);}
void TShapePoint::Randomize(Particle *p) {p->xyzP = xyOffset; return;}
bool TShapePoint::bInside(TXY xy) {return false;}

TShapeCircle::TShapeCircle(void) : TShape() {rRadius = 0.0;}
void TShapeCircle::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Radius")) parser >> rRadius;
  else if (!strcmp(s_token, "Diameter")) {parser >> rRadius; rRadius /= 2;}
  else TShape::Parse(parser, s_token);
}
void TShapeCircle::Randomize(Particle * p)
{
  real r_radius = rRadius * sqrt(drand48());
  real r_alpha = 2*M_PI*drand48();
  p->xyzP.rX = r_radius * cos(r_alpha);
  p->xyzP.rY = r_radius * sin(r_alpha);
  p->xyzP += xyOffset;
}
bool TShapeCircle::bInside(TXY xy)
{
  xy -= xyOffset;
  return xy.rX*xy.rX + xy.rY*xy.rY < rRadius*rRadius;
}

TShapeBagel::TShapeBagel(void) : TShape() {rRadius = 0.0; rHole = 0.0;}
void TShapeBagel::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Radius")) parser >> rRadius;
  else if (!strcmp(s_token, "Diameter")) {parser >> rRadius; rRadius /= 2;}
  else if (!strcmp(s_token, "Hole")) {parser >> rHole; rHole /= 2;}
  else TShape::Parse(parser, s_token);
}
void TShapeBagel::Randomize(Particle * p)
{
  if (rRadius <= rHole) {
    cerr << "Bad Bagel Parameters!\n";
    exit(0);
  }

  real r_radius = 0.0;
  while ((r_radius  = rRadius * sqrt(drand48())) < rHole);
  real r_alpha = 2*M_PI*drand48();
  p->xyzP.rX = r_radius * cos(r_alpha);
  p->xyzP.rY = r_radius * sin(r_alpha);
  p->xyzP += xyOffset;
}
bool TShapeBagel::bInside(TXY xy)
{
  xy -= xyOffset;
  if (xy.rX*xy.rX + xy.rY*xy.rY > rRadius*rRadius) return false;
  if (xy.rX*xy.rX + xy.rY*xy.rY < rHole*rHole) return false;
  return true;
}

TShapeSquare::TShapeSquare(void) : TShape() {rSize = 0.0;}
void TShapeSquare::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Size")) parser >> rSize;
  else TShape::Parse(parser, s_token);
}
void TShapeSquare::Randomize(Particle * p)
{
  p->xyzP.rX = rSize * drand48() - rSize/2;
  p->xyzP.rY = rSize * drand48() - rSize/2;
  p->xyzP += xyOffset;
}
bool TShapeSquare::bInside(TXY xy)
{
  xy -= xyOffset;
  if (fabs(xy.rX) > rSize/2) return false;
  if (fabs(xy.rY) > rSize/2) return false;
  return true;
}

TShapeRectangle::TShapeRectangle(void) : TShape() {xySize = xy0;}
void TShapeRectangle::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Size")) parser >> xySize;
  else TShape::Parse(parser, s_token);
}
void TShapeRectangle::Randomize(Particle * p)
{
  p->xyzP.rX = xySize.rX * drand48() - xySize.rX/2;
  p->xyzP.rY = xySize.rY * drand48() - xySize.rY/2;
  p->xyzP += xyOffset;
}
bool TShapeRectangle::bInside(TXY xy)
{
  xy -= xyOffset;
  if (fabs(xy.rX) > xySize.rX/2) return false;
  if (fabs(xy.rY) > xySize.rY/2) return false;
  return true;
}

