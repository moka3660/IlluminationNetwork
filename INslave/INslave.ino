volatile byte state = 0;
volatile bool flag = false;


void setup()
{
  Serial.begin(9600);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  attachInterrupt(0,PinRead,CHANGE);
  attachInterrupt(1,PinRead,CHANGE);
}

void loop()
{
  while(flag == true)
  {
    //割り込み停止
    detachInterrupt(0);
    detachInterrupt(1);

    switch(state)
    {
      case 3:
        Serial.write('T');
        delay(100);
        break;
      case 2:
        Serial.write('P');
        delay(100);
        break;
      case 1:
        Serial.write('P');
        delay(100);
        break;
      case 0:
        Serial.write('S');
        delay(100);
        break;
      default:
        Serial.write('P');
        delay(100);
    }
    attachInterrupt(0,PinRead,CHANGE);
    attachInterrupt(1,PinRead,CHANGE);
    flag = false;
  }
}

void PinRead()
{
  state = digitalRead(2);
  state = (state << 1) | digitalRead(3);
  flag = true;
}
