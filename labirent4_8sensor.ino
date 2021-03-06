#include <QTRSensors.h>

#define Kp 0.6 // Kendi deneyimlerinizle bulmanız gerekli küçük bir değer ile başlayıp, büyütebilirsiniz en kararlı Kp değerini bulana kadar.. 0.4
#define Kd 2.5 // Bunu da küçük bir değerden başlatın ve deneyimleyerek büyütün. ( Not: Kp < Kd) 2.0
#define rightMaxSpeed 80 //105
#define leftMaxSpeed 80
#define rightBaseSpeed 65 // 90 robot için kp ve kd değerlerini tutturduysanız şayet motorların dönmesi gereken hız budur
#define leftBaseSpeed 65 // yukarıdaki değer ile aynıdır

#define leftFarSensor A7
#define leftOuterSensor A6
#define leftNearSensor A5
#define leftCenterSensor A4
#define rightCenterSensor A3
#define rightNearSensor A2
#define rightOuterSensor A1
#define rightFarSensor A0
int leftCenterReading;
int leftNearReading;
int leftOuterReading;
int leftFarReading;
int rightCenterReading;
int rightNearReading;
int rightOuterReading;
int rightFarReading;
int leftNudge;
int replaystage;
int rightNudge;
#define leapTime 400
#define leftMotor1 5
#define leftMotor2 6
#define rightMotor1 11
#define rightMotor2 12
#define leftMotorE 3
#define rightMotorE 9

int lastError = 0;

#define led 13
char path[30] = {};
int pathLength;
int readLength;
int leftspeed;
int rightspeed;
int donushizi=90;//110

QTRSensors qtr;

const uint8_t SensorCount = 8;
unsigned int sensorValues[SensorCount];
unsigned int posit;

