#include <Stepper.h>
#include <LiquidCrystal.h>
#include "FretDistances.h"

const int STEPS_PER_REVOLUTION = 200;
const double MM_INCREASE_PER_REVOLUTION = 1.25;
const int BTN_SHORT_TRESHOLD = 40;      // ignore electrical contacts less than 20 ms
const int BTN_MIDLONG_TRESHOLD = 1500;  // a button pressed more than 1.5 seconds can be assigned a different function
const int BTN_LONG_TRESHOLD = 4000;     // a button pressed more than 4 seconds can be assigned a different function
const float FRET_SAW_THICKNESS = 0.5;   // millimeters
const float NUT_THICKNESS = 3.175;      // millimeters
const int PRE_LIST_LENGTH = 3;          // Reserved for values before the first fret: [0] zero fret/position, [1] nut headstock side, [2] nut fingerboard side
const int MAX_DISTANCES = 60;           // Max. length of distance list: number of frets plus length of prelist
const int HIGH_SPEED = 180;
const int MEDIUM_SPEED = 120;
const int LOW_SPEED = 30;


/** Pin assignment **/

const int leftButtonPin = 2;
const int rightButtonPin = 3;

const int leftButtonLedPin = 9;
const int rightButtonLedPin = 10;

const int motorPin1 = 8;
const int motorPin2 = 11;
const int motorPin3 = 12;
const int motorPin4 = 13;

FretDistances fd;
Stepper stepper(STEPS_PER_REVOLUTION, motorPin1, motorPin2, motorPin3, motorPin4);
LiquidCrystal lcd(A0, A1, 4, 5, 6, 7);

/** Scale length **/

const int N_SCALE_LENGTH_OPTIONS = 5;
int currentScaleLengthOption = 0;
const char* scaleLengthStrings[N_SCALE_LENGTH_OPTIONS] = {
  "25\"",
  "25.5\"",
  "650 mm",
  "33.25\"",
  "34\"",
};

const float scaleLengthOptionsMm[N_SCALE_LENGTH_OPTIONS] = {
  635.0,   // 25"
  647.7,   // 25.5"
  650.0,   // 650 mm
  844.55,  // 33.25"
  863.6,   // 34"
};

/** Fret division **/

const int N_FRET_DIVISION_OPTIONS = 8;
int currentFretDivisionOption = 0;
const char* fretDivisionStrings[N_FRET_DIVISION_OPTIONS] = {
  "12-TET (21)     ",
  "12-TET (22)     ",
  "Meantone-0 (28) ",
  "Meantone-0 (29) ",
  // "JI makam (42)   ",
  "19-TET (33)     ",
  "19-TET (35)     ",
  "7-TET (12)      ",
  "7-TET (13)      ",
};

const int fretDivisionNFrets[N_FRET_DIVISION_OPTIONS] = {
  21,
  22,
  28,
  29,
  // 42,
  33,
  35,
  12,
  13,
};

/** Variable initialization **/

double distances[MAX_DISTANCES];
int distanceListLength;

int leftBtnStatus = HIGH;
int rightBtnStatus = HIGH;
long lastChangeLeftBtnStatus = 0;   // millis
long lastChangeRightBtnStatus = 0;  // millis

bool menuMode = true;  // if false, active saw mode is enabled
int menuItem = 0;
bool printToLCD = true;
bool moving = false;


int currentPosList = 0;
/** positions in steps **/
long currentPosSteps = 0;
long nextPosSteps = 0;
long prevPosSteps = 0;

long mmToSteps(double distanceInMm) {
  return ((distanceInMm * STEPS_PER_REVOLUTION) / MM_INCREASE_PER_REVOLUTION);
}

double stepsToMm(long steps) {
  return (steps / STEPS_PER_REVOLUTION) * MM_INCREASE_PER_REVOLUTION;
}

int posInListToRange(int pos, int length) {  // handle negative number and modulo
  while (pos < 0) {
    pos += length;
  }
  return pos % length;
}

void setup() {

  Serial.begin(9600);

  pinMode(9, OUTPUT);
  stepper.setSpeed(120);

  // nFrets = 21;
  // double scaleLenMm = 635.0;
  // double freqInterval = 2.0;
  // double nSteps = 12;

  lcd.begin(16, 2);

  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
}

