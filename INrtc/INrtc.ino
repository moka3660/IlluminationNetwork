#include<Wire.h>
#include<TimeLib.h>
#include<DS1307RTC.h>

#define NOOP  12

bool flag = false;

struct TrainTime{
  int hour;
  int minute;
  bool deperture; //発着  true:発車   false:到着
  bool direction; //方向  true:西舞鶴・綾部方面   false:小浜・敦賀方面
};



struct TrainTime traintime[NOOP] = {
  {17, 1,true, true},//0   //17時から22時(終電)
  {17,26,false,true},
  {17,40,false,false},
  {17,47,true, true},
  {18, 4,true,false},
  {18, 8,true, true},
  {18,32,false,true},
  {18,46,false,false},
  {18,51,true, true},
  {18,59,true,false},
  {19,16,false,true},
  {19,38,false,false},
  {19,39,true, true},
  {19,53,false,true},
  {19,55,true,false},
  {20,33,false,false},
  {20,35,true, true},
  {21,01,false,true},
  {21,18,true,false},
  {21,38,true, true},
  {21,51,false,true},
  {21,59,false,false},
  {22, 0,true, true}
};

void setup()
{
  pinMode(8,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  while(!Serial); //wait for serial

  digitalWrite(8,LOW);
}

void loop()
{
  delay(300);
  tmElements_t tm;  //
  RTC.read(tm);   //時間取得

  int i;
  for(i=0;i<NOOP;i++)
  {
    if((tm.Hour==traintime[i].hour) && (tm.Minute==traintime[i].minute))
    {
      digitalWrite(13,HIGH);
      flag = true;  //フラグ立てる
      break;  //脱出
    }
  }

  if(flag == true)
  {
    digitalWrite(8,HIGH);
    switch(traintime[i].deperture)
    {
      case true:
        Serial.write('D');
        delay(100);//一応delay
        break;
      case false:
        Serial.write('A');
        delay(100);//一応delay
        break;
    }
    digitalWrite(8,LOW);
    digitalWrite(13,LOW);
  }else
  {
    digitalWrite(12,HIGH);
    delay(200);
    digitalWrite(12,LOW);
  }

  flag = false; //フラグを落とす

}
