typedef struct CMD{
  unsigned long MIN_MOVE;
  unsigned long MAX_MOVE;
  unsigned long MIN_SPEED;
  unsigned long MAX_SPEED;
};

typedef struct MOTOR{
  char NAME [30];
  int STEP_PIN;
  int DIR_PIN;
  long step;
  unsigned long step_to_run;
  unsigned long step_runned;
  int dir;
  unsigned long startTime;
};

