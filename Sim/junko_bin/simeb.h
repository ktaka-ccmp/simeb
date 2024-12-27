#ifndef _SIMEB
#define _SIMEB

#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h> // by ktka 12/24/99

#define ASSERT(x) if(!(x)) {fprintf(stderr, "Assertion Failed! %s line %d iteration %d\n", __FILE__, __LINE__, iIteration); exit(0);}

#define MAX_TOKEN  128

class TShape;
class TIllumin;
class TPlane;
class TSource;
class Particle;

#include "include.h"
#include "vector.h"
#include "cap.h"
#include "setup.h"

class SimEB
{
private:
  static real const rSmall;
  static real const rCoulomb;

  static char sRoot[MAX_TOKEN];
  static char sName[MAX_TOKEN];

  static real rQ;
  static real rM;
  static real rQM;
  static real rScaleFactor;

  static real rEpsilon;
  static int  iSphere;

  static int  iHalt;

  static FILE * fpProgress;

  static Particle * ParticleDatabase;
  static Particle ** ParticleArray;
  static Particle ** ParticleHeap;

  inline static void Iteration(void);

  inline static void Commit(Particle * const, Particle * const);
  inline static void CommitMirror(Particle * const, Particle * const);
  inline static bool Attempt(Particle * const, Particle * const);
  inline static void Catchup(
    Particle * const p, Particle * const q,
    TXYZ & xyz_dp, TXYZ & xyz_dv);

  inline static real Integrate(TXYZ const &, TXYZ const &, real const, real &);
  inline static void Integrate(TXYZ &, TXYZ &, real const);

  inline static void Kill(Particle * const);
  inline static void HeapFilterUp(Particle * const);
  inline static void HeapFilterDown(Particle * const);
  inline static void ArrayDelete(Particle * const);

public:
  static real const rQ0;
  static real const rM0;
  static real const rM1;
  static real const rE0;

  static int iIteration;
  static int iNumGenerated;
  static int iNumActivated;
  static int iNumKilled;

  const static char * sGetRoot(void)        {return sRoot;};
  const static char * sGetName(void)        {return sName;};

  const static real rGetQ(void)             {return rQ;};
  const static real rGetQM(void)            {return rQM;};
  const static real rGetScaleFactor(void)   {return rScaleFactor;};

  const static real rGetEpsilon(void)       {return rEpsilon;}
  const static int  iGetSphere(void)        {return iSphere;};

  static void sSetName(char const * const s_name) {
    strcpy(sRoot, s_name);
    strcpy(sName, s_name);
  };
  static void sExtName(char const * const s_ext) {strcat(sName, s_ext);};

  static void rSetQM(real r_q, real r_m);

  static void InitProgress(void);
  static void PollProgress(int);
  static void KillProgress(void);
  static void Halt(void);

  static void Setup(int, char const * const[]);
  static void Parse(TParser &, char *);
  static void Initialize(void);
  static void Simulate(void);
  static void Finalize(void);
};

inline void SimEB::rSetQM(real r_q, real r_m)
{
  rQ = r_q;
  rM = r_m;
  rQM = r_q / r_m;
  rScaleFactor = rCoulomb * rQ*rQ/rM;
};

#include "relativity.h"
#include "shape.h"
#include "illumin.h"
#include "plane.h"
#include "source.h"
#include "column.h"
#include "particle.h"

#endif // SIMEB








