#include "simeb.h"

real const SimEB::rQ0 = 1.60217733E-19;
real const SimEB::rM0 = 9.1093897E-31;
real const SimEB::rM1 = 1.673E-27;
real const SimEB::rE0 = 8.854E-12;
real const SimEB::rCoulomb = 1/(4*M_PI*SimEB::rE0);
real const SimEB::rSmall = 1.0e-2;

char SimEB::sRoot[MAX_TOKEN];
char SimEB::sName[MAX_TOKEN];

real SimEB::rQ = SimEB::rQ0;
real SimEB::rM = SimEB::rM0;
real SimEB::rQM = SimEB::rQ / SimEB::rM;
real SimEB::rScaleFactor = SimEB::rCoulomb * SimEB::rQ * SimEB::rQM;

int  SimEB::iSphere = 0;
real SimEB::rEpsilon = 0.0;

FILE * SimEB::fpProgress = NULL;

int  SimEB::iHalt = -1;

int SimEB::iIteration = 0;
int SimEB::iNumGenerated = 0;
int SimEB::iNumActivated = 0;
int SimEB::iNumKilled = 0;

Particle * SimEB::ParticleDatabase = NULL;
Particle ** SimEB::ParticleArray = NULL;
Particle ** SimEB::ParticleHeap = NULL;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// main()
//
void main(int i_argc, char const * const s_argv[])
{
  cout << setprecision(6);

  SimEB::Setup(i_argc, s_argv);
  SimEB::Initialize();
  SimEB::InitProgress();
  SimEB::Simulate();
  SimEB::KillProgress();
  SimEB::Finalize();
}

void SimEB::Simulate(void)
{
  while (iNumKilled < iNumGenerated) {
    if (iIteration == iHalt) Halt();
    Iteration();
    ++iIteration;
  }
}

void SimEB::Iteration(void)
{
  Particle * const particle_p = ParticleHeap[0];

  if (!particle_p->iGetPlane()) ++iNumActivated;

  int const i_z = particle_p->iZOrder;

  // Fly the particle to rTf and commit all interactions.
  if (particle_p->rTf > particle_p->rTi) {
    int const i_min = i_z - iGetSphere() >? iNumKilled;
    int const i_max = i_z + iGetSphere() <? (iNumActivated-1);

    for (int i = i_min; i < i_z; ++i) Commit(particle_p, ParticleArray[i]);
    for (int i = i_z+1; i <= i_max; ++i) Commit(particle_p, ParticleArray[i]);

    particle_p->Commit();
    particle_p->TimeStepIncr(rEpsilon);
  }

  if (!particle_p->NextEvent()) {Kill(particle_p); return;}

  int i_max = iSphere;
  i_max <?= (i_z - iNumKilled) >? (iNumActivated-i_z-1);

  int i;
  for (i = 1; i <= i_max; ++i) {
    if (i_z - i >= iNumKilled)   if (!Attempt(particle_p, ParticleArray[i_z-i])) break;
    if (i_z + i < iNumActivated) if (!Attempt(particle_p, ParticleArray[i_z+i])) break;
  }

  if (i <= i_max) particle_p->TimeStepDecr(rEpsilon);

  HeapFilterDown(ParticleHeap[0]);
}

inline void SimEB::Kill(Particle * const particle_p)
{
  particle_p->rTf = FLT_MAX;
  HeapFilterDown(particle_p);
  ArrayDelete(particle_p);
  ++iNumKilled;
}


inline void SimEB::Commit(Particle * const particle_p, Particle * const particle_q)
{
  if (particle_p->iGetGroup() & particle_q->iGetGroup()) return;

  // Compute the interaction time.
  real const r_dt = particle_p->rTf - (particle_p->rTi >? particle_q->rTi);
  if (r_dt == 0.0) return;

  TXYZ xyz_dp, xyz_dv;
  Catchup(particle_p, particle_q, xyz_dp, xyz_dv);
  Integrate(xyz_dp, xyz_dv, r_dt);

  // Update the commitments.
  particle_p->xyzPositionCommit += xyz_dp;
  particle_p->xyzVelocityCommit += xyz_dv;

  particle_q->xyzPositionCommit -= xyz_dp;
  particle_q->xyzVelocityCommit -= xyz_dv;
}

