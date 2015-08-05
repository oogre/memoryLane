long t0 = 0;

void move() {
  digitalWrite(stepper.DIR_PIN, stepper.dir);
  unsigned long d = getDelay();
  digitalWrite(stepper.STEP_PIN, HIGH);
  myDelay(d);
  digitalWrite(stepper.STEP_PIN, LOW);
  myDelay(d);
  stepper.step += (stepper.dir == HIGH ? 1 : -1);
  stepper.step_to_run --;
  stepper.step_runned ++;

  if (DEBUG_DRAW && millis() - t0 > 100) {
    t0 = millis();

    long D = abs(stepper.step);
    int __one = D / 16384;
    int __two = (D - (__one * 16384)) / 128;
    int __three = D % 128;

    Serial.write(255);
    Serial.write(stepper.step > 0 ? 1 : 0);
    Serial.write(__one);
    Serial.write(__two);
    Serial.write(__three);
  }
}

unsigned long getDelay() {
  float toRun = (float) stepper.step_to_run;
  float runned = (float) stepper.step_runned;
  float d = (float) (abs(toRun - runned) / (toRun + runned));

  long D = (MIN__SPEED + (MAX__SPEED * pow(d, 10)));

  if (DEBUG_MOVE && millis() - t0 > 100) {
    t0 = millis();

    int __one = D / 16384;
    int __two = (D - (__one * 16384)) / 128;
    int __three = D % 128;

    Serial.write(255);
    Serial.write(__one);
    Serial.write(__two);
    Serial.write(__three);

    int one = stepper.step_to_run / 16384;
    int two = (stepper.step_to_run - (one * 16384)) / 128;
    int three = stepper.step_to_run % 128;
    Serial.write(254);
    Serial.write(one);
    Serial.write(two);
    Serial.write(three);

    int _one = stepper.step_runned / 16384;
    int _two = (stepper.step_runned - (_one * 16384)) / 128;
    int _three = stepper.step_runned % 128;
    Serial.write(253);
    Serial.write(_one);
    Serial.write(_two);
    Serial.write(_three);
  }
  return D;
}

void resetMouvement() {
  MIN__MOVE = rail.MIN_MOVE;
  MAX__MOVE = rail.MAX_MOVE;
  MIN__SPEED = rail.MIN_SPEED;
  MAX__SPEED = rail.MAX_SPEED;

  if (initializer) {
    stepper.step_to_run = MAX__MOVE;
  }
  else {
    unsigned long _max = MAX__MOVE;
    unsigned long _min = (stepper.bumpers[0].BUMPED || stepper.bumpers[1].BUMPED) ?
                         max(MIN__MOVE , (AFTER_BUMP_STEP * 2))
                         :
                         MIN__MOVE ;
    stepper.step_to_run = abs(random(min(_min, _max), max(_min, _max)));
    for (int k = 0 ; k < 2 ; k++) {
      if ( isDestinationIsOnBumper(k))
      {
        DEBUG_MOVE && Serial.write(250);
        long bumpersize = getBumperSize(k);
        stepper.step_to_run += (2 * bumpersize);
      }
    }
  }
  stepper.step_runned = 0;

  if(!isONaBumper()){
    stepper.dir = !stepper.dir;
  }
}

bool isDestinationIsOnBumper(int bumperId) {
  long destination = stepper.step + ( (stepper.dir ? 1 : -1) * stepper.step_to_run);
  return  destination - 1000 >= stepper.bumpers[bumperId].stepPositionBegin &&
          destination + 1000 <= stepper.bumpers[bumperId].stepPositionEnd;
}

void stopMouvement() {
  if (stepper.step_to_run > AFTER_BUMP_STEP) {
    float toRun = (float) stepper.step_to_run;
    float runned = (float)stepper.step_runned;
    stepper.step_to_run =  AFTER_BUMP_STEP;
    stepper.step_runned = (long) (AFTER_BUMP_STEP * runned) / toRun;
    if (stepper.step_to_run > stepper.step_runned) {
      long tmp = stepper.step_to_run ;
      stepper.step_to_run  = stepper.step_runned;
      stepper.step_runned = tmp ;
    }
  }
}

void myDelay(long v) {
  if (v < 16383) delayMicroseconds(v);
  else        delay((int)(v / 1000.0));
}

bool isMoveFinished() {
  return stepper.step_to_run == 0;
}
