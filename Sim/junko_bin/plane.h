#ifndef _TPLANE
#define _TPLANE

class TPlane
{
 private:
  static bool bRayTrace;
  static int iPlaneCounter;
  int        iPlane;

  real rZ;

  bool bVoltageSpec;
  real rVoltage;
  real rFieldA;
  
  int  iSetGroup;
  int  iClearGroup;

 protected:
  int iGroup;

 public:
  static TPlane * Parse(TParser &);

  TPlane();
  virtual ~TPlane(void) {};

  virtual void Parse(TParser &, char *);
  virtual void Initialize(void);
  virtual bool Cross(Particle * const p);
  virtual void Finalize(FILE *pfile) {};

  int  const iGetPlane(void) const {return iPlane;};
  char const *  sGetName(void) const {return sName;};
  real rGetZ(void)         const {return rZ;};
  real rGetVoltage(void)   const {return rVoltage;};
  int  iGetSetGroup(void)   const {return iSetGroup;};
  int  iGetClearGroup(void) const {return iClearGroup;};
  real rGetFieldA(void)    const {return rFieldA;};

  void rSetVoltage(real r_voltage)
    {if (!bVoltageSpec) rVoltage = r_voltage;}

  void rSetFieldA(real r_field_a) {rFieldA = r_field_a;};

 protected:
  char sName[MAX_TOKEN];

  static void Trajectory(Particle * p, TXY const & xy_v_normal);
};

class TTarget : public TPlane
{
 public:
  TTarget();

  virtual void Parse(TParser &, char *);
  virtual void Initialize(void);
  virtual bool Cross(Particle * const p);

 private:
  FILE *TargetFile;
  int iNumParticles;
  int iTotalParticles;
  int iNumTrailers;
};

class TAperture : public TPlane
{
 public:
  TAperture() : TPlane() {Shape = NULL;};

  virtual void Parse(TParser &, char *);
  virtual bool Cross(Particle * const p);

 private:
  TShape * Shape;
};

class TThinLens : public TPlane
{
public:
  TThinLens();

  virtual void Parse(TParser &, char *);
  virtual bool Cross(Particle * const p);

private:
  real   rFocalLength;
};


#define EELENS_OFF    0
#define EELENS_TARGET 1
#define EELENS_LENS   2

class EELens : public TTarget
{
public:
  static int    iMode;
  static int    iGroup;
  static FILE * pFile;
  static int    iNumLens;
  static int    iNumBeams;
  static int    iNumElements;
  static int    iNumSegments;
  static int *  LensMap;
  static real * CurrentMap;

  EELens();

  virtual void Initialize(void);
  virtual bool Cross(Particle * const p);

private:
  static int   iLensCounter;
  int          iLens;

  TXY *   LensAxis;
  real *  rMaxRadius;
  real *  rStepRadius;
  real ** LensArray;
  int     iMap;

  TXY    LensLookup(TXY const xy_p);
};


#endif