inline bool SimEB::Attempt(Particle * const particle_p, Particle * const particle_q)
{
  if (particle_p->iGetGroup() & particle_q->iGetGroup()) return true;

  // Compute the interaction time.
  real const r_dt = (particle_p->rTf <? particle_q->rTf) - (particle_p->rTi >? particle_q->rTi);
  if (r_dt == 0.0) return true;

  TXYZ xyz_dp, xyz_dv;
  Catchup(particle_p, particle_q, xyz_dp, xyz_dv);

  real r_impact;
  real r_pert = Integrate(xyz_dp, xyz_dv, r_dt, r_impact);
  r_pert /= r_impact;

  real r_da = particle_p->rGetFieldA() - particle_q->rGetFieldA();
  if (r_da != 0.0) r_pert += 0.125*fabs(r_da)*r_dt*r_dt / r_impact;

  particle_p->rPert += r_pert;
  if (particle_p->rPert > rEpsilon) return false;

  particle_q->rPert += r_pert;
  if (particle_q->rPert > rEpsilon) {
    particle_p->rPert -= r_pert;
    particle_q->TimeStepTrunc(particle_p->rTi, r_pert, rEpsilon);
    HeapFilterUp(particle_q);
  }

  return true;
}

inline void SimEB::Catchup(Particle * const particle_p, Particle * const particle_q, TXYZ & xyz_dp, TXYZ & xyz_dv)
{
  // Compute the relative position and velocity at the start
  // of the interaction period.
  real const r_dt = particle_p->rTi - particle_q->rTi;

  if (particle_p->rGetFieldA()==0.0 && particle_q->rGetFieldA()==0.0) {
    xyz_dv = particle_p->xyzVelocity - particle_q->xyzVelocity;

    if (r_dt <= 0.0)
      xyz_dp = (particle_p->xyzPosition - particle_p->xyzVelocity * r_dt) - particle_q->xyzPosition;
    else
      xyz_dp = particle_p->xyzPosition - (particle_q->xyzPosition + particle_q->xyzVelocity * r_dt);
  }

  else {
    TXYZ xyz_p1 = particle_p->xyzPosition;
    TXYZ xyz_v1 = particle_p->xyzVelocity;

    TXYZ xyz_p2 = particle_q->xyzPosition;
    TXYZ xyz_v2 = particle_q->xyzVelocity;

    if (r_dt < 0.0) particle_p->Fly(xyz_p1, xyz_v1, -r_dt);
    else            particle_q->Fly(xyz_p2, xyz_v2, r_dt);

    xyz_dp = xyz_p1 - xyz_p2;
    xyz_dv = xyz_v1 - xyz_v2;
  }
}

