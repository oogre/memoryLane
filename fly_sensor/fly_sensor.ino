/*
  CAPTEUR IR DEBUG : 
    ARDUINO Alim + Cable normal :     ~650 + la value du capteur ne change pas ac la position du rocher
    ARDUINO NO Alim + Cable normal :  ~700 + la value du capteur ne change pas ac la position du rocher
    ARDUINO Alim + Cable debug :      [200, 300] lorsque le rocher est contre ou absent 
                                      [300, 500] lorsque le rocher est dans la zone de levitation
    ARDUINO NO Alim + Cable debug :   [0, 100]   lorsque le rocher est contre ou absent 
                                      [600, 200] lorsque le rocher est dans la zone de levitation
*/
#define SERIAL_PORT 115200

/*
  ROCK_INTHE_AIR ---- DEFAULT [150,700]
  CAPTOR RANGE VALUE WHEN ROCK IS LEVITATING
*/
#define ROCK_INTHE_AIR_MIN  150
#define ROCK_INTHE_AIR_MAX  700

/*
  MESURING_INTERVAL ---- DEFAULT 10
  PROCESS TO A MESUREMENT EVERY "MESURING_INTERVAL" µs
*/
#define MESURING_INTERVAL   50

/*
  NUM_READINGS ---- DEFAULT 10
  MATH AN AVERAGE ON "NUM_READINGS" VALUES TO DEFINE STATE
*/

#define NUM_READINGS        10

/*
  NUM_TO_BE_SURE ---- DEFAULT 10
  STATE HAVE TO BE THE SAME DURING "NUM_TO_BE_SURE" MESURMENTS TO BE VALIDATED
*/
#define NUM_TO_BE_SURE      10

/*
  FALL_BUTTON ---- DEFAULT 11 | 10 | 9
  digitalRead(FALL_BUTTON) == HIGH > ROCK ISN'T LEVITATING
  digitalRead(FALL_BUTTON) == LOW > ROCK IS LEVITATING
  THESE ARE PLUG INTO MOTOR DRIVERS IN PIN 2
  WHEN IT'S HIGH, EVERY MOVE ARE RAUGHLY STOPED
  RESTART THE MOVE : MAKE THE ROCK LEVITATE AND TURN OFF AND ON THE SWITCH (HOLD_BUTTON)
*/
#define FALL_BUTTON_1_PIN   11
#define FALL_BUTTON_2_PIN   10
#define FALL_BUTTON_3_PIN   9
#define PIN_CAPTOR          A0

/*
  DEBUG_STATE ---- DEFAULT false
  true :  Serial.println("fly") when rock is flying
          Serial.println("fall") when rock is fell
  false : nothing
*/
#define DEBUG_STATE false

/*
  DEBUG_CAPTOR_VALUE ---- DEFAULT false
  true : Serial.println(averaged(analogRead(PIN_CAPTOR)))
  false : nothing
*/
#define DEBUG_CAPTOR_VALUE true

/*
  FIN DE LA CONFIGURATION
*/

// VAR FOR DECISION : areYouSure()
int sure_cmp = 0;
boolean flying_state = false;

// VAR FOR AVERAGING : getSensorValue()
int readings [NUM_READINGS];
int index = 0;
long total = 0;
boolean ready = false;


byte DEBUG_STATE_FLAG = 0;


void setup()
{
  Serial.begin(SERIAL_PORT);

  pinMode(FALL_BUTTON_1_PIN, OUTPUT);
  digitalWrite(FALL_BUTTON_1_PIN, LOW);

  pinMode(FALL_BUTTON_2_PIN, OUTPUT);
  digitalWrite(FALL_BUTTON_2_PIN, LOW);

  pinMode(FALL_BUTTON_3_PIN, OUTPUT);
  digitalWrite(FALL_BUTTON_3_PIN, LOW);

  pinMode(PIN_CAPTOR, INPUT);

  for (int i = 0 ; i < NUM_READINGS ; i ++)
  {
    readings[i] = 0;
  }
};

void loop()
{
  switch (areYouSure(isRockFlying(getSensorValue()))) {
    case 0 :  // ROCK IS FELL
      setFell();
      break;
    case 1:   // ROCK IS FLYIN
      setFying();
      break;
    case 2 :  // ROCK STATE NOT SURE
      // NOTHING
      break;
  }
  delay(MESURING_INTERVAL);
};

byte areYouSure(bool isFlying)
{
  if (flying_state == isFlying)
  {
    sure_cmp ++;
    if (sure_cmp >= NUM_TO_BE_SURE)
    {
      return isFlying ? 1 : 0;
    }
  }
  else
  {
    flying_state = isFlying;
    sure_cmp = 0;
  }
  return 2;
}

bool isRockFlying (int value)
{
  return value > ROCK_INTHE_AIR_MIN && value < ROCK_INTHE_AIR_MAX;
}

int getSensorValue()
{
  total = total - readings[index];
  readings[index] = analogRead(PIN_CAPTOR);
  total = total + readings[index];
  index ++;
  if (index >= NUM_READINGS)
  {
    index = 0;
    ready = true;
  }
  int tmp = total / NUM_READINGS;
  DEBUG_CAPTOR_VALUE && Serial.println(tmp);
  return tmp;
}

void setFell()
{
  if (!ready) return;
  if (DEBUG_STATE && (DEBUG_STATE_FLAG == 1 ||  DEBUG_STATE_FLAG == 0)) {
    Serial.println("fall");
    DEBUG_STATE_FLAG = 2;
  }
  digitalWrite(FALL_BUTTON_1_PIN, HIGH);
  digitalWrite(FALL_BUTTON_2_PIN, HIGH);
  digitalWrite(FALL_BUTTON_3_PIN, HIGH);
}

void setFying()
{
  if (!ready) return;
  if (DEBUG_STATE && (DEBUG_STATE_FLAG == 2 ||  DEBUG_STATE_FLAG == 0)) {
    Serial.println("fly");
    DEBUG_STATE_FLAG = 1;
  }
  digitalWrite(FALL_BUTTON_1_PIN, LOW);
  digitalWrite(FALL_BUTTON_2_PIN, LOW);
  digitalWrite(FALL_BUTTON_3_PIN, LOW);
}
