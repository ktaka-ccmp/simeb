#ifndef _VECTOR
#define _VECTOR

#include <iostream.h>
#include "include.h"

class TXY;
class TXYZ;

class TXY
{
public:
  TXY(real rix = 0.0, real riy = 0.0) {rX = rix; rY = riy;};
  TXY(const TXYZ &xyz);

  TXY operator =(const TXYZ &xyz);

  real rX;
  real rY;
};

class TXYZ
{
public:
  TXYZ(real rix = 0.0, real riy = 0.0, real riz = 0.0) {rX = rix; rY = riy; rZ = riz;};
  TXYZ(const TXY &xy, real riz = 0.0);

  TXYZ operator =(const TXY &xy);

  real rX;
  real rY;
  real rZ;
};

extern TXY  xy0;
extern TXYZ xyz0;

inline TXY::TXY(const TXYZ &xyz) {rX = xyz.rX; rY = xyz.rY;}
inline TXY TXY::operator=(const TXYZ &xyz) {rX = xyz.rX; rY = xyz.rY; return *this;}
inline TXYZ::TXYZ(const TXY &xy, real riz) {rX = xy.rX; rY = xy.rY; rZ = riz;}
inline TXYZ TXYZ::operator=(const TXY &xy) {rX = xy.rX; rY = xy.rY; return *this;}

inline ostream& operator<<(ostream &stream_out, TXY xy)
{return stream_out << xy.rX << " " << xy.rY << " ";}

inline ostream& operator<<(ostream &stream_out,TXYZ xyz)
{return stream_out << xyz.rX << " " << xyz.rY << " " << xyz.rZ << " ";}



inline TXYZ operator +(TXYZ const xyz1,  TXYZ const xyz2)
{
  TXYZ xyzresult;

  xyzresult.rX = xyz1.rX + xyz2.rX;
  xyzresult.rY = xyz1.rY + xyz2.rY;
  xyzresult.rZ = xyz1.rZ + xyz2.rZ;
  return xyzresult;
}

inline TXY operator +(TXY const xy1, TXY const xy2)
{
  TXY xyresult;

  xyresult.rX = xy1.rX + xy2.rX;
  xyresult.rY = xy1.rY + xy2.rY;
  return xyresult;
}

inline TXYZ operator +(TXYZ const xyz, TXY const xy)
{
  TXYZ xyzresult;

  xyzresult.rX = xyz.rX + xy.rX;
  xyzresult.rY = xyz.rY + xy.rY;
  xyzresult.rZ = xyz.rZ;
  return xyzresult;
}



inline TXYZ& operator +=(TXYZ &xyz1, TXYZ xyz2)
{
  xyz1.rX += xyz2.rX;
  xyz1.rY += xyz2.rY;
  xyz1.rZ += xyz2.rZ;
  return xyz1;
}

inline TXY& operator +=(TXY &xy1, TXY xy2)
{
  xy1.rX += xy2.rX;
  xy1.rY += xy2.rY;
  return xy1;
}

inline TXYZ& operator +=(TXYZ &xyz, TXY xy)
{
  xyz.rX += xy.rX;
  xyz.rY += xy.rY;
  return xyz;
}



inline TXYZ operator -(TXYZ xyz1, TXYZ xyz2)
{
  TXYZ xyz_result;

  xyz_result.rX = xyz1.rX - xyz2.rX;
  xyz_result.rY = xyz1.rY - xyz2.rY;
  xyz_result.rZ = xyz1.rZ - xyz2.rZ;
  return xyz_result;
}

inline TXY operator -(TXY xy1, TXY xy2)
{
  TXY xyresult;

  xyresult.rX = xy1.rX - xy2.rX;
  xyresult.rY = xy1.rY - xy2.rY;
  return xyresult;
}


inline TXYZ operator -(TXY xy, TXYZ xyz)
{
  TXYZ xyz_result;

  xyz_result.rX = xy.rX - xyz.rX;
  xyz_result.rY = xy.rY - xyz.rY;
  xyz_result.rZ = -xyz.rZ;
  return xyz_result;
}

inline TXYZ& operator -=(TXYZ &xyz1, TXYZ xyz2)
{
  xyz1.rX -= xyz2.rX;
  xyz1.rY -= xyz2.rY;
  xyz1.rZ -= xyz2.rZ;
  return xyz1;
}

inline TXY& operator -=(TXY &xy1, TXY &xy2)
{
  xy1.rX -= xy2.rX;
  xy1.rY -= xy2.rY;
  return xy1;
}

inline TXYZ& operator -(TXYZ &xyz)
{
  xyz.rX = -xyz.rX;
  xyz.rY = -xyz.rY;
  xyz.rZ = -xyz.rZ;
  return xyz;
}

inline real operator *(TXYZ xyz1, TXYZ xyz2)
{
  return xyz1.rX*xyz2.rX + xyz1.rY*xyz2.rY + xyz1.rZ*xyz2.rZ;
}

inline TXYZ operator *(TXYZ xyz, real r)
{
  TXYZ xyzresult;

  xyzresult.rX = xyz.rX * r;
  xyzresult.rY = xyz.rY * r;
  xyzresult.rZ = xyz.rZ * r;
  return xyzresult;
}

inline TXYZ& operator *=(TXYZ &xyz, real r)
{
  xyz.rX *= r;
  xyz.rY *= r;
  xyz.rZ *= r;
  return xyz;
}


inline TXYZ operator *(real r, TXYZ xyz)
{
  TXYZ xyzresult;

  xyzresult.rX = r * xyz.rX;
  xyzresult.rY = r * xyz.rY;
  xyzresult.rZ = r * xyz.rZ;
  return xyzresult;
}

inline TXYZ operator /(TXYZ xyz, real r)
{
  TXYZ xyzresult;

  real r_inv = 1/r;
  xyzresult.rX = xyz.rX * r_inv;
  xyzresult.rY = xyz.rY * r_inv;
  xyzresult.rZ = xyz.rZ * r_inv;
  return xyzresult;
}

inline TXYZ& operator /=(TXYZ &xyz, real r)
{
  real r_inv = 1/r;

  xyz.rX *= r_inv;
  xyz.rY *= r_inv;
  xyz.rZ *= r_inv;
  return xyz;
}

inline real operator *(TXY xy1, TXY xy2)
{
  return xy1.rX*xy2.rX + xy1.rY*xy2.rY;
}

inline TXY operator *(TXY xy, real r)
{
  TXY xyresult;

  xyresult.rX = xy.rX * r;
  xyresult.rY = xy.rY * r;
  return xyresult;
}

inline TXY operator *(real r, TXY xy)
{
  TXY xyresult;

  xyresult.rX = r * xy.rX;
  xyresult.rY = r * xy.rY;
  return xyresult;
}

inline TXY operator /(TXY xy, real r)
{
  TXY xyresult;

  real r_inv = 1/r;
  xyresult.rX = xy.rX * r_inv;
  xyresult.rY = xy.rY * r_inv;
  return xyresult;
}

inline TXYZ& operator -=(TXYZ &xyz, TXY xy)
{
  xyz.rX -= xy.rX;
  xyz.rY -= xy.rY;
  return xyz;
}

#endif