inline real SimEB::Integrate(TXYZ const & xyz_p, TXYZ const & xyz_v,
		      real const r_dt, real & r_impact)
{
  // Calculate speed and initial radius.
  real const r_v2 = xyz_v * xyz_v; real const r_v = sqrt(r_v2);
  real const r_p2 = xyz_p * xyz_p; real const r_p = sqrt(r_p2);

  real const r_da = r_v * r_dt;

  // Calculate a vector and ai, af.
  TXYZ xyz_an;
  if (r_v == 0.0) xyz_an = xyz_p / r_p;
  else xyz_an = xyz_v / r_v;
  real const r_ai = xyz_p * xyz_an;
  real const r_ai_abs = fabs(r_ai);
  real const r_dtdt = 0.5 * r_dt * r_dt;

  if (r_da < rSmall*r_ai_abs) {
    r_impact = r_p;
    return rScaleFactor * r_dtdt / r_p2;
  }

  real const r_ai2 = r_ai*r_ai;
  real const r_af = r_ai + r_da;
  real const r_af_abs = fabs(r_af);
  real const r_a_abs = r_ai_abs + r_af_abs;

  // Calculate b vector and b.
  real const r_b2 = r_p2 - r_ai2;
  real const r_b = sqrt(r_b2);

  real const r_ri = r_p; real r_ri_inv = 1/r_ri;
  real const r_rf = sqrt(r_af*r_af+r_b2);

  if (r_af < 0.0) r_impact = r_rf;
  else if (r_ai > 0.0) r_impact = r_ri;
  else r_impact = r_b;

  real r_ap;

  // NOTE: we may lose numerical accuracy if ai<0<af and the a perturbation
  // nearly cancels, but there is not much we can do about it.
  if (r_a_abs < rSmall*r_b)
    r_ap = r_dt*r_dt * (r_af_abs + r_ai_abs+r_ai_abs) / (6.0*r_b2*r_b);
  else if (r_af < 0.0)
    r_ap = (-log(r_af_abs+r_rf) + log(r_ai_abs+r_ri) - r_da*r_ri_inv) / r_v2;
  else if (r_ai > 0.0)
    r_ap = (-log(r_af_abs+r_rf) + log(r_ai_abs+r_ri) + r_da*r_ri_inv) / r_v2;
  else {
    if (r_b == 0.0) {
      r_impact = r_af <? -r_ai;
      return rEpsilon * r_impact;
    }

    if (r_af < -r_ai)
      r_ap = (log(r_af_abs+r_rf) + log(r_ai_abs+r_ri) - log(r_b2) -
	      r_da*r_ri_inv) / r_v2;
    else {
      // -(ai, -ai) + (-ai, af).
      r_ap = (-log(r_af+r_rf) + 3.0*log(-r_ai+r_ri) - log(r_b2) +
	      (r_af + r_ai+r_ai+r_ai)*r_ri_inv) / r_v2;
    }
  }

  real r_bp;
  if (r_da < rSmall*r_b) {
    real const r_p3 = r_p * r_p2;
    r_bp = r_dtdt * r_b / r_p3;
  }
  else {
    // If ai,af >> b, use Taylor series to avoid 1/b numerical difficulties.
    // Use of <= catches the b==0 case.
    if (r_b <= rSmall*r_ai_abs && r_b <= rSmall*r_af_abs) {
      // Note it is impossible for ai==0 || af==0 at this point.

      if (r_af < 0)
	r_bp = (-r_dtdt*r_b) / (r_ai2*r_af);
      else if (r_ai > 0)
	r_bp = (r_dtdt*r_b) / (r_ai2*r_af);
      else { // if (ai <= 0 <= af)...
	if (r_b == 0.0) {
	  r_impact = r_af <? -r_ai;
	  return rEpsilon * r_impact;
	}

	r_bp = (r_af+r_af) / (r_v2 * r_b);
      }
    }
    else // r_b is not too small...
      r_bp = (r_rf - r_ri - r_ai*r_da*r_ri_inv) / (r_v2 * r_b);
  }

  return rScaleFactor * (r_ap + r_bp);
}

