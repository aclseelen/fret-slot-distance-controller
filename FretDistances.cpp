#include <Arduino.h>
#include "FretDistances.h"

// byte puppet[8] = {
//   0b01110,
//   0b10001,
//   0b01110,
//   0b00100,
//   0b11111,
//   0b00100,
//   0b01010,
//   0b10001,
// };

// byte heart[8] = {
// 	0b00000,
// 	0b01010,
// 	0b11111,
// 	0b11111,
// 	0b01110,
// 	0b00100,
// 	0b00000,
// 	0b00000
// };

// lcd.createChar(0, puppet);
// lcd.createChar(1, heart);

typedef struct tsVal {
  int T;
  int s;
} tsVal;

const double semitoneToneRatioMeantone = 0.60628371950;

const tsVal meantone0[] = {
  { 1, -1 },
  { 0, 1 },
  { 1, 0 },
  { 2, -1 },
  { 1, 1 },
  { 2, 0 },
  { 1, 2 },
  { 2, 1 },
  { 3, 0 },
  { 2, 2 },
  { 3, 1 },
  { 4, 0 },
  { 3, 2 },
  { 4, 1 },
  { 3, 3 },
  { 4, 2 },
  { 5, 1 },
  { 4, 3 },
  { 5, 2 },
  { 5, 3 },  // 13
  { 6, 2 },  // 14
  { 6, 3 },  // 15
  { 7, 2 },  // 16
  { 7, 3 },  // 17
  { 8, 2 },  // 18
  { 8, 3 },  // 19
  { 8, 4 },  // 20
  { 9, 3 },  // 21
  { 9, 4 },  // 22
};

FretDistances::FretDistances() {}

double intervalToDistance(double scaleLenMm, double fretFreqInterval) {
  return scaleLenMm - (scaleLenMm / fretFreqInterval);
}

void FretDistances::nTet(double* distances, double scaleLenMm, double refFreqInterval, int nSteps, int nFrets, int preListLength) {

  for (int i = 0; i < nFrets; i++) {
    double fretFreqInterval = pow(refFreqInterval, (i + 1.0) / nSteps);
    distances[i + preListLength] = intervalToDistance(scaleLenMm, fretFreqInterval);
  }
}

void FretDistances::meantone(double* distances, double scaleLenMm, double refFreqInterval, int nFrets, int preListLength) {

  for (int i = 0; i < nFrets; i++) {

    tsVal currentTs = meantone0[i];

    double fretFreqInterval = pow(refFreqInterval, ((currentTs.T + currentTs.s * semitoneToneRatioMeantone ) / (5 + 2 * semitoneToneRatioMeantone)));
    distances[i + preListLength] = intervalToDistance(scaleLenMm, fretFreqInterval);
  }
}