#include "simeb.h"

int  TPlane::iPlaneCounter = 0;
bool TPlane::bRayTrace = false;

TPlane * TPlane::Parse(TParser &parser)
{
  TPlane * plane = NULL;
  char s_token[MAX_TOKEN];

  parser >> s_token;
  if (!strcmp(s_token, "Target")) plane = new TTarget();
  if (!strcmp(s_token, "Aperture")) plane = new TAperture();
  if (!strcmp(s_token, "ThinLens")) plane = new TThinLens();
  if (!strcmp(s_token, "EELens")) plane = new EELens();
  if (!plane) parser.Error();

  parser >> s_token;
  if (strcmp(s_token, "{")) parser.Error();

  for (;;) {
    parser >> s_token;
    if (parser.Eof()) parser.Error();
    if (!strcmp(s_token, "}")) break;
    plane->Parse(parser, s_token);
  }
  return plane;
}

// Generic plane constructor.
TPlane::TPlane(void)
{
  iPlane = TPlane::iPlaneCounter++;

  *sName = 0;
  rZ = 0.0;

  iGroup = 0;
  iSetGroup = 0;
  iClearGroup = 0;

  bVoltageSpec = false;
  rVoltage = 0.0;
}


void TPlane::Parse(TParser &parser, char *s_token)
{
  // Read in parameters.
  if (!strcmp(s_token, "Name")) parser >> sName;
  else if (!strcmp(s_token, "Z")) parser >> rZ;
  else if (!strcmp(s_token, "Electrode"))
    {parser >> rVoltage; bVoltageSpec = true;}
  else if (!strcmp(s_token, "Group")) parser >> iGroup;
  else if (!strcmp(s_token, "SetGroup")) parser >> iSetGroup;
  else if (!strcmp(s_token, "ClearGroup")) parser >> iClearGroup;
  else if (!strcmp(s_token, "RayTrace")) {parser >> bRayTrace;}
  else parser.Error();
}

void TPlane::Initialize(void)
{
}

// Dump particle information to the output file.
bool TPlane::Cross(Particle * const particle_p)
{
  if (bRayTrace) if (!(particle_p->iGetGroup() & ~iGroup))
    cout << particle_p->xyzPosition.rZ << space
         << particle_p->xyzPosition.rX << space
	 << particle_p->xyzPosition.rY << space
	 << particle_p->xyzVelocity.rX / particle_p->xyzVelocity.rZ << space
	 << particle_p->xyzVelocity.rY / particle_p->xyzVelocity.rZ << newline;

  return true;
}

void TPlane::Trajectory(Particle * particle_p, TXY const & xy_v_normal)
{
  particle_p->xyzVelocity.rZ = sqrt(particle_p->xyzVelocity * particle_p->xyzVelocity) / sqrt(1 + xy_v_normal*xy_v_normal);

  particle_p->xyzVelocity.rX = xy_v_normal.rX * particle_p->xyzVelocity.rZ;
  particle_p->xyzVelocity.rY = xy_v_normal.rY * particle_p->xyzVelocity.rZ;
}

TTarget::TTarget() : TPlane()
{
  TargetFile = NULL;

  iNumParticles = 0;
  iTotalParticles = 0;
  iNumTrailers = 0;
}


void TTarget::Parse(TParser &parser, char * s_token)
{
  if (!strcmp(s_token, "Trailers")) parser >> iNumTrailers;
  else TPlane::Parse(parser, s_token);
}

void TTarget::Initialize(void)
{
  TPlane::Initialize();

  iTotalParticles = TSource::iGetTotalParticles();

  char s_filename[MAX_TOKEN];
  strcpy(s_filename, SimEB::sGetName());
  strcat(s_filename, ".");

  if (*sGetName()) strcat(s_filename, sGetName());
  else strcat(s_filename, "target");

  // Change by ktaka starts here.
  strcpy(out_filename, s_filename);
  cout << out_filename << endl;

  ofstream outfile(out_filename);
  if (!outfile) cerr << out_filename << " read open error." << endl;

//  TargetFile = fopen(s_filename, "wb");
//  if (!TargetFile) {
//    fprintf(stderr, "Cannot open file %s.\n", s_filename);
//   exit(0);
//  }
  // Change by ktaka ends here.
}