inline void SimEB::Integrate(TXYZ & xyz_p, TXYZ & xyz_v, real const r_dt)
{
  // Calculate speed and initial radius.
  real const r_v2 = xyz_v * xyz_v; real const r_v = sqrt(r_v2);
  real const r_p2 = xyz_p * xyz_p; real const r_p = sqrt(r_p2);

  real const r_da = r_v * r_dt;

  // Calculate a vector and ai, af.
  TXYZ xyz_an;
  if (r_v == 0.0) xyz_an = xyz_p / r_p;
  else xyz_an = xyz_v / r_v;
  real const r_ai = xyz_p * xyz_an;
  real const r_ai_abs = fabs(r_ai);

  if (r_da < rSmall*r_ai_abs) {
    real const r_dq = xyz_p * xyz_v;
    TXYZ xyz_j = xyz_v - xyz_p * (3*r_dq/r_p2);

    real r_dt1 = rScaleFactor * r_dt / (r_p*r_p2);
    real r_dt2 = r_dt1 * r_dt / 2;
    real r_dt3 = r_dt2 * r_dt / 3;

    xyz_v = xyz_p * r_dt1 + xyz_j * r_dt2;
    xyz_p = xyz_p * r_dt2 + xyz_j * r_dt3;
    return;
  }

  real const r_af = r_ai + r_da;
  real const r_af_abs = fabs(r_af);
  real const r_a_abs = r_ai_abs + r_af_abs;
  TXYZ const xyz_ai = r_ai * xyz_an;

  // Calculate b vector and b.
  TXYZ const xyz_b = xyz_p - xyz_ai;
  real const r_b2 = xyz_b * xyz_b;
  real const r_b = sqrt(r_b2);

  real const r_ri = r_p; real const r_ri_inv = 1/r_ri;
  real const r_rf = sqrt(r_af*r_af+r_b2); real const r_rf_inv = 1/r_rf;

  // NOTE: we may lose numerical accuracy if af==-ai, but there is
  // not much we can do about it.
  real const r_a_sum = r_af + r_ai;

  real r_av;
  if (r_a_abs < rSmall*r_b)
    r_av = r_a_sum * r_dt / (2*r_b2*r_b);
  else if (fabs(r_a_sum) < rSmall*r_a_abs)
    r_av = -r_ai*r_a_sum / (r_v * r_p2 * r_p);
  else
    r_av = (-r_rf_inv + r_ri_inv) / r_v;

  // NOTE: we may lose numerical accuracy if ai<0<af and the perturbation
  // nearly cancels, but there is not much we can do about it.
  real r_ap;

  if (r_a_abs < rSmall*r_b)
    r_ap = r_dt*r_dt * (r_a_sum + r_ai) / (6.0*r_b2*r_b);
  else if (r_af < 0.0)
    r_ap = (log(-r_af+r_rf) - log(-r_ai+r_ri) + r_da*r_ri_inv) / r_v2;
  else if (r_ai > 0.0)
    r_ap = (-log(r_af+r_rf) + log(r_ai+r_ri) + r_da*r_ri_inv) / r_v2;
  else
    r_ap =
      (-log(r_af+r_rf) - log(-r_ai+r_ri) + r_da*r_ri_inv + log(r_b2)) / r_v2;

  real r_bv, r_bp;
  if (r_da <= rSmall*r_b) {
    // Catches r_v == 0.0.
    real const r_p3 = r_p * r_p2;
    real const r_dtp3 = r_dt / r_p3;
    r_bv = r_dtp3 * r_b;
    r_bp = 0.5 * r_dt * r_bv;
  }
  else {
    // If ai,af >> b, use Taylor series to avoid 1/b numerical difficulties.
    // Use of <= catches the b==0 case.
    if (r_b <= rSmall*r_ai_abs && r_b <= rSmall*r_af_abs) {

      // Note it is impossible for ai==0 || af==0 at this point.
      if (r_af < 0) {
	r_bv = (0.5*r_b/r_v) * (1/(r_af*r_af) - 1/(r_ai*r_ai));
	r_bp = (-0.5*r_b*r_dt*r_dt) / (r_ai*r_ai*r_af);
      }
      else if (r_ai > 0) {
	r_bv = (-0.5*r_b/r_v) * (1/(r_af*r_af) - 1/(r_ai*r_ai));
	r_bp = (0.5*r_b*r_dt*r_dt) / (r_ai*r_ai*r_af);
      }
      else { // if (ai <= 0 <= af)...
	r_bv = 2 / (r_v * r_b);
	r_bp = (r_af / r_v) * r_bv;
      }
    }
    else { // r_b is not too small...
      r_bv = (r_af*r_rf_inv - r_ai*r_ri_inv) / (r_v * r_b);
      r_bp = (r_rf - r_ri - r_ai*r_da*r_ri_inv) / (r_v2 * r_b);
    }
  }

  real r_b_inv;
  if (r_b > 0.0) r_b_inv = 1/r_b; else r_b_inv = 0.0;
  TXYZ const xyz_bn = xyz_b * r_b_inv;

  r_av *= rScaleFactor; r_bv *= rScaleFactor;
  r_ap *= rScaleFactor; r_bp *= rScaleFactor;

  xyz_v = r_av*xyz_an + r_bv*xyz_bn;
  xyz_p = r_ap*xyz_an + r_bp*xyz_bn;
}


