void bumperProcess() {
  for (int k = 0 ; k < 2 ; k++)
  {
    if (isReleased(k))
    {
      DEBUG_MOVE && Serial.write(251);
      if (isUnbumped(k))
      {
        DEBUG_MOVE && Serial.write(252);
        setUnbumped(k);
        stepper.bumpers[k].ONBUMPER = false;
      }
      if (isBumped(k))
      {
        DEBUG_MOVE && Serial.write(252);
        if (initializer){
          setBumperEnd(k);
        }
        setBumped(k);
        stepper.bumpers[k].ONBUMPER = false;
      }

    }
    if (isPress(k))
    {
      DEBUG_MOVE && Serial.write(251);
      stepper.bumpers[k].ONBUMPER = true;

      if ( initializer && stepper.bumpers[k].COUNTER % 4 == 0 )
      {
        setBumperStart(k);
      }
      stepper.bumpers[k].COUNTER++;
    }
  }
}

boolean isONaBumper(){
  for (int k = 0 ; k < 2 ; k++) {
    if(stepper.bumpers[k].ONBUMPER){
      return true;
    }
  }
};

bool isReleased (int bumperId ) {
  boolean r = !isPressed(stepper.bumpers[bumperId].PIN) && stepper.bumpers[bumperId].TOUCHED;
  if (r) {
    stepper.bumpers[bumperId].TOUCHED = false;
  }
  return r;
}

bool isPress (int bumperId ) {
  boolean r = isPressed(stepper.bumpers[bumperId].PIN) && !stepper.bumpers[bumperId].TOUCHED;
  if (r) {
    stepper.bumpers[bumperId].TOUCHED = true;
  }
  return r;
}

bool isPressed(int button) {
  return digitalRead(button) == HIGH;
}

void setUnbumped( int bumperId ) {
  stepper.bumpers[bumperId].BUMPED = false;
  stepper.bumpers[bumperId].COUNTER = 0;
}

void setBumped( int bumperId ) {
  stepper.bumpers[bumperId].BUMPED = true;
  stopMouvement();
}

void setBumperStart(int bumperId) {
  if (stepper.bumpers[bumperId].stepPositionBegin == 0) {
    stepper.bumpers[bumperId].stepPositionBegin = stepper.step;
  }
}
void setBumperEnd(int bumperId) {
  if (stepper.bumpers[bumperId].stepPositionEnd == 0) {
    stepper.bumpers[bumperId].stepPositionEnd = stepper.step;
    if ( stepper.bumpers[0].stepPositionEnd != 0 && stepper.bumpers[1].stepPositionEnd != 0 ) {
      initializer = false;
      if(DEBUG_MOVE){
        Serial.write(249);
        long L = getBumperSize(0);
        int _one = L / 16384;
        int _two = (L - (_one * 16384)) / 128;
        int _three = L % 128;
        Serial.write(_one);
        Serial.write(_two);
        Serial.write(_three);

        long _L = getBumperSize(1);
        int __one = _L / 16384;
        int __two = (_L - (__one * 16384)) / 128;
        int __three = _L % 128;
        Serial.write(_one);
        Serial.write(_two);
        Serial.write(_three);
      }
    }
  }
}

bool isBumped( int bumperId ) {
  return stepper.bumpers[bumperId].COUNTER % 4 == 2;
}

bool isUnbumped( int bumperId ) {
  return stepper.bumpers[bumperId].COUNTER % 4 == 0;
}

long getBumperSize(int bumperId) {
  return stepper.bumpers[bumperId].stepPositionEnd - stepper.bumpers[bumperId].stepPositionBegin;
}