bool TTarget::Cross(Particle * const particle_p)
{
  bool b = TPlane::Cross(particle_p);

  ++iNumParticles;
  if (iNumParticles-1 < iNumTrailers) return b;
  if (iTotalParticles - iNumParticles < iNumTrailers) return b;
  if (particle_p->iGetGroup() & ~iGroup) return b;

  int i;
  float f;

  // Change by ktaka starts here.
  ofstream outfile(out_filename,ios::app);
  if (!outfile) cerr << out_filename << " read open error." << endl;

  outfile << setiosflags(ios::left) << setw(10) << particle_p->iGetId();
  outfile << setiosflags(ios::left) << setw(5) << particle_p->iGetBeam();
  outfile << setiosflags(ios::fixed) << setprecision(10) ;
  outfile << setiosflags(ios::left) << setw(15) << particle_p->xyzPosition.rX;
  outfile << setiosflags(ios::left) << setw(15) << particle_p->xyzPosition.rY;
  outfile << setiosflags(ios::left) << setw(15) << -1*particle_p->xyzPosition.rZ;
  outfile << setiosflags(ios::fixed) << setprecision(5) ;
  outfile << setiosflags(ios::left) << setw(15) << particle_p->xyzVelocity.rX;
  outfile << setiosflags(ios::left) << setw(15) << particle_p->xyzVelocity.rY;
  outfile << setiosflags(ios::left) << setw(15) << particle_p->xyzVelocity.rZ << endl;
  
//  i = particle_p->iGetId(); fwrite(&i, sizeof(i), 1, TargetFile);
//  i = particle_p->iGetBeam(); fwrite(&i, sizeof(i), 1, TargetFile);

//  f = particle_p->xyzPosition.rX; fwrite(&f, sizeof(f), 1, TargetFile);
//  f = particle_p->xyzPosition.rY; fwrite(&f, sizeof(f), 1, TargetFile);

//  f = particle_p->xyzVelocity.rX; fwrite(&f, sizeof(f), 1, TargetFile);
//  f = particle_p->xyzVelocity.rY; fwrite(&f, sizeof(f), 1, TargetFile);
//  f = particle_p->xyzVelocity.rZ; fwrite(&f, sizeof(f), 1, TargetFile);
  // Change by ktaka ends here.

  return b;
}

void TAperture::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Shape")) Shape = TShape::Parse(parser);
  else TPlane::Parse(parser, s_token);
}

// Process a particle crossing a TAperture.
bool TAperture::Cross(Particle * const particle_p)
{
  if (!TPlane::Cross(particle_p)) return false;
  return Shape->bInside((TXY)particle_p->xyzPosition);
}

// Create a TThinLens.
TThinLens::TThinLens(void) : TPlane()
{
  rFocalLength = 0.0;
}


// Parse arguments for TThinLens.
void TThinLens::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "FocalLength")) parser >> rFocalLength;
  else TPlane::Parse(parser, s_token);
}

// Process a particle crossing a TThinLens.
bool TThinLens::Cross(Particle * const particle_p)
{
  if (!TPlane::Cross(particle_p)) return false;

  const TXY xy_p = (TXY)particle_p->xyzPosition;
  const TXY xy_v = (TXY)particle_p->xyzVelocity;
  const TXY xy_v_normal = xy_v / particle_p->xyzVelocity.rZ - xy_p / rFocalLength;
  Trajectory(particle_p, xy_v_normal);
  return true;
}

int    EELens::iMode = EELENS_OFF;
int    EELens::iGroup = 0;
FILE * EELens::pFile = NULL;
int *  EELens::LensMap = NULL;
real * EELens::CurrentMap = NULL;
int    EELens::iNumLens = 0;
int    EELens::iNumBeams = 0;
int    EELens::iNumElements = 0;
int    EELens::iNumSegments = 1;
int    EELens::iLensCounter = 0;

EELens::EELens(void) : TTarget()
{
  iLens = EELens::iLensCounter++;

  rMaxRadius = NULL;
  rStepRadius = NULL;
  LensArray = NULL;
}

void EELens::Initialize(void)
{
  if (EELens::iMode == EELENS_OFF) {
    TPlane::Initialize();
    return;
  }

  if (EELens::iMode == EELENS_TARGET) {
    sprintf(sName, "eeLens.%d.target", EELens::iLens);
    TTarget::iGroup = EELens::iGroup;
    TTarget::Initialize();
    return;
  }

  TPlane::Initialize();

  EELens::LensMap[iLens] = iGetPlane();

  LensAxis = new TXY[iNumBeams];
  rMaxRadius = new real[iNumBeams];
  rStepRadius = new real[iNumBeams];
  LensArray = new real *[iNumBeams];

  real r_velocity = Relativity::rCalcVelocity(rGetVoltage());

  for (int i = 0; i < iNumBeams; ++i) {
    float f1, f2, f3;
    fscanf(EELens::pFile, "%f %f %f", &f1, &f2, &f3);
    LensAxis[i].rX = f1;
    LensAxis[i].rY = f2;
    rMaxRadius[i] = f3;
    rStepRadius[i] = rMaxRadius[i] / iNumElements;

    real r_current = EELens::CurrentMap[i];
    real r_scale = SimEB::rGetQM() * r_current / r_velocity;

    LensArray[i] = new real[iNumElements+1];
    LensArray[i][0] = 0.0;
    for (int j = 1; j <= iNumElements; ++j) {
      float f;
      fscanf(EELens::pFile, "%f", &f);
      LensArray[i][j] = ((real)f) * r_scale;
    }
  }
}

