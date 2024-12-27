#include "simeb.h"

////////////////////////////////////////////////////////////////////
// Error messages.
#define ERROR_USAGE         "simeb (v4.0)\nUsage: simeb system args..."
#define ERROR_USAGE_ACCEL   "Compiled w/ Accelerating Field Capability."
#define ERROR_USAGE_RELATIVITY "Compiled w/ Special Relativity Capability."
#define ERROR_BADFILE       "simeb: Could not open file: "
#define ERROR_BADARG        "simeb: Error: Bad argument."
#define ERROR_MULTSOURCEDEF "simeb: Multiple source definition."
#define ERROR_NOSOURCEDEF   "simeb: No source definition."
#define ERROR_MAXSECTIONS   "simeb: Too many section definitions."
#define WARNING_NOCAP_TRACK "simeb: -Track directive ignored. Recompile \
with -D_SIMEB_CAP_TRACK."
#define WARNING_NOCAP_PROXIMITY "simeb: -Proximity directive ignored. \
Recompile with -D_SIMEB_CAP_PROXIMITY."

void SimEB::Setup(int i_argc, char const * const s_argv[])
{
  /////////////////////////////////////////////////////////////
  // 1. Check command line arguments.

  if (i_argc < 2) {
    // If no command line arguments are given, print the
    // usage information.
    cerr << ERROR_USAGE << newline;

    #ifdef _SIMEB_CAP_ACCEL
      cerr << ERROR_USAGE_ACCEL << newline;
    #endif // ACCEL

    #ifdef _SIMEB_CAP_RELATIVITY
      cerr << ERROR_USAGE_RELATIVITY << newline;
    #endif // RELATIVITY

    exit(0);
  }

  SimEB::sSetName(s_argv[1]);
    // The first command line argument is always the default
    // name of the system.

  Column::Setup();

  ////////////////////////////////////////////////////////////
  // 2. Parse the input file and command line arguments.

  // Open the input file. The first command line argument is also
  // the name of input file describing the system.
  TFileParser * file_parser = new TFileParser(s_argv[1]);
  if (file_parser->Eof()) {
    cerr << "simeb: Could not open " << s_argv[1] << newline;
    exit(0);
  }

  for (;;) { // Parse tokens in the input file.
    char s_token[MAX_TOKEN];
    *file_parser >> s_token;
    if (file_parser->Eof()) break;
    Parse(*file_parser, s_token);
  }

  // Close the input file.
  delete file_parser; 

  TArgParser * arg_parser = new TArgParser(i_argc, s_argv);
    // All arguments following the first argument are parsed as
    // if they were included in the input file.

  for (;;) { // Parse tokens in the command line.
    char s_token[MAX_TOKEN];
    *arg_parser >> s_token;
    if (arg_parser->Eof()) break;
    Parse(*arg_parser, s_token);
  }
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// Parse()
//
// Parse system level commands. The current command to
// parse is specified in <s_token>. Subsequent arguments
// may be obtained from <parser>. Return value is TRUE
// for successfully parsing the command and FALSE for
// any error encountered. See the simeb user documentation
// for further information on the command options and
// formats.
void SimEB::Parse(TParser &parser, char *s_token)
{
  if (s_token[0] == '-') ++s_token;
  /////////////////////////////////////////////////////////
  // The default value for the system name is the name
  // specified in the first argument on the command line.
  // The following two commands may be used to change the
  // name to something else. They are useful because various
  // output files names are derived from the system name, so
  // these options may be used to generate unique output
  // files for several simulation trials on the same input
  // system file.

  // Redefine the system name.
  if (!strcmp(s_token, "Name")) {parser >> s_token;  SimEB::sSetName(s_token);}

  // Redefine the system name.
  else if (s_token[0] == '.') SimEB::sExtName(s_token);

  else if (!strcmp(s_token, "Sphere")) parser >> iSphere;
  else if (!strcmp(s_token, "Epsilon")) parser >> rEpsilon;
  else if (!strcmp(s_token, "Ion")) {
    parser >> SimEB::rM; SimEB::rM *= SimEB::rM1;
    parser >> SimEB::rQ; SimEB::rQ *= SimEB::rQ0;
  }
  else if (!strcmp(s_token, "Halt")) parser >> iHalt;

  else Column::Parse(parser, s_token);
}

void SimEB::Initialize(void)
{
  SimEB::rQM = SimEB::rQ / SimEB::rM;
  Relativity::rSetQM(SimEB::rQM);
  SimEB::rScaleFactor = SimEB::rCoulomb * SimEB::rQ * SimEB::rQM;

  Column::Initialize();

  iNumGenerated = TSource::iGetTotalParticles();

  // Allocate space for and initialize <ParticleArray> and <ParticleHeap>.
  ParticleArray = new Particle *[iNumGenerated];
  ParticleHeap = new Particle *[iNumGenerated];

  for (int i = 0; i < iNumGenerated; ++i) 
    ParticleArray[i] = TSource::GetParticle(i);
  for (int i = 0; i < iNumGenerated; ++i)
    ParticleHeap[i] = TSource::GetParticle(i);

  qsort(ParticleArray, iNumGenerated, sizeof(Particle *),
	(int(*)(void const *, void const *))Particle::EmissionOrder);
  qsort(ParticleHeap, iNumGenerated, sizeof(Particle *),
	(int(*)(void const *, void const *))Particle::EmissionOrder);

  for (int i = 0; i < iNumGenerated; ++i) ParticleArray[i]->iZOrder = i;
  for (int i = 0; i < iNumGenerated; ++i) ParticleHeap[i]->iTOrder = i;
}


void SimEB::Finalize(void)
{
  cerr << "simeb: Simulation complete for " << SimEB::sGetName() << ".\n";
  cerr << "simeb: " << iIteration << " iterations.\n";
}



void SimEB::InitProgress(void)
{
  char s_filename[MAX_TOKEN];

  sprintf(s_filename, "%s.progress", sRoot);
  fpProgress = fopen(s_filename, "w");
  if  (!fpProgress)  {
    cerr << "Could not open " << s_filename << ".\n";
    exit(0);
  }

  FILE * fp_batch;

  sprintf(s_filename, "%s.poll", sRoot);
  fp_batch = fopen(s_filename, "w"); 
  if (!fp_batch) {
    cerr << "Could not open " << s_filename << ".\n";
    exit(0);
  }
  fprintf(fp_batch, "kill -USR1 %d\n", getpid());
  fprintf(fp_batch, "sleep 1\n");
  fprintf(fp_batch, "cat %s.progress\n", sRoot);
  fclose(fp_batch);
  chmod(s_filename, S_IRUSR | S_IWUSR | S_IXUSR);

  sprintf(s_filename, "%s.kill", sRoot);  
  fp_batch = fopen(s_filename, "w"); 
  if (!fp_batch) {
    cerr << "Could not open " << s_filename << ".\n";
    exit(0);
  }
  fprintf(fp_batch, "kill %d\n", getpid());
  fprintf(fp_batch, "rm %s.progress\n", sRoot);
  fprintf(fp_batch, "rm %s.poll\n", sRoot);
  fprintf(fp_batch, "rm %s.kill\n", sRoot);
  fclose(fp_batch);
  chmod(s_filename, S_IRUSR | S_IWUSR | S_IXUSR);

  signal(SIGUSR1, PollProgress);
}

void SimEB::KillProgress(void)
{
  char s_filename[MAX_TOKEN];

  sprintf(s_filename, "%s.poll", sRoot);
  unlink(s_filename);

  sprintf(s_filename, "%s.kill", sRoot);
  unlink(s_filename);

  sprintf(s_filename, "%s.progress", sRoot);
  unlink(s_filename);
}

void SimEB::PollProgress(int)
{
  if (!iNumActivated) return;
  if (iNumKilled == iNumGenerated) return;

  //ftruncate(fpProgress->_file, 0);
  ftruncate(fpProgress->_fileno, 0); // by ktaka 12/24/99

  Particle * particle;
  
  particle = ParticleArray[iNumKilled];
  real r_lead = particle->xyzP.rZ - Column::rGetZ(0);

  particle = ParticleArray[iNumActivated-1];
  real r_tail = particle->xyzP.rZ - Column::rGetZ(0);

  fprintf(fpProgress, "I=%d A=%d K=%d L=%f T=%f\n",
    iIteration, iNumActivated-iNumKilled, iNumKilled, (float)r_lead, (float)r_tail);

  fflush(fpProgress);
}



TFileParser::TFileParser(char const *s_filename) : TParser()
{
  File = fopen(s_filename, "r");
  if (!File) Eof(true);

  iLine = 1;
}

bool TFileParser::GetToken()
{
  unsigned  u = 0;
  int       i = ' ';

  *sToken = '\0';
  if (Eof()) return !Eof();

  while (isspace(i)) {
    if (i == '\n') ++iLine;

    i = getc(File);
    if (i == EOF) return !Eof(true);
    if (i == '/') while ((i = getc(File)) != '\n') if (i == EOF) return !Eof(true);
  }

  while (!isspace(i)) {
    sToken[u++] = i;

    i = getc(File);
    if (i == EOF) return !Eof(true);
  }
  if (i == '\n') ++iLine;

  sToken[u++] = '\0';
  return !Eof();
}

void TFileParser::Error()
{
  cerr << "Error parsing input file!\n";
  if (Eof()) cerr << "Unexpected EOF.\n";
  else cerr << "Line " << iLine << space << ": " << sToken << newline;
  exit(0);
}


TArgParser::TArgParser(unsigned u_argc, char const * const * s_argv) : TParser()
{
  uNumArgs = u_argc;
  uArg = 2;

  sArgVArray = s_argv;
}

bool TArgParser::GetToken()
{
  if (uArg == uNumArgs) return Eof(true);
  strcpy(sToken, sArgVArray[uArg++]);
  return true;
}

void TArgParser::Error()
{
  cerr << "Error parsing command line!\n";
  if (Eof()) cerr << "Unterminated command.\n";
  else cerr << "Token " << uArg << space << ": " << sToken << newline;
  exit(0);  
}


