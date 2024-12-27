#ifndef _PARSER
#define _PARSER

class TParser
{
public:
  TParser(void) {bEof = false;};

  virtual ~TParser(void) {};
  virtual bool GetToken(void) = 0;
  virtual void Error(void) = 0;

  bool Eof(void) {return bEof;};
  TParser& operator >>(char *s)
    {if (GetToken()) strcpy(s, sToken); return *this;};
  TParser& operator >>(unsigned &u)
    {if (GetToken()) u = atoi(sToken); return *this;};
  TParser& operator >>(int &i)
    {if (GetToken()) i = atoi(sToken); return *this;};
  TParser& operator >>(long &l)
    {if (GetToken()) l = atol(sToken); return *this;};
  TParser& operator >>(real &r)
    {if (GetToken()) r = atof(sToken); return *this;};

  TParser& operator >>(TXYZ &xyz) {
    if (GetToken()) xyz.rX = atof(sToken);
    if (GetToken()) xyz.rY = atof(sToken);
    if (GetToken()) xyz.rZ = atof(sToken);
    return *this;
  }

  TParser& operator >>(TXY &xy) {
    if (GetToken()) xy.rX = atof(sToken);
    if (GetToken()) xy.rY = atof(sToken);
    return *this;
  }

  TParser& operator >>(bool &b) {
    b = false;
    if (GetToken()) {
      if (!strcmp(sToken, "True")) b = true;
      if (!strcmp(sToken, "Yes")) b = true;
      if (!strcmp(sToken, "On")) b = true;
      if (!strcmp(sToken, "1")) b = true;
    }
    return *this;
  }

protected:
  bool Eof(bool b) {return bEof = b;};

  char sToken[MAX_TOKEN];

private:
  bool bEof;

};


class TFileParser : public virtual TParser
{
public:
  TFileParser(char const *sfilename);
  virtual bool GetToken(void);
  virtual void Error(void);

private:
  FILE *File;
  int  iLine;
};

class TArgParser : public virtual TParser
{
public:
  TArgParser(unsigned uargc, char const * const * argv);
  virtual bool GetToken(void);
  virtual void Error(void);

private:
  unsigned               uNumArgs;
  unsigned               uArg;
  char const * const *   sArgVArray;
};

#endif