bool EELens::Cross(Particle * const particle_p)
{
  if (EELens::iMode == EELENS_OFF) return TPlane::Cross(particle_p);
  if (EELens::iMode == EELENS_TARGET) return TTarget::Cross(particle_p);

  if (!TPlane::Cross(particle_p)) return false;

  TXY const xy_p0 = (TXY)particle_p->xyzPosition;
  TXY const xy_v0 = (TXY)particle_p->xyzVelocity;

  TXY xy_dp = xy0;
  TXY xy_dv = xy0;

  if (iLens > 0) {
    EELens * eelens = (EELens *)Column::GetPlane(EELens::LensMap[iLens-1]);

    real const r_dt = (rGetZ() - eelens->rGetZ()) / 2 / particle_p->xyzVelocity.rZ;
    real const r_dt1 = r_dt / iNumSegments;
    real const r_dt2 = r_dt1 * r_dt1 / 2;
    real const r_dt3 = r_dt2 * r_dt1 / 3;

    if (r_dt > 0.0) for (int i = -iNumSegments; i < 0; ++i) {
      TXY const xy_p1 = xy_p0 + i * r_dt1 * xy_v0;
      real r_mix1 = - ((real)i) / (2 * iNumSegments);
      TXY const xy_a1 = r_mix1       * eelens->LensLookup(xy_p1) +
	                (1.0-r_mix1) * LensLookup(xy_p1);

      TXY const xy_p2 = xy_p0 + (i+1) * r_dt1 * xy_v0;
      real r_mix2 = - ((real)(i+1)) / (2 * iNumSegments);
      TXY const xy_a2 = r_mix2       * eelens->LensLookup(xy_p2) +
                        (1.0-r_mix2) * LensLookup(xy_p2);

      TXY const xy_b = (xy_a2 - xy_a1) / r_dt1;

      xy_dp += xy_dv*r_dt1 + xy_a1*r_dt2 + xy_b*r_dt3;
      xy_dv += xy_a1*r_dt1  + xy_b*r_dt2;
    }
  }

  if (iLens < EELens::iLensCounter-1) {
    EELens * eelens = (EELens *)Column::GetPlane(EELens::LensMap[iLens+1]);

    real const r_dt = (eelens->rGetZ() - rGetZ()) / 2 / particle_p->xyzVelocity.rZ;
    real const r_dt1 = r_dt / iNumSegments;
    real const r_dt2 = r_dt1 * r_dt1 / 2;
    real const r_dt3 = r_dt2 * r_dt1 / 3;

    if (r_dt > 0.0) for (int i = 1; i <= iNumSegments; ++i) {
      TXY const xy_p1 = xy_p0 + (i-1) * r_dt1 * xy_v0;
      real r_mix1 = ((real)(i-1)) / (2 * iNumSegments);
      TXY const xy_a1 = r_mix1       * eelens->LensLookup(xy_p1) +
	                (1.0-r_mix1) * LensLookup(xy_p1);

      TXY const xy_p2 = xy_p0 + i * r_dt1 * xy_v0;
      real r_mix2 = ((real)i) / (2 * iNumSegments);
      TXY const xy_a2 = r_mix2       * eelens->LensLookup(xy_p2) +
                        (1.0-r_mix2) * LensLookup(xy_p2);

      TXY const xy_b = (xy_a2 - xy_a1) / r_dt1;

      xy_dp += xy_dv*r_dt1 + xy_a1*r_dt2 + xy_b*r_dt3;
      xy_dv += xy_a1*r_dt1  + xy_b*r_dt2;
    }

    particle_p->xyzPosition -= xy_dv * r_dt;
  }

  particle_p->xyzVelocity += xy_dv;
  particle_p->xyzPosition += xy_dp;

  return true;
}

TXY EELens::LensLookup(TXY xy_p)
{
  TXY xy_a = xy0;

  for (int i = 0; i < EELens::iNumBeams; ++i) {
    TXY xy_p_rel = xy_p - LensAxis[i];
    real const r_p = sqrt(xy_p_rel * xy_p_rel);

    real r_a;
  
    if (r_p < rMaxRadius[i]) {
      int const j = (int)(r_p / rStepRadius[i]);
      real const r = (r_p - j * rStepRadius[i]) / rStepRadius[i];
    
      r_a = (1-r) * LensArray[i][j] + r * LensArray[i][j+1];
    }
    else r_a = LensArray[i][iNumElements] * rMaxRadius[i] / r_p;

    if (r_p != 0.0) xy_a += xy_p_rel * (r_a / r_p);
  }

  return xy_a;
}
