#include "struct.h"

#define SERIAL_PORT 115200

/* 
	HOLD_BUTTON ---- DEFAULT : 3
	PIN ARDUINO POUR METTRE LE MOUVEMENT EN PAUSE
	digitalRead(HOLD_BUTTON) == HIGH > PAUSE
	digitalRead(HOLD_BUTTON) == LOW > RUN
	
*/
#define HOLD_BUTTON 3

/* 
	FALL_BUTTON ---- DEFAULT : 2
	PIN ARDUINO POUR METTRE LE MOUVEMENT EN PAUSE SI LE ROCHER TOMBE
	digitalRead(FALL_BUTTON) == HIGH > PAUSE
	digitalRead(FALL_BUTTON) == LOW > RUN
	
	FALL_SENSOR ---- DEFAULT : false
	FALSE : FALL_BUTTON EST DÉSACTIVÉ
	TRUE : FALL_BUTTON EST ACTIVÉ
*/
#define FALL_BUTTON 2
#define FALL_SENSOR false

/*
	AFTER_BUMP_STEP ---- DEFAULT 10000
	NOMBRE DE STEP MAXIMUM APRES LE PASSAGE SUR UN CAPTEUR DE FIN DE COURSE
*/
#define AFTER_BUMP_STEP 10000

/*
	DEBUG_MOVE ---- DEFAULT false
	TRUE : 	
			LORSQUE LE BOUTON DE FIN DE COURSE EST RELACHÉ
				SERIAL.write 251 
			LORSQUE LE STATUS DE FIN DE COURSE EST ENCLENCHÉ / DÉSENCLENCHÉ
				SERIAL.write 252 
			LORSQUE LES POSTIONS DES 2 CAPTEURS DE FIN DE COURSES SONT CONNUES : FIN DU MODE D'INITIALISATION
				SERIAL.write 249 
			NEW_MOVE : 
				SERIAL.write 255 (SPEED en µs/step)
				SERIAL.write (SPEED / 16384)
				SERIAL.write (SPEED % 16384) / 128 
				SERIAL.write (SPEED % 128) 
				SERIAL.write 254 
				SERIAL.write (STEP_TO_RUN / 16384)
				SERIAL.write (STEP_TO_RUN % 16384) / 128 
				SERIAL.write (STEP_TO_RUN % 128) 
				SERIAL.write 253
				SERIAL.write (STEP_RUNNED / 16384)
				SERIAL.write (STEP_RUNNED % 16384) / 128 
				SERIAL.write (STEP_RUNNED % 128) 
	FALSE : NOTHING
*/
#define DEBUG_MOVE false

/*
	DEBUG_DRAW ---- DEFAULT false
	TRUE : 	SERIAL.write 255 
			SERIAL.write (POSITION_IN_STEP / 16384)
			SERIAL.write (POSITION_IN_STEP % 16384) / 128 
			SERIAL.write (POSITION_IN_STEP % 128) 
	FALSE : NOTHING
*/
#define DEBUG_DRAW false

/*
	ALPHA ---- DEFAULT 1000
	NOMBRE DE STEP PAR HUITIÉME DE REVOLUTION ( 45deg OR π/4RAD )
*/
#define ALPHA 1000
/*
	MOVE : GAMME DE DISTANCE EN STEP
	MIN__MOVE : LE PLUS PETIT NOMBRE DE STEP POSSIBLE (MOVE x ALPHA) ---- DEFAULT -4
	MAN__MOVE : LE PLUS GRAND NOMBRE DE STEP POSSIBLE (MOVE x ALPHA) ---- DEFAULT  4
*/
int MIN__MOVE = -4;
int MAX__MOVE = 4;

/*
	SPPED : GAMME DE VITESSE EN MICROSECONDE PAR STEP  (µs/step)
	MIN__SPEED : LE PLUS PLUS GRANDE VITESSE ---- DEFAULT 20000 µs/step
	MAX__SPEED : LE PLUS PETITE VITESSE ----  DEFAULT 50000 µs/step
*/
unsigned long MIN__SPEED = 20000;
unsigned long MAX__SPEED = 50000;

/*
	TIME_BEFORE_MOVE : GAMME DE TEMPS DE PAUSE EN µsecond
	MIN_TIME_BEFORE_MOVE : LE PLUS PETIT TEMPS DE PAUSE POSSIBLE ---- DEFAULT 10000
	MAX_TIME_BEFORE_MOVE : LE PLUS GRAND TEMPS DE PAUSE POSSIBLE ---- DEFAULT 40000
*/
int MIN_TIME_BEFORE_MOVE = 10000;
int MAX_TIME_BEFORE_MOVE = 40000;

MOTOR stepper = {
	"Rotor",		// NAME 		---- DEFAULT : Rotor
	11,				// STEP_PIN 	---- DEFAULT : 11
	10,				// DIR_PIN 		---- DEFAULT : 10
	0,				// step
	0,				// step_to_run
	0,				// step_runned
	HIGH,			// dir
	0				// startTime
};


/*
	FIN DE LA CONFIGURATION
*/
boolean holded = false;
boolean fell = false;
boolean restartProc = false;


void setup()
{
	Serial.begin(SERIAL_PORT);
	Serial.flush();

	// SETUP HOLD BUTTON
	pinMode(HOLD_BUTTON, INPUT);
	digitalWrite(HOLD_BUTTON, LOW);

  
	// SETUP STEPPER
	pinMode(stepper.STEP_PIN, OUTPUT);
	pinMode(stepper.DIR_PIN, OUTPUT);

	// SETUP FALL BUTTON
	if(FALL_SENSOR){
		pinMode(FALL_BUTTON, INPUT);
		digitalWrite(FALL_BUTTON, LOW);
	}
}

void loop()
{
	if(FALL_SENSOR && isFell())
	{
		restartProc = true;
		resetMouvement();
	}
	if (isHolded())
	{
		stopMouvement();
		while (!isMoveFinished())
		{
			move();
		}
		resetMouvement();
	}
	while(restartProc || isHolded())
	{
		if (isHolded())
		{
			restartProc = false;
		}
		delay(1000);
	}

	// END MOVING PROCESS
	if(isMoveFinished())
	{
		long t0 = millis();
		int t = random(MIN_TIME_BEFORE_MOVE, MAX_TIME_BEFORE_MOVE);
		while(millis() - t0 < t );
		resetMouvement();
	}

	// MOVING PROCESS
	move();
}


