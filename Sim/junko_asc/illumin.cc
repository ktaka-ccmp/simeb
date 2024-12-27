#include "simeb.h"

TIllumin * TIllumin::Parse(TParser &parser)
{
  TIllumin * illumin = NULL;
  char s_token[MAX_TOKEN];

  parser >> s_token;
  if (!strcmp(s_token, "Uniform")) illumin = new TIlluminUniform();
  if (!strcmp(s_token, "Lumped")) illumin = new TIlluminLumped();
  if (!strcmp(s_token, "Gaussian")) illumin = new TIlluminGaussian();
  if (!strcmp(s_token, "Fixed")) illumin = new TIlluminFixed();
  if (!illumin) parser.Error();

  parser >> s_token;
  if (strcmp(s_token, "{")) parser.Error();

  for (;;) {
    parser >> s_token;
    if (parser.Eof()) parser.Error();
    if (!strcmp(s_token, "}")) break;
    illumin->Parse(parser, s_token);
  }
  return illumin;
}

void TIllumin::Parse(TParser &parser, char *s_token) {parser.Error();}


TIlluminUniform::TIlluminUniform(void) : TIllumin()
  {rAlpha = 0.0; rDeltaE = 0.0;}

void TIlluminUniform::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Alpha")) parser >> rAlpha;
  else if (!strcmp(s_token, "DeltaE")) parser >> rDeltaE;
  else TIllumin::Parse(parser, s_token);
}
void TIlluminUniform::Randomize(Particle * particle_p, real r_energy)
{
  real r_totalenergy = r_energy - rDeltaE * log(drand48());
  real r_alpha= rAlpha * sqrt(drand48());
  real r_phi = 2*M_PI*drand48();
  
  particle_p->xyzVelocity.rZ = Relativity::rCalcVelocity(r_totalenergy, r_alpha);
  particle_p->xyzVelocity.rX = cos(r_phi) * r_alpha * particle_p->xyzVelocity.rZ;
  particle_p->xyzVelocity.rY = sin(r_phi) * r_alpha * particle_p->xyzVelocity.rZ;
}

TIlluminLumped::TIlluminLumped(void) : TIllumin()
  {rAlpha = 0.0; rDeltaE = 0.0;}

void TIlluminLumped::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Alpha")) parser >> rAlpha;
  else if (!strcmp(s_token, "DeltaE")) parser >> rDeltaE;
  else TIllumin::Parse(parser, s_token);
}
void TIlluminLumped::Randomize(Particle * particle_p, real r_energy)
{
  real r_totalenergy = r_energy - rDeltaE * log(drand48());
  real r_alpha= rAlpha * drand48();
  real r_phi = 2*M_PI*drand48();
  
  particle_p->xyzVelocity.rZ = Relativity::rCalcVelocity(r_totalenergy, r_alpha);
  particle_p->xyzVelocity.rX = cos(r_phi) * r_alpha * particle_p->xyzVelocity.rZ;
  particle_p->xyzVelocity.rY = sin(r_phi) * r_alpha * particle_p->xyzVelocity.rZ;
}

TIlluminGaussian::TIlluminGaussian(void) : TIllumin()
  {rAlpha = 0.0; rDeltaE = 0.0;}

void TIlluminGaussian::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Alpha")) parser >> rAlpha;
  else if (!strcmp(s_token, "DeltaE")) parser >> rDeltaE;
  else TIllumin::Parse(parser, s_token);
}
void TIlluminGaussian::Randomize(Particle * particle_p, real r_energy)
{
  real r_totalenergy = r_energy - rDeltaE * log(drand48());
  real r_alpha = rAlpha * sqrt(-log(1-drand48()));
  real r_phi = 2*M_PI*drand48();
  
  particle_p->xyzVelocity.rZ = Relativity::rCalcVelocity(r_totalenergy, r_alpha);
  particle_p->xyzVelocity.rX = cos(r_phi) * r_alpha * particle_p->xyzVelocity.rZ;
  particle_p->xyzVelocity.rY = sin(r_phi) * r_alpha * particle_p->xyzVelocity.rZ;
}

TIlluminFixed::TIlluminFixed(void) : TIllumin()
 {xyAlpha = xy0;}

void TIlluminFixed::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Alpha")) parser >> xyAlpha;
  else TIllumin::Parse(parser, s_token);
}

void TIlluminFixed::Randomize(Particle * particle_p, real r_energy)
{
  particle_p->xyzVelocity.rZ = Relativity::rCalcVelocity(r_energy, sqrt(xyAlpha*xyAlpha));
  particle_p->xyzVelocity.rX = xyAlpha.rX * particle_p->xyzVelocity.rZ;
  particle_p->xyzVelocity.rY = xyAlpha.rY * particle_p->xyzVelocity.rZ;
}