void loadDistances() {

  distanceListLength = fretDivisionNFrets[currentFretDivisionOption] + PRE_LIST_LENGTH;

  Serial.println("line 120");
  Serial.println(distanceListLength);
  Serial.println(sizeof(distances) / sizeof(double));

  distances[0] = 0.0;                                       // Zero fret slot / position zero
  distances[1] = 0.5 * FRET_SAW_THICKNESS - NUT_THICKNESS;  // nut edge slot headstock side
  distances[2] = -0.5 * FRET_SAW_THICKNESS;                 // nut edge slot fingerboard side

  FretDistances fd;
  switch (currentFretDivisionOption) {
    case 0:
    case 1:
      fd.nTet(distances, scaleLengthOptionsMm[currentScaleLengthOption], 2.0, 12, fretDivisionNFrets[currentFretDivisionOption], PRE_LIST_LENGTH);
      break;
    case 2:
    case 3:
      fd.meantone(distances, scaleLengthOptionsMm[currentScaleLengthOption], 2.0, fretDivisionNFrets[currentFretDivisionOption], PRE_LIST_LENGTH);
      break;
    case 4:
    case 5:
      fd.nTet(distances, scaleLengthOptionsMm[currentScaleLengthOption], 2.0, 19, fretDivisionNFrets[currentFretDivisionOption], PRE_LIST_LENGTH);
      break;
    case 6:
    case 7:
      fd.nTet(distances, scaleLengthOptionsMm[currentScaleLengthOption], 2.0, 7, fretDivisionNFrets[currentFretDivisionOption], PRE_LIST_LENGTH);
      break;
    default:
      break;
  }
  // for (int i = 0; i < distanceListLength; i++) {
  //   Serial.println(distances[i]);
  // }
}

void saveMotorStateBeforeIdle() {
}