void setup() {

   // sensör ayarı
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);
  pinMode(leftCenterSensor, INPUT);
  pinMode(leftNearSensor, INPUT);
  pinMode(leftOuterSensor, INPUT);
  pinMode(leftFarSensor, INPUT);
  pinMode(rightCenterSensor, INPUT);
  pinMode(rightNearSensor, INPUT);
  pinMode(rightOuterSensor, INPUT);
  pinMode(rightFarSensor, INPUT);
  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);


  replaystage = 0;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // arduinonun ledi yanıyorsa kalibrasyon modundayız demektir


  int i;
  for (int i = 0; i < 100; i++) { // tercihe bağlı alandır ya robotunuzu hatta yürüterek kalibre edin veya bunu otomatik yapın.
    //otomatik kalibrasyon için burasının yorumunu kaldırın
    if ( i  < 25 || i >= 75 ) {// sensörleri, karşılaşılabilecek en aydınlık ve en karanlık okumalara maruz bırakmak için sola ve sağa çevirin.
      turn_right();
    } //bu fonksiyonları yazacaz
    else {
      turn_left();
    }
    qtr.calibrate();
    delay(20);
  }
  wait(); //motorları durdur
  digitalWrite(LED_BUILTIN, LOW); // kalibrasyon bitince ledi söndür.

  delay(3000); // Ana döngüye girmeden önce botu konumlandırmak için 3 saniye bekleyin


}
void loop() {
  readSensors();
  while ((leftFarReading < 200 && rightFarReading < 200 ) && (leftCenterReading > 200 || rightCenterReading > 200 )) {
    ilerle();
    delay(6);
    Serial.println("ilerliyor");
    readSensors();
  }

  
    solelkurali();
    Serial.println("Lefthand başladı");
  
}
void readSensors() {
  leftCenterReading = analogRead(leftCenterSensor);
  leftNearReading = analogRead(leftNearSensor);
  leftOuterReading = analogRead(leftOuterSensor);
  leftFarReading = analogRead(leftFarSensor);
  rightCenterReading = analogRead(rightCenterSensor);
  rightNearReading = analogRead(rightNearSensor);
  rightOuterReading = analogRead(rightOuterSensor);
  rightFarReading = analogRead(rightFarSensor);
   //serial printing below for debugging purposes
//   Serial.print("leftCenterReading: ");
//   Serial.println(leftCenterReading);
//   Serial.print("leftNearReading: ");
//   Serial.println(leftNearReading);
//   Serial.print("leftFarReading: ");
//   Serial.println(leftFarReading);
//   Serial.print("rightCenterReading: ");
//   Serial.println(rightCenterReading);
//   Serial.print("rightNearReading: ");
//   Serial.println(rightNearReading);
//   Serial.print("rightFarReading: ");
//   Serial.println(rightFarReading);
//   delay(200);
}
void solelkurali() {
  if (leftFarReading < 200 && rightFarReading < 200 && leftOuterReading < 200 && rightOuterReading < 200 && leftNearReading < 200 && rightNearReading < 200 && leftCenterReading < 200 && rightCenterReading < 200) {
    turnAround();
    Serial.println("geri dönüyor");
    return;
  }
  ///////////////////////////////////////////////
 
  //////////////////////////////////////////////////
    //kavşağa geldiysek.Sağ dışlar da Sol dışlar da çizgi görüyor.
    if ((leftNearReading > 200 && rightNearReading > 200) || (leftOuterReading > 200 && rightOuterReading > 200) ) {
      //hafif ileri gidiyor ki bakıyor çizginin devam karakteri nasıl? Leap time kadar gidiyor.
      Serial.println("orta kavşak algıladı");
      digitalWrite(leftMotor1,HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
  analogWrite(rightMotorE, rightBaseSpeed);
  analogWrite(leftMotorE, leftBaseSpeed);
      delay(leapTime);
      //sonra çizgiyi geçince durup tekrar sensör okuyor ve bakıyor.
      readSensors();
      if (leftFarReading > 800 && rightFarReading > 800 && leftOuterReading > 800 && rightOuterReading > 800) { //ileri gitmesine rağmen hala çizgi varsa o zman bitmiş demektir.
        done(); // sensörler siyahsa bittiğini anlayacak
        
      }
      //eğer sağda ve solda da çizgi yoksa bir kavşağı geçtik demektir o zaman kural gereği sola dönecek
      if (leftFarReading < 300 && rightFarReading < 300) {
        Serial.println("kavşaktan sola dönüyor");
        turnLeft();
      }
    return;
    }
    ///////////////////////////////////////////////////////////////////////////////
 //sola yarım artı -| , sola dirsek veya kavşak şeklinde bir yola geldiyse
  if (leftFarReading > 300 && rightFarReading < 300) { // soladönüş var
    Serial.println("sola dönüş algıladı");
    digitalWrite(leftMotor1,HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
  analogWrite(rightMotorE, rightBaseSpeed);
  analogWrite(leftMotorE, leftBaseSpeed);
    delay(leapTime);
    readSensors();
    if (leftFarReading < 300 && leftOuterReading < 300) { //maze bitmemişse onun dışındaki durumlarda zaten sola dönecek
      Serial.println("sola dönüyor");
      turnLeft();
    }
    else if (leftFarReading > 500 && rightFarReading > 500 && leftOuterReading > 500 && rightOuterReading > 500) { //ileri gitmesine rağmen hala çizgi varsa o zman bitmiş demektir.
      Serial.println("bitiş çizgisi");
      done(); //tüm sensörler siyahsa bittiğini anlayacak

    }
    return;

  }
  /////////////////////////////////////////////////////////////////
    
  /////////////////////////////////////////////////////////////////////////////////
  //sağa yarım artı |-  veya sağa dirsek şeklinde bir yola geldiyse
  if ((rightFarReading > 200 && rightOuterReading > 200) && (leftFarReading < 400 && leftOuterReading < 400)) { // sağadönüşlü yol var
    Serial.println("sağa dönüş algıladı");
    digitalWrite(leftMotor1,HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
  analogWrite(rightMotorE, rightBaseSpeed);
  analogWrite(leftMotorE, leftBaseSpeed);
    delay(leapTime);
    readSensors();
    if (leftFarReading > 500 && rightFarReading > 500 && leftOuterReading > 500 && rightOuterReading > 500 ) { //ileri gitmesine rağmen hala çizgi varsa o zman bitmiş demektir.
      done(); //tüm sensörler siyahsa bittiğini anlayacak
    }
    else if (leftCenterReading > 200 || rightCenterReading > 200) { //ortada yol varsa kural gereği sağa girmeyecek ortadan gidecek
      
      if (replaystage == 0) {
        path[pathLength] = 'S';
        Serial.println("s");
        pathLength++;
         Serial.print("Path length: ");
        Serial.println(pathLength);
        if (path[pathLength - 2] == 'B') {
        Serial.println("shortening path");
          shortPath();
        }
      }
      Serial.println("ortadan gidiyor");
      ilerle();
      return;

    }
    else  { //ortada yol yoksa sağa dirsek geçilmiştir. sağa dönecek
      turnRight();
      Serial.println("sağa dönüyor");
      return;
    }

  }//rightturn
  /////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////


  //    path[pathLength] = 'S';
  //    // Serial.println("s");
  //    pathLength++;
  //    //Serial.print("Path length: ");
  //    //Serial.println(pathLength);
  //    if (path[pathLength - 2] == 'B') {
  //      //Serial.println("shortening path");
  //      shortPath();
  //    }


}//void solel

void turn_left() {
      digitalWrite(rightMotor1,HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite (rightMotorE,90); 

    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    analogWrite (leftMotorE,90);
}
void turn_right() {
      digitalWrite(rightMotor1,LOW);
    digitalWrite(rightMotor2, HIGH);
    analogWrite (rightMotorE,90);   

    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    analogWrite (leftMotorE,90);    
}
void done() {
  Serial.print("Labirenti bitirdi ");
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  replaystage = 1;
  path[pathLength] = 'D';
  pathLength++;
  readSensors();
  while (leftFarReading > 300 && rightFarReading > 300) { //yerinden kaldırana kadar replay bekliyor
    digitalWrite(led, LOW);
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    readSensors();
  }
  delay(2000);
  readLength = 0;
  replay();
}
void turnLeft() {
  
    
  while (analogRead(leftCenterSensor) > 200 || analogRead(rightCenterSensor) > 200 ) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);
    delay(5);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(2);
  }
  while (analogRead(leftCenterSensor) < 200) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);    
    delay(4);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
  while (analogRead(rightCenterSensor) < 800) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);
    delay(4);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
  
  if (replaystage == 0) {
    path[pathLength] = 'L'; //diziye hareketi kaydediyoruz.
    Serial.println("L");
    pathLength++; //diziuzunluğu değişkenini bir artırıyoruz. Böylece hem lattaki if düzgün çalışacak hem de bir sonraki hamle doğru yere yazılacak
    Serial.print("Path length: ");
    Serial.println(pathLength);
    if (path[pathLength - 2] == 'B') { //dizide az önce yazdığımız hameleden bir önceki hamle B mi diye bakıyor B ise kısaltmayı çalıştıracak
      Serial.println("shortening path");
      shortPath();
    }
  }
}//voidturnleft
void turnRight() {
    
  while (analogRead(leftCenterSensor) > 200 || analogRead(rightCenterSensor) > 200 ) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);
    delay(5);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(2);
  }
 
  while (analogRead(rightCenterSensor) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);    
    delay(4);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
  while (analogRead(leftCenterSensor) < 800) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);    
    delay(4);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }  
    
  if (replaystage == 0) {
    path[pathLength] = 'R';
    Serial.println("r");
    pathLength++;
     Serial.print("Path length: ");
    Serial.println(pathLength);
    if (path[pathLength - 2] == 'B') {
      Serial.println("shortening path");
      shortPath();
    }
  }
} //voidturnright
void ilerle() {

 // çizginin tam ortasını bulmaya çalışacak
  // 0 ile 7000 arasındaki skalada aslında amacımız 3500 değerini tutmak olmalı ki tam ortadan gitsin.
  

  posit = qtr.readLineBlack(sensorValues);
  int error = posit - 3500;
  int motorSpeed = Kp * error + Kd * (error - lastError);
  lastError = error;
  int rightMotorSpeed = rightBaseSpeed + motorSpeed;
  int leftMotorSpeed = leftBaseSpeed - motorSpeed;
  if (rightMotorSpeed > rightMaxSpeed ) rightMotorSpeed = rightMaxSpeed;
  if (leftMotorSpeed > leftMaxSpeed ) leftMotorSpeed = leftMaxSpeed;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;
  

    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorE, rightMotorSpeed);

    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    analogWrite(leftMotorE, leftMotorSpeed);
}//voidilerle


