#ifndef _PARTICLE
#define _PARTICLE

class Particle
{
 public:
  int iPlane;
  int iId;
  int iBeam;

  #ifdef _SIMEB_CAP_GROUP
  int iGroup;
  #endif

  #ifdef _SIMEB_CAP_ACCEL
  real rFieldA;
  #endif

  #ifdef _SIMEB_CAP_MIRROR
  int iMirror;
  #endif

  inline real Impact(real const r_dz);
  inline bool Cross(real const r_dz);
  inline bool bIncrPlane(void);

  static int EmissionOrder(Particle const * const *, Particle const * const *);
  Particle();

  TXYZ    xyzP;
  TXYZ    xyzV;
  TXYZ    xyzPCommit;
  TXYZ    xyzVCommit;

  real    rPert;
  real    rTi, rTf, rDt;

  int     iTOrder;
  int     iZOrder;

  #ifndef _SIMEB_CAP_ACCEL
  static void rSetFieldA(real) {};
  static real rGetFieldA() {return 0.0;};
  #endif

  #ifdef _SIMEB_CAP_ACCEL
  void rSetFieldA(real r_field_a) {rFieldA = r_field_a;};
  real rGetFieldA() const {return rFieldA;};
  #endif

  #ifndef _SIMEB_CAP_GROUP
  static int  iGetGroup(void) {return 0;};
  static void iSetGroup(int) {};
  static void iClearGroup(int) {};
  #endif
 
  #ifdef _SIMEB_CAP_GROUP
  int  iGetGroup(void) const {return iGroup;};
  void iSetGroup(int i_group) {iGroup |= i_group;};
  void iClearGroup(int i_group) {iGroup &= ~i_group;};
  #endif

  int iGetPlane(void) const {return iPlane;};
  int iGetId(void)    const {return iId;};
  int iGetBeam(void)  const {return iBeam;};

  void iSetId(int const i_id) {iId = i_id;};
  void iSetBeam(int const i_beam) {iBeam = i_beam;};

  inline void Commit(void);
  inline void Fly(TXYZ & xyz_p, TXYZ & xyz_v, real const r_dt) const;
  inline void FlyZ(real & r_pz, real & r_vz, real const r_dt) const;
  inline void TimeStepIncr(real const r_epsilon);
  inline void TimeStepDecr(real const r_epsilon);
  inline void TimeStepTrunc(real const r_ti, real const r_pert, real const r_epsilon);
  inline bool NextEvent(void);
};


inline void Particle::Commit(void)
{
  Fly(xyzP, xyzV, rTf - rTi);

  xyzP += xyzPCommit;
  xyzV += xyzVCommit;
  xyzPCommit = xyzVCommit = xyz0;
}

inline void Particle::Fly(TXYZ & xyz_p, TXYZ & xyz_v, real const r_dt) const {
  if (rGetFieldA() == 0.0) xyz_p += r_dt * xyz_v;
  else {
    xyz_p.rX += r_dt * xyz_v.rX;
    xyz_p.rY += r_dt * xyz_v.rY;
    FlyZ(xyz_p.rZ, xyz_v.rZ, r_dt);
  }
};


inline void Particle::FlyZ(real & r_pz, real & r_vz, real const r_dt) const {
  if (rGetFieldA() == 0.0) {r_pz += r_dt*r_vz; return;}
  
  r_pz += r_vz * r_dt + 0.5 * rGetFieldA() * r_dt*r_dt;
  r_vz += rGetFieldA() * r_dt;
};

inline void Particle::TimeStepIncr(real const r_epsilon) {
  if (rPert == 0.0) rDt = FLT_MAX;
  else if (rDt < 2*(rTf-rTi)) rDt *= sqrt(r_epsilon / rPert);

  rPert = 0.0;
  rTi = rTf;
};

inline void Particle::TimeStepDecr(real const r_epsilon) {
  rDt = (rTf-rTi) / (1 + sqrt(rPert / r_epsilon));

  rPert = 0.0;
  rTf = rTi;
};


inline void Particle::TimeStepTrunc(real const r_ti, real const r_pert, real const r_epsilon) {
  rDt = (rTf-rTi) / (1 + sqrt(rPert / r_epsilon));

  rPert -= r_pert;
  rTf = r_ti;
};

inline bool Particle::NextEvent(void) {
  real r_dz;

  while ((r_dz = Column::rGetZ(iPlane) - xyzP.rZ) < Column::rGetCapture())
    if (!Cross(r_dz)) return false;

  rTf += rDt <? Impact(r_dz);

  return true;
};

inline bool Particle::Cross(real const r_dz)
{
  real r_dt = Impact(r_dz);

  Fly(xyzP, xyzV, r_dt);

  if (!Column::GetPlane(iPlane)->Cross(this)) return false;
  if (!bIncrPlane()) return false;

  Fly(xyzP, xyzV, -r_dt);

  return true;
};


inline real Particle::Impact(real const r_dz) {
  if (r_dz == 0.0) return 0.0;

  const real r_vz = xyzV.rZ;

  real r_dt;
  if (rGetFieldA() == 0.0) r_dt = r_dz / r_vz;
  else r_dt = 2*r_dz / (r_vz + sqrt(r_vz*r_vz + 2*rGetFieldA()*r_dz));

  return r_dt;
};


inline bool Particle::bIncrPlane(void)
{
  TPlane * plane = Column::GetPlane(iPlane);

  rSetFieldA(plane->rGetFieldA());

  iSetGroup(plane->iGetSetGroup());
  iClearGroup(plane->iGetClearGroup());

  if (++iPlane == Column::iGetNumPlanes()) return false;
  return true;
};

#endif
