#define UNBREAKER 223
#define BREAKER 222
#define HEADER 255
#define FOOTER 254
#define MESSAGE_LENGTH 12

typedef struct BUFFER{
  byte current;
  byte cursor;
  byte buffer [MESSAGE_LENGTH];
};

BUFFER buffer = {
  0, 0,
  { 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
  }
};

CMD rail = {
  MIN__MOVE, MAX__MOVE, MIN__SPEED, MAX__SPEED
};

boolean consumeUntilHeader()
{
  while(Serial.available())
  {
    byte value = Serial.read();
    if(value == HEADER)
    {
      return true;
    }
  }
  return false;
}

void readCommand()
{
  while(true)
  {
    if(Serial.available())
    {
      buffer.current = Serial.read();
      if(buffer.current == FOOTER || buffer.cursor == MESSAGE_LENGTH)
      {
        rail.MIN_MOVE  = ((unsigned long)buffer.buffer[0]) * ((unsigned long)16384);
        rail.MIN_MOVE += ((unsigned long)buffer.buffer[1]) * ((unsigned long)128);
        rail.MIN_MOVE += ((unsigned long)buffer.buffer[2]);

        rail.MAX_MOVE  = ((unsigned long)buffer.buffer[3]) * ((unsigned long)16384);
        rail.MAX_MOVE += ((unsigned long)buffer.buffer[4]) * ((unsigned long)128);
        rail.MAX_MOVE += ((unsigned long)buffer.buffer[5]);

        rail.MIN_SPEED  = ((unsigned long)buffer.buffer[6]) * ((unsigned long)16384);
        rail.MIN_SPEED += ((unsigned long)buffer.buffer[7]) * ((unsigned long)128);
        rail.MIN_SPEED += ((unsigned long)buffer.buffer[8]);

        rail.MAX_SPEED  = ((unsigned long)buffer.buffer[9]) * ((unsigned long)16384);
        rail.MAX_SPEED += ((unsigned long)buffer.buffer[10]) * ((unsigned long)128);
        rail.MAX_SPEED += ((unsigned long)buffer.buffer[11]);

        buffer.cursor     = 0;
        break;
      }
      else if(buffer.current == BREAKER)
      { 
        holded = true;
        buffer.cursor     = 0;
        break;
      }
      else if(buffer.current == UNBREAKER)
      { 
        holded = false;
        buffer.cursor     = 0;
        break;
      }
      else
      {
        buffer.buffer[buffer.cursor++] = buffer.current;
      }
    }
  } 
}
