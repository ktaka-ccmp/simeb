#ifndef _TRAP
#define _TRAP

#include "simeb.h"

class TTrap : public TPlane
{
private:
  real test;

public:
  static TTrap *  Parse(TParser &parser);

  TTrap(void);

  virtual void Parse(TParser &, char *);
  virtual void Initialize(void);
};

class TAddFeild : public TTrap
{
 public:
  TAddFeild(void);

  virtual void Parse(TParser &parser, char *);
  virtual void Initialize(void);
  
  virtual real rGetFeildA(void) const {return rFeildTrap;};

 private:
  real	rFeildTrap;
  real	rTrapPotential;
  real	rLengthAddFeild;

};

#endif