void motorOnIdle() {

  saveMotorStateBeforeIdle();

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

long checkBtn(int btnPin, int btnStatus, long lastChangeBtnStatus) {

  int btnVal = digitalRead(btnPin);
  long btnPressedMillis = 0;

  if (btnVal != btnStatus) {

    long lastStatusDuration = millis() - lastChangeBtnStatus;

    if (btnVal == HIGH) {
      // button just got into "released" state, thus HIGH
      btnPressedMillis = lastStatusDuration;
    }

    if (btnPin == leftButtonPin) {
      leftBtnStatus = btnVal;
      lastChangeLeftBtnStatus = millis();
    } else if (btnPin == rightButtonPin) {
      rightBtnStatus = btnVal;
      lastChangeRightBtnStatus = millis();
    }
  }

  return btnPressedMillis;
}

void menuScaleLength() {

  if (printToLCD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(scaleLengthStrings[currentScaleLengthOption]);
    lcd.setCursor(0, 1);
    lcd.print("select <--> next");
  }
  printToLCD = false;  // prevent printing in loop without changes

  long leftBtnPressedMillis = checkBtn(leftButtonPin, leftBtnStatus, lastChangeLeftBtnStatus);
  if (leftBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    // CONFIRM AND MOVE TO NEXT MENU ITEM
    menuItem++;
    printToLCD = true;
    // possible improvement: see what to do when both buttons pressed, e.g check other btnPin state
  }
  long rightBtnPressedMillis = checkBtn(rightButtonPin, rightBtnStatus, lastChangeRightBtnStatus);
  if (rightBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    // SKIP TO NEXT OPTION
    currentScaleLengthOption = (currentScaleLengthOption + 1) % N_SCALE_LENGTH_OPTIONS;
    printToLCD = true;
  }
}

void menuFretDivision() {

  if (printToLCD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(fretDivisionStrings[currentFretDivisionOption]);
    lcd.setCursor(0, 1);
    lcd.print("select <--> next");
  }
  printToLCD = false;  // prevent printing in loop without changes

  long leftBtnPressedMillis = checkBtn(leftButtonPin, leftBtnStatus, lastChangeLeftBtnStatus);
  if (leftBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    // CONFIRM AND MOVE TO NEXT MENU ITEM
    menuItem++;
    printToLCD = true;
    // possible improvement: see what to do when both buttons pressed, e.g check other btnPin state
  }
  long rightBtnPressedMillis = checkBtn(rightButtonPin, rightBtnStatus, lastChangeRightBtnStatus);

  if (rightBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    // SKIP TO NEXT OPTION
    currentFretDivisionOption = (currentFretDivisionOption + 1) % N_FRET_DIVISION_OPTIONS;
    printToLCD = true;
  }
}

void menuConfirmSelection() {
  if (printToLCD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(scaleLengthStrings[currentScaleLengthOption]);
    lcd.setCursor(0, 1);
    lcd.print(fretDivisionStrings[currentFretDivisionOption]);
    printToLCD = false;  // prevent printing in loop without changes
  }

  long leftBtnPressedMillis = checkBtn(leftButtonPin, leftBtnStatus, lastChangeLeftBtnStatus);
  if (leftBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    menuItem++;
    printToLCD = true;
    // possible improvement: see what to do when both buttons pressed, e.g check other btnPin state

    loadDistances();
  }

  long rightBtnPressedMillis = checkBtn(rightButtonPin, rightBtnStatus, lastChangeRightBtnStatus);
  if (rightBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    menuItem = 0;                                    // reject -> back to start
    printToLCD = true;
  }
}

bool anyButtonReleased() {

  long millisPressed = 0;
  millisPressed = checkBtn(leftButtonPin, leftBtnStatus, lastChangeLeftBtnStatus);
  if (millisPressed > 0) {
    return true;
  }
  millisPressed = checkBtn(rightButtonPin, rightBtnStatus, lastChangeRightBtnStatus);
  if (millisPressed > 0) {
    return true;
  }
  return false;
}

void freeMove(int stepsPerTurn, int speed) {

  stepper.setSpeed(speed);
  bool moving = true;
  while (moving) {
    stepper.step(stepsPerTurn);
    if (anyButtonReleased()) {
      moving = false;
    }
  }
  motorOnIdle();
}

void menuSetZeroPosition() {

  if (printToLCD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move left-right ");
    lcd.setCursor(0, 1);
    lcd.print("hold lft confirm");
    printToLCD = false;  // prevent printing in loop without
  }

  long leftBtnPressedMillis = checkBtn(leftButtonPin, leftBtnStatus, lastChangeLeftBtnStatus);
  if (leftBtnPressedMillis > BTN_MIDLONG_TRESHOLD) {  // button pressed long
    menuMode = false;
    printToLCD = true;
    // possible improvement: see what to do when both buttons pressed, e.g check other btnPin state
  } else if (leftBtnPressedMillis > BTN_SHORT_TRESHOLD) {
    // move backward
    freeMove(-30, HIGH_SPEED);
  }

  long rightBtnPressedMillis = checkBtn(rightButtonPin, rightBtnStatus, lastChangeRightBtnStatus);
  if (rightBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    // move forward
    freeMove(30, HIGH_SPEED);
  }
}

void selectionMenu() {

  switch (menuItem) {
    case 0:
      menuScaleLength();
      break;
    case 1:
      menuFretDivision();
      break;
    case 2:
      menuConfirmSelection();
      break;
    case 3:
      menuSetZeroPosition();
      break;
    default:
      break;
  }
}

void printCurrentAndNextPosition() {

  char line0[17];
  snprintf(line0, sizeof(line0), "Pos[%d]: %ld", currentPosList, currentPosSteps);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line0);

  snprintf(line0, sizeof(line0), "Pos[%d]: %ld", currentPosList + 1, nextPosSteps);
  lcd.setCursor(0, 1);
  lcd.print(line0);
}

void printCurrentPos(bool printAll, bool isPrev) {

  int plusOrMinusOne = 1;
  if (isPrev) {
    plusOrMinusOne = -1;
  }

  char line0[17];
  snprintf(line0, sizeof(line0), "Pos[%d-%d] %ld", currentPosList, posInListToRange(currentPosList + plusOrMinusOne, distanceListLength), currentPosSteps);
  lcd.setCursor(0, 0);
  lcd.print(line0);

  if (printAll) {

    long prevOrNextPosSteps = nextPosSteps;
    if (isPrev) {  // instead of next not next, i.e. pressed button to go to previous position
      prevOrNextPosSteps = prevPosSteps;
      // plusOrMinusOne = -1;
    }
    // int multiply = 1;

    int posInList = posInListToRange(currentPosList + plusOrMinusOne, distanceListLength);

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(distances[posInList]);
    lcd.setCursor(7, 1);
    lcd.print("-> ");
    lcd.setCursor(10, 1);
    lcd.print(prevOrNextPosSteps);
  }
}

