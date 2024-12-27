#ifndef _TSHAPE
#define _TSHAPE

class TShape
{
 public:
  static TShape * Parse(TParser &);

  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *) = 0;
  virtual bool bInside(TXY xy) = 0;

  TXY xyOffset;
};

class TShapePoint : public TShape
{
 public:
  TShapePoint(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *);
  virtual bool bInside(TXY xy);
};

class TShapeCircle : public TShape
{
 public:
  TShapeCircle(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *);
  virtual bool bInside(TXY xy);

 private:
  real rRadius;
};

class TShapeBagel : public TShape
{
 public:
  TShapeBagel(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *);
  virtual bool bInside(TXY xy);

 private:
  real rRadius;
  real rHole;
};

class TShapeSquare : public TShape
{
 public:
  TShapeSquare(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *);
  virtual bool bInside(TXY xy);

 private:
  real rSize;
};

class TShapeRectangle : public TShape
{
 public:
  TShapeRectangle(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *);
  virtual bool bInside(TXY xy);

 private:
  TXY xySize;
};

#endif


