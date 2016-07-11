#include<SoftwareSerial.h>

#define RXpin 6
#define TXpin 7
//シリアルポート設定
SoftwareSerial SSerial = SoftwareSerial(RXpin,TXpin);

volatile byte state = 0;
volatile bool flag = false;

void setup()
{
  SSerial.begin(9600);
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
        SSerial.write('T');
        delay(100);
        break;
      case 2:
        SSerial.write('P');
        delay(100);
        break;
      case 1:
        SSerial.write('P');
        delay(100);
        break;
      case 0:
        SSerial.write('S');
        delay(100);
        break;
      default:
        SSerial.write('P');
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
