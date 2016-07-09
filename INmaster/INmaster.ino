/*  変更点(6/21のチェック以降)
  waittime
  mode
  switch
  割り込みの
  whileの中身
*/

/*  マザーから各イルミへ(DIO1,0)
    全点灯   11
    パターン  01
    消灯     00               */

/*  各イルミArduinoから各Xbeeへ(DIO08)
    ついてる  1
    消えてる  0               */

#include<Wire.h>
#include<XBeeLibrary.h>
XBeeLibrary XBee;

const int LED0 = 8;
const int LED1 = 9;
const int LED2 = 10;
#define waittime 7000    //各オブジェ待機時間
#define gamelimit 33000   //ゲームの上限時間(33sec)
volatile int mode = 0;    //点灯モード
volatile int train = 0;  //電車
volatile int dep = false;    //到着
volatile int arri = false;   //発車
volatile int game = 0;    //入力
char traindata; //シリアル電車データ
byte data;  //入力データ


void setup()        //セットアップ
{
  //xbee設定
  XBee.setup(true);
  XBee.setAddress(0x0013A200,0x40B1D24B);   //1
  XBee.setAddress(0x0013A200,0x40B1D240);   //2
  XBee.setAddress(0x0013A200,0x40B40160);   //3
  XBee.setAddress(0x0013A200,0x40B401C9);   //4
  XBee.setAddress(0x0013A200,0x40B401BF);   //5
  XBee.setAddress(0x0013A200,0x40B4017C);   //6
  //LEDピン
  pinMode(LED0,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT); //割り込み時点灯
  pinMode(13,OUTPUT);
  //入力ピン
  pinMode(A8,INPUT);//電車
  pinMode(A9,INPUT);//ゲーム
  //シリアル
  Serial1.begin(9600);    //らずパイ
  Serial2.begin(9600);    //RTCのUNO
  Serial3.begin(9600);    //入力(C2班)
  //LED消灯
  digitalWrite(LED0,LOW);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(13,LOW);
}

void loop()       //メイン
{
  XBee.loopAction();

  mode = random(0,6);
  switch(mode)
  {
    case 0:
      All();
      break;
    case 1:
      ForwardSingle();
      break;
    case 2:
      ReverseSingle();
      break;
    case 3:
      RandomSingle();
      break;
    case 4:
      RandomAll();
      break;
    default:
      All();
  }

/*  //入力再読み込み
  train = digitalRead(A8);
  game = digitalRead(A9);
*/

  //電車よ
  while(train == 1)
  {
    if(Serial2.available() > 0)
    {
      traindata = Serial2.read();//読み込み
      if(traindata == 'D')
      {
        digitalWrite(LED2,HIGH);
        ForwardAll();
        digitalWrite(LED2,LOW);
      }
      else /*if(traindata == 'A')*/
      {
        digitalWrite(LED2,HIGH);
        ReverseAll();
        digitalWrite(LED2,LOW);
      }
    }
    //  到着D 発車A
    train = 0;
  }

  //ゲームヨ
  if(game == 1)
  {
    unsigned long time;
    //時間取得(開始した時間)
    time = millis();
    while(((millis()-time)<gamelimit)&&(game == 1))
    {
      data == Serial3.read();//シリアル3　受信
      //データ通りに光らす(データは1バイト)

      game = digitalRead(A9);//チェック
    }
  }

  train = 0;
  game = 0;

  digitalWrite(13,HIGH);
  delay(3000);
  digitalWrite(13,LOW);

}


void TurnOff()    //全消灯
{
  //全ての子機を消灯状態(00)に
  XBee.sendRemoteATCommand(0, "D0", false, 1, 4);
  XBee.sendRemoteATCommand(0, "D1", false, 1, 4);
  //適用コマンド送信
  XBee.loopAction();
  //LED消灯
  digitalWrite(LED0,LOW);
  digitalWrite(LED1,LOW);
}

