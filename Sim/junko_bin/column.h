#ifndef _COLUMN
#define _COLUMN

class Column
{
 private:
  //  static real const iMaxSections; // by ktaka 12/24/99
  static int const iMaxSections;

  static int iNumPlanes;       // Total number of defined planes.
  static TPlane **  Planes;    // Array of pointers to planes.
  static real rCapture;        // Total length of the system.

 public:
  static real const rPrecision;

  static int iGetNumPlanes(void) {return iNumPlanes;};
  static TSource *  GetSource(void) {return (TSource *)Planes[0];};
  static TPlane * GetPlane(int const i) {return Planes[i];};
  static real rGetZ(int const i) {return Planes[i]->rGetZ();};
  static real rGetCapture(void) {return rCapture;};

  static void Setup(void);
  static void Initialize(void);
  static void Parse(TParser &, char *);
  static void Fly(real const r_dt);
};

#endif


