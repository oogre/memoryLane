
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
}



unsigned long getDelay() {
  float toRun = (float) stepper.step_to_run;
  float runned = (float) stepper.step_runned;
  float d = (float) (abs(toRun - runned) / (toRun + runned));

  return (MIN__SPEED + (MAX__SPEED * pow(d, 10)));
}



void resetMouvement() {
  int x = random(min(MIN__MOVE, MAX__MOVE), max(MIN__MOVE, MAX__MOVE));
  if(x == 0) x = 1;
  
  stepper.dir = x > 0 ? HIGH : LOW;
  stepper.step_to_run = abs(x * ALPHA);
  stepper.step_runned = 0;
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