void All()          //全部つける
{
  //全ての子機を点灯状態(11)に
  XBee.sendRemoteATCommand(0, "D0", false, 1, 5);
  XBee.sendRemoteATCommand(0, "D1", false, 1, 5);
  //適用コマンド送信
  XBee.loopAction();
  //LED点灯
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,HIGH);
}

void ForwardSingle()    //順方向一つずつ
{
  int i;
  unsigned long time;
  //  unsigned long ontime = 0;
  char* recieve;
  //全消灯
  TurnOff();
  //LED
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,LOW);
  //一秒待機
  delay(1000);
  //る～ぷ～
  for(i=1;i<=7;i++)
  {
    // i番目をパターン点灯(01)へ
    XBee.sendRemoteATCommand(i, "D0", false, 1, 5);
    XBee.sendRemoteATCommand(i, "D1", false, 1, 4);
    //適用コマンド送信
    XBee.loopAction();
    //時間取得(i番目を点けた時間)
    time = millis();
    //    ontime = millis() - time;
    // 15秒待機(終了状態が入力されたら離脱)
    while( ((millis() - time) < waittime) && (train==0) && (game==0) )
    {
      train = digitalRead(A8);
      game = digitalRead(A9);
      //メッセージ受信チェック
      recieve = XBee.recieveRXData(i);//チェック必要
      if(recieve) //データが返されたら脱出
        break;

    }
    // i番目を消灯(00)
    XBee.sendRemoteATCommand(i, "D0", false, 1, 4);
    XBee.sendRemoteATCommand(i, "D1", false, 1, 4);
    //適用
    XBee.loopAction();
  }
  XBee.loopAction();
}

void ForwardAll()       //順方向つけていく
{
  int i;
  unsigned long time;
  char* recieve;
  //全消灯
  TurnOff();
  //LED
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,LOW);
  //一秒待機
  delay(1000);
  //る～ぷ～
  for(i=1;i<=7;i++)
  {
    // i番目をパターン点灯(01)へ(一度保留)
    XBee.sendRemoteATCommand(i, "D0", false, 1, 5);
    XBee.sendRemoteATCommand(i, "D1", false, 1, 4);
    //適用コマンド送信
    XBee.loopAction();
    //時間取得(i番目を点けた時間)
    time = millis();
    // 15秒待機(終了状態が入力されたら離脱)
    while(((millis()-time)<waittime)/*&&(train==0)&&(game==0)*/)
    {
      train = digitalRead(A8);
      game = digitalRead(A9);
      //メッセージ受信チェック
      recieve = XBee.recieveRXData(i);//チェック必要
      if(recieve) //データが返されたら脱出
        break;
    }
  }
  XBee.loopAction();
}

void ReverseSingle()    //逆方向一つずつ
{
  int i;
  unsigned long time;
  //  unsigned long ontime = 0;
  char* recieve;
  //全消灯
  TurnOff();
  //LED
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,LOW);
  //一秒待機
  delay(1000);
  //る～ぷ～
  for(i=7;i>=1;i--)
  {
    // i番目をパターン点灯(01)へ(一度保留)
    XBee.sendRemoteATCommand(i, "D0", false, 1, 5);
    XBee.sendRemoteATCommand(i, "D1", false, 1, 4);
    //適用コマンド送信
    XBee.loopAction();
    //時間取得(i番目を点けた時間)
    time = millis();
    //    ontime = millis() - time;
    // 15秒待機(終了状態が入力されたら離脱)
    while(((millis()-time)<waittime)&&(train==0)&&(game==0))
    {
      train = digitalRead(A8);
      game = digitalRead(A9);
      //メッセージ受信チェック
      recieve = XBee.recieveRXData(i);//チェック必要
      if(recieve) //データが返されたら脱出
        break;
    }
    // i番目を消灯(00)
    XBee.sendRemoteATCommand(i, "D0", false , 1, 4);
    XBee.sendRemoteATCommand(i, "D1", false , 1, 4);
    //適用
    XBee.loopAction();
  }
  XBee.loopAction();
}

