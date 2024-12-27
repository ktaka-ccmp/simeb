#ifndef _CAP
#define _CAP

#ifdef _SIMEB_CAP_GROUP
  #define CAP_bGroup true
#else
  #define CAP_bGroup false
#endif

#ifdef _SIMEB_CAP_ACCEL
  #define CAP_bAcceleration true
#else
  #define CAP_bAcceleration false
#endif

#ifdef _SIMEB_CAP_RELATIVITY
  #define CAP_bRelativity true
#else
  #define CAP_bRelativity false
#endif

#endif

