#include <Arduino.h>

#ifndef fretDistances_h
#define fretDistances_h

class FretDistances
{
  public:
    FretDistances();
    // double* nTet(double scaleLenMm, double freqInterval, int nSteps, int nFrets, int preListLength);
    void nTet(double* distances, double scaleLenMm, double freqInterval, int nSteps, int nFrets, int preListLength);
    void meantone(double* distances, double scaleLenMm, double freqInterval, int nFrets, int preListLength);
};

#endif // fretDistances_h