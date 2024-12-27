#ifndef _SOURCE
#define _SOURCE

#include "simeb.h"

class TSource : public TPlane
{
public:
  static TSource *  Parse(TParser &parser);
  static Particle * NewParticle(void);
  static int        iGetTotalParticles(void);
  static Particle * GetParticle(int i);

  TSource(void);

  virtual void Parse(TParser &, char *);
  virtual void Initialize(void);

  real rEmissionTime;

private:
  static int   iTotalParticles;
  static Particle * ParticleDatabase; 

  long      lSeed;
};

class TSourceArray : public TSource
{
 public:
  TSourceArray(void);
  virtual void Parse(TParser &parser, char *);
  virtual void Initialize(void);

 private:
  int         iNumParticles;
  int         iGroup;
  real        rCurrent;
  int         iDim;
  real        rPitch;
  TShape *    Shape;
  TIllumin *  Illumination;
  TIllumin *  Scattering;
  real *      MaskIllumination;
  real *      MaskScattering;
};

class TSourceEELens : public TSource
{
 public:
  TSourceEELens(void);
  virtual void Parse(TParser &parser, char *);
  virtual void Initialize(void);

 private:
  char   sFileName[MAX_TOKEN];
  real   rCurrent;
  int    iDim;
  real * MaskIllumination;
  real * MaskScattering;
};

class TSourceGrid : public TSource
{
 public:
  TSourceGrid(void);
  virtual void Parse(TParser &parser, char *);
  virtual void Initialize(void);

 private:
  int         iNumParticles;
  int         iGroup;
  int         iDim;
  real        rPitch;
  TIllumin *  Illumination;
  TXY         xyOffset;
};

#endif
