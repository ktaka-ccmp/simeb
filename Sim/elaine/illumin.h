#ifndef _TILLUMIN
#define _TILLUMIN

class TIllumin
{
 public:
  static TIllumin * Parse(TParser &);

  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *, real) = NULL;
};

class TIlluminUniform : public TIllumin
{
 public:
  TIlluminUniform(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *, real);

 private:
  real rAlpha;
  real rDeltaE;
};

class TIlluminLumped : public TIllumin
{
 public:
  TIlluminLumped(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *, real);

 private:
  real rAlpha;
  real rDeltaE;
};

class TIlluminGaussian : public TIllumin
{
 public:
  TIlluminGaussian(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *, real);

 private:
  real rAlpha;
  real rDeltaE;
};

class TIlluminFixed : public TIllumin
{
 public:
  TIlluminFixed(void);
  virtual void Parse(TParser &, char *);
  virtual void Randomize(Particle *, real);

 private:
  TXY xyAlpha;
};

#endif


