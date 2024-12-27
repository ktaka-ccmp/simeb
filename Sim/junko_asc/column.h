#ifndef _COLUMN
#define _COLUMN

class Column
{
 private:
  //  static real const iMaxSections;
  static int const iMaxSections; //by ktaka 1999.12.23

  static int iNumPlanes;       // Total number of defined planes.
  static TPlane **  Planes;    // Array of pointers to planes.
  static real rCapture;        // Total length of the system.

 public:
  static real const rPrecision;

  static int iGetNumPlanes(void) {return iNumPlanes;};
  static TSource *  GetSource(void) {return (TSource *)Planes[0];};
  static TPlane * GetPlane(int const i) {return Planes[i];};

  // 12/29/1999 by ktaka
  //  static TTrap * GetTrap(int const i) {return (TTrap *)Planes[i];}; 
  // 12/29/1999 by ktaka

  static real rGetZ(int const i) {return Planes[i]->rGetZ();};
  static real rGetCapture(void) {return rCapture;};

  static void Setup(void);
  static void Initialize(void);
  static void Parse(TParser &, char *);
  static void Fly(real const r_dt);
};

#endif


