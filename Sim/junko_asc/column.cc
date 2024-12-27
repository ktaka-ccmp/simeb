#include "simeb.h"

#define ERROR_NOLEFTBRACE   "simeb: Missing '{'."
#define ERROR_NORGHTBRACE   "simeb: Missing '}'."
#define ERROR_NOARG         "simeb: Error: Missing argument."
#define ERROR_UNKNOWN       "simeb: Error: Unknown token."

real const Column::rPrecision = 1.0e-6;
//real const Column::iMaxSections = 50;
int const Column::iMaxSections = 50; // by ktaka 1999.12.23

int        Column::iNumPlanes = 0;    // Total number of defined planes.
TPlane **  Column::Planes = NULL;     // Array of pointers to planes.
real       Column::rCapture = 0.0;    // Minimum flatness of a plane.

void Column::Setup(void)
{
  Planes = new (TPlane *)[iMaxSections];
}

void Column::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Source"))
    Planes[iNumPlanes++] = TSource::Parse(parser);

  else if (!strcmp(s_token, "Plane"))
    Planes[iNumPlanes++] = TPlane::Parse(parser);

  // 12/29/1999	by ktaka Parsing Trap Feild
  else if (!strcmp(s_token, "Trap"))
    Planes[iNumPlanes++] = TTrap::Parse(parser);
  // 12/29/1999	by ktaka

  ///////////////////////////////////////////////////////////////
  // If none of the known commands have been matched yet, the
  // token should be the name of a defined plane, and the rest
  // of the command should be redirected to the appropriate plane.
  // The following loop searches for the a plane name matching
  // the token and if one is found, redirects the command to
  // that plane.
  else {
    TPlane * plane = NULL;
    for (int i = 0; i < iNumPlanes; ++i)
      if (!strcmp(s_token, Planes[i]->sGetName()))
	  plane = Planes[i];
    if (!plane) parser.Error();

    parser >> s_token;
    if (parser.Eof()) parser.Error();
    plane->Parse(parser, s_token);
  }
}


void Column::Initialize(void)
{
  /////////////////////////////////////////////////////////////
  // 4. Initialize each plane.

  // During the parsing phase, each plane may have processed an
  // "Electrode" statement specifying the voltage at that plane.
  for (int i = 0; i < iNumPlanes-1; ++i)
    Planes[i+1]->rSetVoltage(Planes[i]->rGetVoltage());

  for (int i = 0; i < iNumPlanes-1; ++i) {
    real r_vi = 
      Relativity::rCalcVelocity(Planes[i]->rGetVoltage());
    real r_vf = 
      Relativity::rCalcVelocity(Planes[i+1]->rGetVoltage());

    real r_dz = rGetZ(i+1) - rGetZ(i);
    if (r_dz > 0.0) 
      Planes[i]->rSetFieldA(0.5 * (r_vf*r_vf - r_vi*r_vi) / r_dz);
    cout << r_vi << " , " << r_vf << endl; // 12/19/1999 by ktaka
  }

  // rCapture records how close a particle must be to a plane to cross
  // that plane.
  rCapture = rPrecision * (Planes[iNumPlanes-1]->rGetZ() - Planes[0]->rGetZ());

  // Allow each plane to initialize itself.
  for (int i = 0; i < iNumPlanes; ++i) Planes[i]->Initialize();
}
