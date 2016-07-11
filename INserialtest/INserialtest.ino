void setup()
{
  pinMode(8,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(A0,INPUT);
  Serial.begin(9600);
  while(!Serial); //wait for serial

  digitalWrite(8,LOW);
  delay(3000);
  digitalWrite(8,HIGH);
  Serial.write('D');
  delay(100);
  digitalWrite(8,LOW);
  delay(900);
}

void loop()
{
  delay(100);
}
