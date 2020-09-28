//#include <FirebaseArduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>

SoftwareSerial MySerial(4, 5); //RX,TX = D1, D2, common GND
ESP8266WiFiMulti wifiMulti;
FirebaseData firebaseData;

#define FIREBASE_HOST "farmbotcnc.firebaseio.com"
#define FIREBASE_AUTH "qMYQ1i0drocYWOxBFXf66d7BHyDwT5FtzRfnWNqR"

bool connectioWasAlive = true;
//String data = "*s;autorace_2G;autoraceonly;abcdefgh;123456789#";
const char *wifi[4];
String dataPic;
String value[5];
String stringFromMega;

void setup(){
  Serial.begin(9600);
  MySerial.begin(19200);
  wifiMulti.addAP("autorace_2.4G","autoraceonly");
  connectWiFi();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void processingData(){
  //Get important variables from Firebase
    String cameraActivatedURL = "/allWorkingModes/weedDetectionMode/cameraActivated";
    String isFinishedURL = "/allWorkingModes/weedDetectionMode/isFinished";
    String isDetectedURL = "/allWorkingModes/weedDetectionMode/isDetected";
    String isActivatedURL = "/allWorkingModes/weedDetectionMode/isActivated";
    int modeSelect = 0;

    Firebase.getInt(firebaseData, isDetectedURL);
    int isDetected = firebaseData.intData();

    Firebase.getInt(firebaseData, isActivatedURL);
    int isActivated = firebaseData.intData();
    
    Firebase.getInt(firebaseData, cameraActivatedURL);
    int cameraActivated = firebaseData.intData();
    
    Firebase.getInt(firebaseData, isFinishedURL);
    int isFinished = firebaseData.intData();

    if(isDetected == 1 && isActivated == 0){
      modeSelect = 1;
    }else if(isActivated == 1 && isDetected == 0){
      modeSelect = 2;
    }else {
      modeSelect = 0;  
    }

    switch(modeSelect){
      case 1: 
        Serial.println("This is the detection mode");
        MySerial.print("*5;1");
        Firebase.setInt(firebaseData, isDetectedURL, 0);
        break;
      case 2: 
        Serial.println("This is the working mode");
        Firebase.setInt(firebaseData, isActivatedURL, 0);
        break;
    }  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(MySerial.available()){
       stringFromMega = MySerial.read();
       Serial.println(stringFromMega);
    }

   processingData();
}

void connectWiFi(){
  Serial.println("Connecting Wifi...");
  for(int k=0;k<30;k++){
     if (wifiMulti.run() != WL_CONNECTED){
      if (connectioWasAlive == true){
        connectioWasAlive = false;
        Serial.print("Looking for WiFi ");
        }
        Serial.print(".");
        delay(500);
        }   
    else if (connectioWasAlive == false){   
      connectioWasAlive = true;
      Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
      if (WiFi.status() == WL_CONNECTED){ 
        Serial.println("");
        Serial.println("WiFi connected");
        WiFi.softAPdisconnect(true);       
        }
      else {
        Serial.println("lỗi kết nối");
        delay(1000);
        ESP.reset();
        delay(2000);
        }
      } 
    }
  }
