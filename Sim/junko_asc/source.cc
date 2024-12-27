#include "simeb.h"

int TSource::iTotalParticles = 0;
Particle * TSource::ParticleDatabase = NULL;

int        TSource::iGetTotalParticles(void) {return iTotalParticles;};
Particle * TSource::GetParticle(int i) {return &ParticleDatabase[i];};

TSource * TSource::Parse(TParser &parser)
{
  TSource * source = NULL;
  char s_token[MAX_TOKEN];

  parser >> s_token;
  if (!strcmp(s_token, "Array")) source = new TSourceArray();
  if (!strcmp(s_token, "EELens")) source = new TSourceEELens();
  if (!strcmp(s_token, "Grid")) source = new TSourceGrid();
  if (!source) parser.Error();

  parser >> s_token;
  if (strcmp(s_token, "{")) parser.Error();

  for (;;) {
    parser >> s_token;
    if (parser.Eof()) parser.Error();
    if (!strcmp(s_token, "}")) break;
    source->Parse(parser, s_token);
  }
  return source;
}

Particle * TSource::NewParticle(void)
{
  if (iTotalParticles % 10000) return &ParticleDatabase[iTotalParticles++];

  Particle * temp_database = ParticleDatabase;
  ParticleDatabase = new Particle[iTotalParticles+10000];

  for (int i = 0; i < iTotalParticles; ++i)
    ParticleDatabase[i] = temp_database[i];

  if (temp_database) delete[] temp_database;
  return &ParticleDatabase[iTotalParticles++];
}

// Generic source constructor.
TSource::TSource(void) : TPlane()
{
  ParticleDatabase = NULL;

  lSeed = 0L;
}

void TSource::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Seed")) parser >> lSeed;
  else TPlane::Parse(parser, s_token);
}

void TSource::Initialize(void)
{
  TPlane::Initialize();

  srand48(lSeed);
}

TSourceArray::TSourceArray(void) : TSource()
{
  iNumParticles = 0;
  iGroup = 0;
  rCurrent = 0.0;
  iDim = 1;
  rPitch = 0.0;
  Shape = NULL;
  Illumination = NULL;
  Scattering = NULL;
  MaskIllumination = NULL;
  MaskScattering = NULL;
}

void TSourceArray::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Particles")) parser >> iNumParticles;
  else if (!strcmp(s_token, "Group")) parser >> iGroup;
  else if (!strcmp(s_token, "Current")) parser >> rCurrent;
  else if (!strcmp(s_token, "Array")) parser >> iDim;
  else if (!strcmp(s_token, "Pitch")) parser >> rPitch;
  else if (!strcmp(s_token, "Shape")) {
    if (!Shape) Shape = TShape::Parse(parser);
    else {parser >> s_token; Shape->Parse(parser, s_token);}
  }
  else if (!strcmp(s_token, "Illumination")) {
    if (!Illumination) Illumination = TIllumin::Parse(parser);
    else {parser >> s_token; Illumination->Parse(parser, s_token);}
  }
  else if (!strcmp(s_token, "Scattering")) {
    if (!Scattering) Scattering = TIllumin::Parse(parser);
    else {parser >> s_token; Scattering->Parse(parser, s_token);}
  }
  else if (!strcmp(s_token, "Mask")) {
    int i_num_beams = iDim * iDim;
    parser >> s_token;
    if (!strcmp(s_token, "Illumination")) {
      parser >> s_token;
      if (strcmp(s_token, "{")) parser.Error();
      MaskIllumination = new real[i_num_beams];
      for (int i = 0; i < i_num_beams; ++i) parser >> MaskIllumination[i];
    }
    else if (!strcmp(s_token, "Scattering")) {
      parser >> s_token;
      if (strcmp(s_token, "{")) parser.Error();
      MaskScattering = new real[i_num_beams];
      for (int i = 0; i < i_num_beams; ++i) parser >> MaskScattering[i];
    }
    parser >> s_token;
    if (strcmp(s_token, "}")) parser.Error();
  }

  else TSource::Parse(parser, s_token);
}