void turnAround() {
  digitalWrite(leftMotor1,HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  analogWrite(rightMotorE, rightBaseSpeed);
  analogWrite(leftMotorE, leftBaseSpeed);
  delay(leapTime);
  while (analogRead(rightCenterSensor) < 300) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorE, donushizi);
    analogWrite(leftMotorE, donushizi);
    delay(5);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(2);
  }
  if (replaystage == 0) {
    path[pathLength] = 'B';
    Serial.println("b");
    pathLength++;
    Serial.print("Path length: ");
    Serial.println(pathLength);
    if (path[pathLength - 2] == 'B') {
    Serial.println("shortening path");
      shortPath();
    }
  }
  
}
void shortPath() {
  int shortDone = 0;
  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'R') {
    pathLength -= 3;
    path[pathLength] = 'B';
    //Serial.println("test1");
    shortDone = 1;
  }
  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'S' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'R';
    //Serial.println("test2");
    shortDone = 1;
  }
  if (path[pathLength - 3] == 'R' && path[pathLength - 1] == 'L' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'B';
    //Serial.println("test3");
    shortDone = 1;
  }
  if (path[pathLength - 3] == 'S' && path[pathLength - 1] == 'L' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'R';
    //Serial.println("test4");
    shortDone = 1;
  }
  if (path[pathLength - 3] == 'S' && path[pathLength - 1] == 'S' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'B';
    //Serial.println("test5");
    shortDone = 1;
  }
  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'L' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'S';
    //Serial.println("test6");
    shortDone = 1;
  }
  path[pathLength + 1] = 'D';
  path[pathLength + 2] = 'D';
  pathLength++;
  Serial.print("Path length: ");
  Serial.println(pathLength);
  printPath();
}
void printPath() {
  Serial.println("+++++++++++++++++");
  int x;
  while (x <= pathLength) {
    Serial.println(path[x]);
    x++;
  }
  Serial.println("+++++++++++++++++");
}
void replay() {
  readSensors();
  
  if ((leftFarReading < 300 && rightFarReading < 300 ) && (leftCenterReading > 300 || rightCenterReading > 300 )) {
    ilerle();
  }
  else {
    if (path[readLength] == 'D') {
      digitalWrite(leftMotor1,HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      analogWrite(rightMotorE, rightBaseSpeed);
      analogWrite(leftMotorE, leftBaseSpeed);
      delay(60);
      digitalWrite(leftMotor1, LOW);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, LOW);
      digitalWrite(rightMotor2, LOW);
      endMotion();
    }
    if (path[readLength] == 'L') {
      digitalWrite(leftMotor1,HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      analogWrite(rightMotorE, rightBaseSpeed);
    analogWrite(leftMotorE, leftBaseSpeed);
      delay(leapTime);
      turnLeft();
    }
    if (path[readLength] == 'R') {
      digitalWrite(leftMotor1,HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      analogWrite(rightMotorE, rightBaseSpeed);
      analogWrite(leftMotorE, leftBaseSpeed);
      delay(leapTime);
      turnRight();
    }
    if (path[readLength] == 'S') {
      digitalWrite(leftMotor1,HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      analogWrite(rightMotorE, rightBaseSpeed);
      analogWrite(leftMotorE, leftBaseSpeed);
      delay(leapTime);
      ilerle();
    }
    readLength++;
  }
  replay();
}
void endMotion() {
    //printPath();
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(500);

  endMotion();
}
void fren() {
    digitalWrite(rightMotor1,HIGH);
    digitalWrite(rightMotor2, HIGH);
   

    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, HIGH);
    
}
void wait() {
    digitalWrite(rightMotor1,LOW);
    digitalWrite(rightMotor2, LOW);
   

    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    
}
