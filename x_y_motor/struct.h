typedef struct BUMPER {
  int PIN;
  bool TOUCHED;
  bool BUMPED;
  bool ONBUMPER;
  unsigned long autoReset;
  bool startedAutoReset;
  long stepPositionBegin;
  long stepPositionEnd;
  char NAME [30];
  int COUNTER;
};

typedef struct MOTOR{
  char NAME [30];
  int STEP_PIN;
  int DIR_PIN;
  BUMPER bumpers [2];
  long step;
  unsigned long step_to_run;
  unsigned long step_runned;
  int dir;
  unsigned long startTime;
};

typedef struct CMD{
  unsigned long MIN_MOVE;
  unsigned long MAX_MOVE;
  unsigned long MIN_SPEED;
  unsigned long MAX_SPEED;
};


