

bool isHolded(){
  return digitalRead(HOLD_BUTTON) == LOW ;
}

bool isFell(){
  return digitalRead(FALL_BUTTON) == HIGH ;
}
