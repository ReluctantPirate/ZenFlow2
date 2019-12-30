enum goStates {INERT, PUSH, SHUFFLE, RESOLVE};
byte goState = INERT;

Color blinkColors[4] = {RED, YELLOW, CYAN, WHITE};
byte currentColor = random(3);
byte incomingColor;

void setup() {
  // put your setup code here, to run once:

}

void loop() {

  //do loops
  switch (goState) {
    case INERT:
      inertLoop();
      break;
    case PUSH:
      pushLoop();
      break;
    case SHUFFLE:
      shuffleLoop();
      break;
    case RESOLVE:
      resolveLoop();
      break;
  }

  //communicate!
  byte sendData = (goState << 2) + (currentColor);
  setValueSentOnAllFaces(sendData);

  //do visuals
  displayColors();
}

void inertLoop() {
  //look for neighbors telling me to PUSH
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //neighbor!
      byte neighborData = getLastValueReceivedOnFace(f);
      if (getGoState(neighborData) == PUSH && getColor(neighborData) == currentColor) {
        //this neighbor is telling me to PUSH to the next color
        goState = PUSH;
        incomingColor = (currentColor + 1) % 4;
      }
    }
  }

  //look for neighbors telling me to SHUFFLE
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //neighbor!
      byte neighborData = getLastValueReceivedOnFace(f);
      if (getGoState(neighborData) == SHUFFLE) {
        //begin shuffling!
        goState = SHUFFLE;
        currentColor = random(3);
      }
    }
  }

  //listen for button presses
  if (buttonSingleClicked()) {
    goState = PUSH;
    incomingColor = (currentColor + 1) % 4;
  }

  if (buttonDoubleClicked()) {
    goState = SHUFFLE;
    incomingColor = random(3);
  }
}

void pushLoop() {
  //so in order to move to RESOLVE, we need to know that all of our same-colored neighbors are in PUSH
  goState = RESOLVE;//defaults to RESOLVE, but can be changed later
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //neighbor!
      byte neighborData = getLastValueReceivedOnFace(f);
      if (getColor(neighborData) == currentColor) {//this neighbor is our color
        if (getGoState(neighborData) == INERT) {//this neighbor is still inertly that color
          goState = PUSH;//don't go to RESOLVE yet
        }
      }
    }
  }

  //look for neighbors telling me to SHUFFLE
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //neighbor!
      byte neighborData = getLastValueReceivedOnFace(f);
      if (getGoState(neighborData) == SHUFFLE) {
        //begin shuffling!
        goState = SHUFFLE;
        incomingColor = random(3);
      }
    }
  }
}

void shuffleLoop() {
  //so in order to move to RESOLVE, we need to know that all of our neighbors are in SHUFFLE or RESOLVE
  goState = RESOLVE;//defaults to RESOLVE, but can be changed later
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //neighbor!
      byte neighborData = getLastValueReceivedOnFace(f);
      if (getGoState(neighborData) == INERT || getGoState(neighborData) == PUSH) {//this neighbor is still INERT or PUSH
        goState = SHUFFLE;//don't go to RESOLVE yet
      }
    }
  }
}

void resolveLoop() {
  //so in order to move to INERT, we need to know that all of our same-colored neighbors are in RESOLVE
  goState = INERT;//defaults to INERT, but can be changed later
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //neighbor!
      byte neighborData = getLastValueReceivedOnFace(f);
      if (getGoState(neighborData) == PUSH) {//this neighbor is still pushing
        goState = INERT;//don't go to RESOLVE yet
        currentColor = incomingColor;
      }
    }
  }
}

byte getGoState(byte data) {
  return (data >> 2);
}

byte getColor(byte data) {
  return (data & 3);
}

void displayColors() {

  switch (goState) {
    case INERT:
      setColor(blinkColors[currentColor]);
      break;
    case PUSH:
      setColorOnFace(blinkColors[currentColor], 0);
      setColorOnFace(blinkColors[currentColor], 2);
      setColorOnFace(blinkColors[currentColor], 4);
      setColorOnFace(blinkColors[incomingColor], 1);
      setColorOnFace(blinkColors[incomingColor], 3);
      setColorOnFace(blinkColors[incomingColor], 5);
      break;
    case SHUFFLE:
      setColorOnFace(blinkColors[currentColor], 0);
      setColorOnFace(blinkColors[currentColor], 2);
      setColorOnFace(blinkColors[currentColor], 4);
      setColorOnFace(blinkColors[incomingColor], 1);
      setColorOnFace(blinkColors[incomingColor], 3);
      setColorOnFace(blinkColors[incomingColor], 5);
      break;
    case RESOLVE:
      setColorOnFace(blinkColors[currentColor], 1);
      setColorOnFace(blinkColors[currentColor], 3);
      setColorOnFace(blinkColors[currentColor], 5);
      setColorOnFace(blinkColors[incomingColor], 0);
      setColorOnFace(blinkColors[incomingColor], 2);
      setColorOnFace(blinkColors[incomingColor], 4);
      break;
  }

}
