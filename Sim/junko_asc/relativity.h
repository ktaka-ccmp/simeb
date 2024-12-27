#ifndef _RELATIVITY
#define _RELATIVITY

#define C          (2.98E8)
#define C2         (C*C)
#define C2INV      (1/C2)

class Relativity {
public:
  static real rQM;
  inline static void rSetQM(real r_qm) {rQM = r_qm;};

  inline const static real rCalcVelocity(real r_voltage);
  inline const static real rCalcVelocity(real r_voltage, real r_alpha);
  inline const static real rCalcVoltage(real r_velocity);
  inline const static real rCalcGammaFromVelocity(real r_velocity);
  inline const static real rCalcGammaFromVoltage(real r_voltage);
};

inline const real Relativity::rCalcVelocity(real r_voltage) {
  real r_velocity;

  if (!CAP_bRelativity)
    return sqrt(2 * r_voltage * rQM);

  if (CAP_bRelativity) {
    real r_gamma = rCalcGammaFromVoltage(r_voltage);
    return C * sqrt((1 - 1 / (r_gamma*r_gamma)));
  }
};

inline const real Relativity::rCalcVelocity(real r_voltage, real r_alpha) {
  return sqrt((2 * r_voltage * rQM) / (1 + r_alpha*r_alpha));
}

inline const real Relativity::rCalcVoltage(real r_velocity) {
  real r_voltage;

  if (!CAP_bRelativity)
    r_voltage = r_velocity*r_velocity / (2 * rQM);

  if (CAP_bRelativity)
    r_voltage = (rCalcGammaFromVelocity(r_velocity)-1) * C2 / rQM;
  
  return r_voltage;
}

inline const real Relativity::rCalcGammaFromVelocity(real r_velocity) {
  if (!CAP_bRelativity) return 1.0;
  if (CAP_bRelativity) return 1 / sqrt(1-r_velocity*r_velocity/C2);
};

inline const real Relativity::rCalcGammaFromVoltage(real r_voltage) {
  if (!CAP_bRelativity) return 1.0;
  if (CAP_bRelativity) return 1.0 + r_voltage * rQM / C2;
};

#endif