void TSourceArray::Initialize(void)
{
  TSource::Initialize();

  if (rCurrent <= 0.0) {
    cerr << "Source Array - Error: invalid Current specification.\n";
    exit(0);
  }

  rEmissionTime = iNumParticles * SimEB::rGetQ() / rCurrent;

  int i_index = 0;

  // Loop through all beams;
  for (int i = 0; i < iNumParticles; ++i) {
    int i_x = (int)(drand48() * iDim);
    int i_y = (int)(drand48() * iDim);
    int i_beam = i_y*iDim + i_x;
    real r_rand = drand48();

    real r_mask = 1.0;
    if (MaskIllumination) r_mask = MaskIllumination[i_beam];
    if (r_rand < r_mask) {
      Particle * particle_p = TSource::NewParticle();
      particle_p->iId = i_index++;
      particle_p->iBeam = i_beam;
      particle_p->iSetGroup(iGroup);

      Shape->Randomize(particle_p);
      particle_p->xyzPosition.rX += i_x * rPitch - (iDim-1)*rPitch/2;
      particle_p->xyzPosition.rY += i_y * rPitch - (iDim-1)*rPitch/2;
      particle_p->xyzPosition.rZ = rGetZ();

      Illumination->Randomize(particle_p, rGetVoltage());

      particle_p->rTf = particle_p->rTi = rEmissionTime * drand48();
      continue;
    }

    if (!MaskScattering) continue;

    r_mask += MaskScattering[i_beam];
    if (r_rand < r_mask) {
      Particle * particle_p = &ParticleDatabase[i_index];
      particle_p->iId = i_index++;
      particle_p->iBeam = i_beam + iDim*iDim;
      particle_p->iSetGroup(iGroup);

      Shape->Randomize(particle_p);
      particle_p->xyzPosition.rX += i_x * rPitch - (iDim-1)*rPitch/2;
      particle_p->xyzPosition.rY += i_y * rPitch - (iDim-1)*rPitch/2;
      particle_p->xyzPosition.rZ = rGetZ();
      Scattering->Randomize(particle_p, rGetVoltage());

      particle_p->rTf = particle_p->rTi = rEmissionTime * drand48();
    }
  }
}

TSourceEELens::TSourceEELens(void)
{
  sFileName[0] = '\0';
  rCurrent = 0.0;
  iDim = 1;
  MaskIllumination = NULL;
  MaskScattering = NULL;
}

void TSourceEELens::Parse(TParser &parser, char * s_token)
{
  if (!strcmp(s_token, "Mode")) {
    parser >> s_token;
    if (!strcmp(s_token, "Lens")) EELens::iMode = EELENS_LENS;
    if (!strcmp(s_token, "Target")) EELens::iMode = EELENS_TARGET;
  }
  else if (!strcmp(s_token, "Group")) parser >> EELens::iGroup;
  else if (!strcmp(s_token, "File")) parser >> sFileName;
  else if (!strcmp(s_token, "Segments")) parser >> EELens::iNumSegments;
  else if (!strcmp(s_token, "Current")) parser >> rCurrent;
  else if (!strcmp(s_token, "Array")) parser >> iDim;
  else if (!strcmp(s_token, "Mask")) {
    int i_num_beams = iDim * iDim;
    parser >> s_token;
    if (!strcmp(s_token, "Illumination")) {
      parser >> s_token;
      if (strcmp(s_token, "{")) parser.Error();
      MaskIllumination = new real[i_num_beams];
      for (int i = 0; i < i_num_beams; ++i) parser >> MaskIllumination[i];
    }
    else if (!strcmp(s_token, "Scattering")) {
      parser >> s_token;
      if (strcmp(s_token, "{")) parser.Error();
      MaskScattering = new real[i_num_beams];
      for (int i = 0; i < i_num_beams; ++i) parser >> MaskScattering[i];
    }
    parser >> s_token;
    if (strcmp(s_token, "}")) parser.Error();
  }
  else TSource::Parse(parser, s_token);
}