inline void SimEB::HeapFilterDown(Particle * const particle_p)
{
  real const r_tf = particle_p->rTf;
  int i = particle_p->iTOrder;
  int j = i;

  for (;;) {
    real rTmin = r_tf;

    int k = i+i+1;
    if (k < iNumGenerated) if (ParticleHeap[k]->rTf < rTmin)
      {j = k; rTmin = ParticleHeap[k]->rTf;}

    ++k;
    if (k < iNumGenerated) if (ParticleHeap[k]->rTf < rTmin)
      {j = k; rTmin = ParticleHeap[k]->rTf;}

    if (j == i) break;

    ParticleHeap[i] =  ParticleHeap[j];
    ParticleHeap[i]->iTOrder = i;

    i = j;
  }

  ParticleHeap[j] = particle_p;
  particle_p->iTOrder = j;
}

inline void SimEB::HeapFilterUp(Particle * const particle_p)
{
  real const r_tf = particle_p->rTf;
  int i = particle_p->iTOrder;

  // Ignore the topmost level!
  while (i>2) {
    int j = (i-1)/2;

    if (r_tf >= ParticleHeap[j]->rTf) break;
    
    ParticleHeap[i] = ParticleHeap[j];
    ParticleHeap[i]->iTOrder = i;

    i = j;
  }

  ParticleHeap[i] = particle_p;
  particle_p->iTOrder = i;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// ArrayDelete()
//
inline void SimEB::ArrayDelete(Particle * const particle_p)
{
  // Compact <ParticleArray>.
  for (int i2 = particle_p->iZOrder-1; i2 >= iNumKilled; --i2) {
    Particle * const particle_q = ParticleArray[i2];
    ParticleArray[i2+1] = particle_q; ++particle_q->iZOrder;
  }

  ParticleArray[iNumKilled] = particle_p;
  particle_p->iZOrder = iNumKilled;
}



void SimEB::Halt(void)
{
  cout << "Halt!  ";
  cout << "Iteration " << iIteration << space << space;
  cout << "Particles " << iNumActivated-iNumKilled << newline;

  real r_ti = 0.0;
  for (int i = iNumKilled; i < iNumActivated; ++i)
    r_ti >?= ParticleArray[i]->rTi;

  for (int i = iNumKilled; i < iNumActivated; ++i)
    ParticleArray[i]->rTf = r_ti;

  for (int i = iNumKilled; i < iNumActivated; ++i) {
    Particle * const particle_p = ParticleArray[i];

    int const i_min = i - iGetSphere() >? iNumKilled;
    int const i_max = i + iGetSphere() <? (iNumActivated-1);

    for (int j = i_min; j < i; ++j) Commit(particle_p, ParticleArray[j]);
    for (int j = i+1; j <= i_max; ++j) Commit(particle_p, ParticleArray[j]);

    particle_p->Commit();
    particle_p->rTi = particle_p->rTf;
  }

  real r_ke = 0.0;
  for (int i = iNumKilled; i < iNumActivated; ++i) {
    Particle const * const particle_p = ParticleArray[i];
    r_ke += particle_p->xyzVelocity * particle_p->xyzVelocity;
  }

  r_ke *= 0.5 * rM / rQ;
  r_ke /= iNumActivated - iNumKilled;
  r_ke -= Column::GetSource()->rGetVoltage();
  
  real r_pe = 0.0;
  for (int i = iNumKilled; i < iNumActivated; ++i)
    for (int j = i+1; j < iNumActivated; ++j) {
      Particle const * const particle_p = ParticleArray[i];
      Particle const * const particle_q = ParticleArray[j];
      TXYZ xyz_dp = particle_p->xyzPosition - particle_q->xyzPosition;
      r_pe += 1 / sqrt(xyz_dp * xyz_dp);
    }
  r_pe *= rCoulomb * rQ * rQ;
  r_pe /= rQ;
  r_pe /= iNumActivated - iNumKilled;

  cout << "KE = " << r_ke << space;
  cout << "PE = " << r_pe << space;
  cout << "TE = " << r_ke + r_pe << newline;
  
  exit(0);
}

