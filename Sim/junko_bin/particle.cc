#include "simeb.h"

Particle::Particle(void)
{
  //////////////////////////////////////////////////////////////////
  // Major parameters identifying the particle.
  iId = 0;       // All particles will have a unique id.
  iBeam = 0;    // All particles are assigned to a beamlet.
  iPlane = 0;

  //////////////////////////////////////////////////////////////////
  // Particle state.
  xyzP = xyzV = xyz0; // Current position and velocity.
  xyzPCommit = xyzVCommit = xyz0;


  ////////////////////////////////////////////////////////////////
  // Various timers.
  rPert = 0.0;
  rTi = rTf = 0.0;
  rDt = FLT_MAX;

  iZOrder = iTOrder = 0;

  #ifdef _SIMEB_CAP_GROUP
  iGroup = 0;
  #endif

  #ifdef _SIMEB_CAP_ACCEL
  rFieldA = 0.0;
  #endif
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// EmissionOrder
//
// Auxiliary function used by the standard library function
// qsort() to sort an array of pointers to particles by
// particle emission time.
int Particle::EmissionOrder(Particle const * const * pparticle1, Particle const * const * pparticle2)
{
  Particle const *particle1 = *pparticle1;
  Particle const *particle2 = *pparticle2;

  // Compare the times at which these particles
  // will arrive at the source plane.
  if (particle1->rTi > particle2->rTi) return 1;
  else return -1;
}