void TSourceEELens::Initialize(void)
{
  TSource::Initialize();
  if (EELens::iMode != EELENS_LENS) return;

  char s_filename[MAX_TOKEN];
  strcpy(s_filename, SimEB::sGetRoot());
  strcat(s_filename, ".");
  strcat(s_filename, sFileName);
  EELens::pFile = fopen(s_filename, "r");
  if (!EELens::pFile) {
    cerr << "Source EELens - Error: Could not open file " << s_filename << ".\n";
    exit(0);
  }

  fscanf(EELens::pFile, "%d %d %d", &EELens::iNumLens, &EELens::iNumBeams, &EELens::iNumElements);

  if ((EELens::iNumBeams != iDim*iDim) &&
      (EELens::iNumBeams != 2*iDim*iDim)) {
    cerr << "Lens file does not match Source EELens configuration!\n";
    exit(0);
  }

  EELens::LensMap = new int[EELens::iNumLens];
  EELens::CurrentMap = new real[2*iDim*iDim];

  for (int i = 0; i < 2*iDim*iDim; ++i)
    EELens::CurrentMap[i] = rCurrent / (iDim*iDim);
  if (MaskIllumination) for (int i = 0; i < iDim*iDim; ++i)
    EELens::CurrentMap[i] *= MaskIllumination[i];
  if (!MaskScattering) for (int i = 0; i < iDim*iDim; ++i)
    EELens::CurrentMap[iDim*iDim+i] = 0.0;
  if (MaskScattering) for (int i = 0; i < iDim*iDim; ++i)
    EELens::CurrentMap[iDim*iDim+i] *= MaskScattering[i];
}


TSourceGrid::TSourceGrid(void) : TSource()
{
  iNumParticles = 0;
  iGroup = 0;
  iDim = 1;
  rPitch = 0.0;
  Illumination = NULL;
  xyOffset = xy0;
}

void TSourceGrid::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Particles")) parser >> iNumParticles;
  else if (!strcmp(s_token, "Group")) parser >> iGroup;
  else if (!strcmp(s_token, "Array")) parser >> iDim;
  else if (!strcmp(s_token, "Pitch")) parser >> rPitch;
  else if (!strcmp(s_token, "Illumination")) {
    if (!Illumination) Illumination = TIllumin::Parse(parser);
    else {parser >> s_token; Illumination->Parse(parser, s_token);}
  }
  else if (!strcmp(s_token, "Offset")) parser >> xyOffset;
  else TSource::Parse(parser, s_token);
}

void TSourceGrid::Initialize(void)
{
  TSource::Initialize();

  real r_emission_time = Column::GetSource()->rEmissionTime;

  int i_index = 0;

  // Loop through all beams;
  for (int i = 0; i < iNumParticles; ++i) {
    int i_x = (int)(drand48() * iDim);
    int i_y = (int)(drand48() * iDim);
    int i_beam = i_y*iDim + i_x;

    Particle * particle_p = TSource::NewParticle();
    particle_p->iId = i_index++;
    particle_p->iBeam = i_beam;
    particle_p->iSetGroup(iGroup);

    particle_p->xyzPosition.rX = i_x * rPitch - (iDim-1)*rPitch/2;
    particle_p->xyzPosition.rY = i_y * rPitch - (iDim-1)*rPitch/2;
    particle_p->xyzPosition.rZ = rGetZ();
    particle_p->xyzPosition += xyOffset;

    Illumination->Randomize(particle_p, rGetVoltage());

    particle_p->rTi = particle_p->rTf = r_emission_time * drand48();
  }
}

