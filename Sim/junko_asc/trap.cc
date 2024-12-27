#include "simeb.h"

TTrap::TTrap(void) : TPlane()
{
}

TTrap * TTrap::Parse(TParser &parser)
{
  TTrap * trap = NULL;
  char s_token[MAX_TOKEN];

  parser >> s_token;
  if (!strcmp(s_token, "AddFeild")) trap = new TAddFeild();
  if (!trap) parser.Error();

  parser >> s_token;
  if (strcmp(s_token, "{")) parser.Error();

  for (;;) {
    parser >> s_token;
    if (parser.Eof()) parser.Error();
    if (!strcmp(s_token, "}")) break;
    trap->Parse(parser, s_token);
  }
  return trap;
}

void TTrap::Parse(TParser &parser, char *s_token)
{
  //  real lSeed;
  //  if (!strcmp(s_token, "Seed")) parser >> lSeed;
  //  else TPlane::Parse(parser, s_token);
  TPlane::Parse(parser, s_token);
}

void TTrap::Initialize(void)
{
  cout << "test" << endl;
  TPlane:Initialize();
}
 
TAddFeild::TAddFeild(void) : TTrap()
{
  rTrapPotential = 0;
  rLengthAddFeild = 0;
  rFeildTrap	= 0;
}

void TAddFeild::Parse(TParser &parser, char *s_token)
{
  if (!strcmp(s_token, "Potential")) parser >> rTrapPotential;
  else if (!strcmp(s_token, "Length")) parser >> rLengthAddFeild;
  //  else if (!strcmp(s_token, "Feild")) parser >> rFeildTrap;
  else TPlane::Parse(parser, s_token);
  //  cout << sName << " , " << rTrapPotential << " , " << rLengthAddFeild << endl;
}
  
void TAddFeild::Initialize(void)
{
  TTrap:Initialize();
  rFeildTrap = rTrapPotential/rLengthAddFeild;
}