void ReverseAll()       //逆方向つけていく
{
  int i;
  unsigned long time;
  char* recieve;
  //全消灯
  TurnOff();
  //LED
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,LOW);
  //一秒待機
  delay(1000);
  //る～ぷ～
  for(i=7;i>=1;i--)
  {
    // i番目をパターン点灯(01)へ(一度保留)
    XBee.sendRemoteATCommand(i, "D0", false, 1, 5);
    XBee.sendRemoteATCommand(i, "D1", false, 1, 4);
    //適用コマンド送信
    XBee.loopAction();
    //時間取得(i番目を点けた時間)
    time = millis();
    // 15秒待機(終了状態が入力されたら離脱)
    while(((millis()-time)<waittime)/*&&(train==0)&&(game==0)*/)
    {
      train = digitalRead(A8);
      game = digitalRead(A9);
      //メッセージ受信チェック
      recieve = XBee.recieveRXData(i);//チェック必要
      if(recieve) //データが返されたら脱出
        break;
    }
  }
  XBee.loopAction();
}

void RandomSingle()     //ランダム一つずつ
{
  int i,addr;
  int num = 0;  //点いた数
  unsigned long time;
  char* recieve;
  //全消灯
  TurnOff();
  //LED
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,LOW);
  //一秒待機
  delay(1000);
  //ループ
  while(1)
  {
    //乱数でアドレスを指定
    addr = random(1,7);
    //そーしん
    // addr番目をパターン点灯(01)へ(一度保留)
    XBee.sendRemoteATCommand(addr, "D0", false, 1, 5);
    XBee.sendRemoteATCommand(addr, "D1", false, 1, 4);
    //適用コマンド送信
    XBee.loopAction();
    //時間取得(addr番目を点けた時間)
    time = millis();
    // 15秒待機(終了状態が入力されたら離脱)
    while(((millis()-time)<waittime)&&(train==0)&&(game==0))
    {
      train = digitalRead(A8);
      game = digitalRead(A9);
      //メッセージ受信チェック
      recieve = XBee.recieveRXData(i);//チェック必要
      if(recieve) //データが返されたら脱出
        break;
    }
    // addr番目を消灯(00)
    XBee.sendRemoteATCommand(addr, "D0", false , 1, 4);
    XBee.sendRemoteATCommand(addr, "D1", false , 1, 4);
    //適用
    XBee.loopAction();
    //点いた数を+1
    num++;
    //10以上で脱出
    if(num >= 10)
      break;
  }
  XBee.loopAction();
}

void RandomAll()        //ランダムつけていく
{
  int i,addr;
  int num=0;  //点いてる数
  unsigned long time;
  char* recieve;
  bool flag[] = {false,false,false,false,false,false,false};
  //全消灯
  TurnOff();
  //LED
  digitalWrite(LED0,HIGH);
  digitalWrite(LED1,LOW);
  //一秒待機
  delay(1000);
  //ループ
  while(1)
  {
    //乱数でアドレスを指定
    addr = random(1,7);
    //既に点いていたらコンテニュー（もどる
    if(flag[addr]==true)
      continue;
    else
      flag[addr] = true;
      num++;      //点いた数を+1
    //そーしん
    // addr番目をパターン点灯(01)へ(一度保留)
    XBee.sendRemoteATCommand(addr, "D0", false, 1, 5);
    XBee.sendRemoteATCommand(addr, "D1", false, 1, 4);
    //適用コマンド送信
    XBee.loopAction();
    //時間取得(addr番目を点けた時間)
    time = millis();
    // 15秒待機(終了状態が入力されたら離脱)
    while(((millis()-time)<waittime)&&(train==0)&&(game==0))
    {
      train = digitalRead(A8);
      game = digitalRead(A9);
      //メッセージ受信チェック
      recieve = XBee.recieveRXData(i);//チェック必要
      if(recieve) //データが返されたら脱出
        break;
    }
    //全部ついていたら脱出
    if(num >= 6)
      break;
  }
  XBee.loopAction();
}
/*
void InputMode()
{
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
}
*/