void moveToNextPos() {

  bool printAll = true;
  stepper.setSpeed(HIGH_SPEED);
  int stepsPerTurn = 30;
  digitalWrite(leftButtonLedPin, LOW);
  digitalWrite(rightButtonLedPin, LOW);

  while (currentPosSteps < nextPosSteps) {

    if ((nextPosSteps - currentPosSteps) > 50) {

    } else if ((nextPosSteps - currentPosSteps) < 12) {
      stepper.setSpeed(LOW_SPEED);
      stepsPerTurn = 1;
    } else {  // if ((nextPosSteps - currentPosSteps) < 40) {
      stepper.setSpeed(MEDIUM_SPEED);
      stepsPerTurn = 10;
    }

    stepper.step(stepsPerTurn);
    currentPosSteps += stepsPerTurn;

    printCurrentPos(printAll, false);
    printAll = false;
  }

  stepsPerTurn *= -1;
  while (currentPosSteps > nextPosSteps) {

    if (abs(nextPosSteps - currentPosSteps) > 50) {

    } else if (abs(currentPosSteps - nextPosSteps) < 12) {
      stepper.setSpeed(LOW_SPEED);
      stepsPerTurn = -1;
    } else {  //if (abs(currentPosSteps - nextPosSteps) < 40) {
      stepper.setSpeed(MEDIUM_SPEED);
      stepsPerTurn = -10;
    }

    stepper.step(stepsPerTurn);
    currentPosSteps += stepsPerTurn;

    printCurrentPos(printAll, false);
    printAll = false;
  }

  motorOnIdle();
  digitalWrite(leftButtonLedPin, HIGH);
  digitalWrite(rightButtonLedPin, HIGH);
}

void moveToPrevPos() {

  bool printAll = true;
  stepper.setSpeed(HIGH_SPEED);
  int stepsPerTurn = 30;
  digitalWrite(leftButtonLedPin, LOW);
  digitalWrite(rightButtonLedPin, LOW);

  while (currentPosSteps < prevPosSteps) {

    if ((prevPosSteps - currentPosSteps) > 80) {

    } else if ((prevPosSteps - currentPosSteps) < 12) {
      stepper.setSpeed(LOW_SPEED);
      stepsPerTurn = 1;
    } else {  // if ((nextPosSteps - currentPosSteps) < 40) {
      stepper.setSpeed(MEDIUM_SPEED);
      stepsPerTurn = 10;
    }

    stepper.step(stepsPerTurn);
    currentPosSteps += stepsPerTurn;

    printCurrentPos(printAll, true);
    printAll = false;
  }

  stepsPerTurn *= -1;
  while (currentPosSteps > prevPosSteps) {

    if (abs(prevPosSteps - currentPosSteps) > 80) {

    } else if (abs(currentPosSteps - prevPosSteps) < 12) {
      stepper.setSpeed(LOW_SPEED);
      stepsPerTurn = -1;
    } else {  //if (abs(currentPosSteps - nextPosSteps) < 40) {
      stepper.setSpeed(MEDIUM_SPEED);
      stepsPerTurn = -10;
    }

    stepper.step(stepsPerTurn);
    currentPosSteps += stepsPerTurn;

    printCurrentPos(printAll, true);
    printAll = false;
  }

  motorOnIdle();
  digitalWrite(leftButtonLedPin, HIGH);
  digitalWrite(rightButtonLedPin, HIGH);
}

void positionMove() {

  // if (!moving) {
  if (printToLCD) {

    currentPosSteps = mmToSteps(distances[currentPosList % distanceListLength]);
    nextPosSteps = mmToSteps(distances[posInListToRange(currentPosList + 1, distanceListLength)]);

    int posInList = posInListToRange(currentPosList - 1, distanceListLength);
    prevPosSteps = mmToSteps(distances[posInList]);

    printCurrentAndNextPosition();
    printToLCD = false;  // prevent printing in loop without
  }

  long leftBtnPressedMillis = checkBtn(leftButtonPin, leftBtnStatus, lastChangeLeftBtnStatus);
  if (leftBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed long
    printToLCD = true;
    moveToNextPos();
    // update current position in list
    currentPosList = posInListToRange(currentPosList + 1, distanceListLength);
  }

  long rightBtnPressedMillis = checkBtn(rightButtonPin, rightBtnStatus, lastChangeRightBtnStatus);
  if (rightBtnPressedMillis > BTN_SHORT_TRESHOLD) {  // button pressed at least short
    printToLCD = true;
    // Serial.println("Move to previous position...");
    moveToPrevPos();
    // update current position in list
    currentPosList = posInListToRange(currentPosList - 1, distanceListLength);
  }
}



void loop() {

  if (menuMode) {
    selectionMenu();
  } else {
    positionMove();
  }
}