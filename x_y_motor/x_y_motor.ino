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
	PIN ARDUINO POUR STOPPER LE MOUVEMENT RAPIDEMENT
	digitalRead(FALL_BUTTON) == HIGH > EVERY MOVE ARE RAUGHLY STOPED
	digitalRead(FALL_BUTTON) == LOW > NOTHING

	TO RESTART THE MOVE : MAKE THE ROCK LEVITATE AND TURN OFF AND ON THE SWITCH (HOLD_BUTTON)
	
	FALL_SENSOR ---- DEFAULT : false
	false : FALL_BUTTON EST DÉSACTIVÉ
	true : FALL_BUTTON EST ACTIVÉ
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
      Serial.write (0 SI POSITION NÉGATIVE | 1 SI POSITION POSITIVE);
			SERIAL.write (POSITION_IN_STEP / 16384)
			SERIAL.write (POSITION_IN_STEP % 16384) / 128 
			SERIAL.write (POSITION_IN_STEP % 128) 
	FALSE : NOTHING
*/
#define DEBUG_DRAW false

/*
	MOVE : GAMME DE DISTANCE EN STEP
	MIN__MOVE : LE PLUS PETIT NOMBRE DE STEP POSSIBLE ---- DEFAULT 60000
	MAN__MOVE : LE PLUS GRAND NOMBRE DE STEP POSSIBLE ---- DEFAULT 200000
*/
unsigned long MIN__MOVE = 60000;
unsigned long MAX__MOVE = 200000;

/*
	SPPED : GAMME DE VITESSE EN MICROSECONDE PAR STEP  (µs/step)
	MIN__SPEED : LE PLUS PLUS GRANDE VITESSE ---- DEFAULT 10 µs/step
	MAX__SPEED : LE PLUS PETITE VITESSE ----  DEFAULT 2000 µs/step
*/
unsigned long MIN__SPEED = 10;
unsigned long MAX__SPEED = 2000;

MOTOR stepper = {
	"Y axis",			// NAME 		---- DEFAULT : X axis 	or	Y axis
	11,					// STEP_PIN 	---- DEFAULT : 11
	10,					// DIR_PIN 		---- DEFAULT : 10
	{// CAPTEURS FIN DE COURSE
		{// CAPTEURS FIN DE COURSE DÉPLACEMENT NÉGATIF 
			7,				// PIN 			---- DEFAULT : 7 		or	5
			false,			// TOUCHED
			false,			// BUMPED
			false,			// ONBUMPER
			0,				// autoReset timer
			false,			// startedAutoReset
			0,				// stepPositionBegin
			0,				// stepPositionEnd
			"MIN",			// NAME 		---- DEFAULT : MIN
			0				// COUNTER
		},
		{// CAPTEURS FIN DE COURSE DÉPLACEMENT POSITIF 
			6,				// PIN 			---- DEFAULT : 6 		or	4
			false,			// TOUCHED
			false,			// BUMPED
			false,			// ONBUMPER
			0,				// autoReset timer
			false,			// startedAutoReset
			0,				// stepPositionBegin
			0,				// stepPositionEnd
			"MAX",			// NAME 		---- DEFAULT : MAX
			0				// COUNTER
		}
	},
	0,				// step
	0,				// step_to_run
	0,				// step_runned
	HIGH,			// dir
	0 				// startTime
};

/*
	FIN DE LA CONFIGURATION
*/


boolean holded = false;
boolean fell = false;
boolean restartProc = false;
boolean initializer = true;

void setup()
{
	Serial.begin(SERIAL_PORT);
	Serial.flush();

	// SETUP HOLD BUTTON
	pinMode(HOLD_BUTTON, INPUT);
	digitalWrite(HOLD_BUTTON, LOW);

	// SETUP BUMPERS BUTTON
	pinMode(stepper.bumpers[0].PIN, INPUT);
	pinMode(stepper.bumpers[1].PIN, INPUT);
	
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
			bumperProcess();
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

	if(consumeUntilHeader())
	{
		readCommand();
	}

	// END MOVING PROCESS
	if(isMoveFinished())
	{
		resetMouvement();
	}

	// MOVING PROCESS
	bumperProcess();
	move();
}


