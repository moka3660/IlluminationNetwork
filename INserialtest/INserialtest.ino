void setup()
{
  pinMode(8,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(A0,INPUT);
  Serial.begin(9600);
  while(!Serial); //wait for serial

  digitalWrite(8,LOW);
}

void loop()
{
  int pin=0;
  digitalRead(A0);
  if(pin == 1)
  {
    digitalWrite(8,HIGH);
    Serial.write('D');
    delay(100);
    digitalWrite(8,LOW);
    delay(900);

  }
}

/*
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
/*    delay(15000);
    digitalWrite(13,LOW);

    digitalWrite(12,HIGH);
    delay(1000);
    digitalWrite(12,LOW);
  }

 if((tm.Hour==traintime[0].hour) && (tm.Minute==traintime[0].minute))
  {
    digitalWrite(13,HIGH);
    delay(500);
    digitalWrite(13,LOW);
  }
  else
  {
    digitalWrite(12,HIGH);
    delay(500);
    digitalWrite(12,LOW);
  }
}
*/
