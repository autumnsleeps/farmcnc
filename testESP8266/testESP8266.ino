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
String dataFromMega;

void setup() {
  Serial.begin(9600);
  MySerial.begin(19200);

//  MySerial.print("*s#");  
//  while(1){                     //*s;autorace_2G;autoraceonly;abdfhdterf;1235456dff#
//    if(MySerial.available()){
//      dataPic = MySerial.readStringUntil('#');
//      if (dataPic.startsWith("*s")) break;
//      }
//    }
//  dataPic = dataPic.substring(1,dataPic.length()); //Xóa * và # ở đầu và cuối chuỗi
//  dataPic.concat(";");
//  for (int i=0; i<5; i++){
//    value[i] = dataPic.substring(0,dataPic.indexOf(";",0));
//    dataPic.remove(0,dataPic.indexOf(";",0)+1);
//    }
//  wifi[0] = value[1].c_str();         
//  wifi[1] = value[2].c_str();         
//  wifi[2] = value[3].c_str(); 
//  wifi[3] = value[4].c_str();
//  
//  wifiMulti.addAP(wifi[0], wifi[1]);
//  wifiMulti.addAP(wifi[2], wifi[3]);

  wifiMulti.addAP("autorace_2.4G","autoraceonly");
  connectWiFi();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//  Firebase.stream("/");   
  }

void loop() { 
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    connectWiFi();
  }
  else {
//      if(MySerial.available()){
//          dataFromMega = MySerial.read();
//          Serial.println(dataFromMega);
//      }
      processingData(); 
//    handleReadingGroundHumiditySensor();
  }
}
  
void processingData(){
//  String dataPic = MySerial.readStringUntil('#');
//  Serial.println(dataPic);
//  if(dataPic.startsWith("*")){
//    dataPic = dataPic.substring(1,dataPic.length()); //Xóa * và # ở đầu và cuối chuỗi
//    dataPic.concat(";");
//    String value[6];
//    for (int i=0; i<6; i++){
//      value[i] = dataPic.substring(0,dataPic.indexOf(";",0));
//      dataPic.remove(0,dataPic.indexOf(";",0)+1);
//      }
//      
//    int device = value[0].toInt();
//
//    switch(device){
//      case 1:
//        Serial.println("TB1");
//        Firebase.setString("/device_1/alert1",value[1]);
//        Firebase.setString("/device_1/alert2",value[2]);
//        Firebase.setString("/device_1/alert3",value[3]);
//        Firebase.setString("/device_1/voltage",value[4]);
//        Firebase.setString("/device_1/current",value[5]);
//        break;
//      case 2:
//        Serial.println("TB2");
//        Firebase.setString("/device_2/alert1",value[1]);
//        Firebase.setString("/device_2/alert2",value[2]);
//        Firebase.setString("/device_2/alert3",value[3]);
//        Firebase.setString("/device_2/voltage",value[4]);
//        Firebase.setString("/device_2/current",value[5]);
//        break;
//      default :
//        Serial.print("Thiet bi chua duoc khai bao");
//      }
//    }

//Define activation variables for each mode
    String automaticMode = "/allWorkingModes/automaticMode/isActivated";
    String manualMode = "/allWorkingModes/manualMode/isActivated";
    String runToAllThePointMode = "/allWorkingModes/runToAllThePoint/isActivated";
    String weedDetectionMode = "/allWorkingModes/weedDetectionMode/isActivated";
    int selectMode = 0;

    Firebase.getInt(firebaseData, automaticMode);
    int autoModeSelect = firebaseData.intData();
    Firebase.getInt(firebaseData, manualMode);
    int manualModeSelect = firebaseData.intData();
    Firebase.getInt(firebaseData, runToAllThePointMode);
    int runToAllThePoint = firebaseData.intData();
    Firebase.getInt(firebaseData, weedDetectionMode);
    int weedDetectionModeSelect = firebaseData.intData();

    //Get recognition activation data
    Firebase.getInt(firebaseData, "/allWorkingModes/weedDetectionMode/isDetected");
    int isDetected = firebaseData.intData();
    
//    Serial.println(autoModeSelect);
//    Serial.println(manualModeSelect);
//    Serial.println(xyCoordsModeSelect);
//    Serial.println(weedDetectionModeSelect);
//    delay(1500);
    if(autoModeSelect == 1 && manualModeSelect == 0 && runToAllThePoint == 0 && weedDetectionModeSelect == 0 && isDetected == 0){
        selectMode = 1;
    }
    else if(autoModeSelect == 0 && manualModeSelect == 0 && runToAllThePoint == 1 && weedDetectionModeSelect == 0 && isDetected == 0){
        selectMode = 2;
    }
    else if(autoModeSelect == 0 && manualModeSelect == 1 && runToAllThePoint == 0 && weedDetectionModeSelect == 0 && isDetected == 0){
        selectMode = 3;
    }
    else if(autoModeSelect == 0 && manualModeSelect == 0 && runToAllThePoint == 0 && weedDetectionModeSelect == 1 && isDetected == 0){
        selectMode = 4;
    }else if(autoModeSelect == 0 && manualModeSelect == 0 && runToAllThePoint == 0 && weedDetectionModeSelect == 0 && isDetected == 1){
        selectMode = 5;  
    }
    
//    Serial.println(selectMode);
    switch(selectMode){
//        case 0:{ 
//            MySerial.println("Nothing happen");
//            }
//            break;
        case 1:{ 
//            Firebase.getInt(firebaseData,"/allWorkingModes/automaticMode/robotArm");
//            int robotArmValue = firebaseData.intData();
            //Get air temperature, air humidity and ground humidity from Firebase
            Firebase.getFloat(firebaseData, "/allSensorsData/farmHumidity");
            float groundHumidity = firebaseData.floatData();
            Firebase.getFloat(firebaseData, "/allSensorsData/atmosphereHumidity");
            float airHumidity = firebaseData.floatData();
            Firebase.getFloat(firebaseData, "/allSensorsData/atmosphereTemperature");
            float airTemperature = firebaseData.floatData();

            //Get the user's parameter settings
            Firebase.getFloat(firebaseData, "/userSetting/airTemp");
            float userTempSetting = firebaseData.floatData();
            Firebase.getFloat(firebaseData, "/userSetting/airHumid");
            float userHumidSetting = firebaseData.floatData();
            Firebase.getFloat(firebaseData, "/userSetting/groundHumid");
            float userGroundHumidSetting = firebaseData.floatData();


            //Get point index of small plants and big plants
            Firebase.getString(firebaseData, "/userSetting/smallPlant");
            String smallPlant = firebaseData.stringData();
            Firebase.getString(firebaseData, "/userSetting/bigPlant");
            String bigPlant = firebaseData.stringData();

            //101 = misting mode, 102 = watering mode

            Serial.println("The air condition isn't very good. Let's do the misting mode");
            MySerial.print("*101#");
            
//            Serial.println("Entering the watering mode...");
//            Serial.println("------------------------------");
//            delay(5000);
//            
            smallPlant.concat(";");
            int indexNumSmallPlantList = 0;
            for(int i = 0; i < smallPlant.length(); i++){
                if(smallPlant.charAt(i) == ';'){
                    indexNumSmallPlantList++;  
                }  
            }

            String smallPlantList[indexNumSmallPlantList];
            for(int i=0; i < indexNumSmallPlantList; i++){
                smallPlantList[i] = smallPlant.substring(0,smallPlant.indexOf(";",0));
                smallPlant.remove(0,smallPlant.indexOf(";",0)+1);
//                      Serial.println(smallPlantList[i]);
            }

            Serial.println("***********************************");

            bigPlant.concat(";");
            int indexNumBigPlantList = 0;
            for(int i = 0; i < bigPlant.length(); i++){
                if(bigPlant.charAt(i) == ';'){
                    indexNumBigPlantList++;  
                }  
            }

            String bigPlantList[indexNumBigPlantList];
            for(int i=0; i < indexNumBigPlantList; i++){
                bigPlantList[i] = bigPlant.substring(0,bigPlant.indexOf(";",0));
                bigPlant.remove(0,bigPlant.indexOf(";",0)+1);
//                      Serial.println(bigPlantList[i]);
            }

            String stringToSend = "";
            //Cay nho: 1, Cay lon: 2
            for(int i = 0; i < 42; i++){
                int plantIndex = i;
              
                for(int val = 0; val < indexNumSmallPlantList; val++){
                    if(plantIndex == smallPlantList[val].toInt()){
                        stringToSend = "*1;" + String(plantIndex) + ";1#";
                        MySerial.print(stringToSend);
//                        MySerial.print("*4#");
                    }
                }

                for(int val = 0; val < indexNumBigPlantList; val++){
                    if(plantIndex == bigPlantList[val].toInt()){
                        stringToSend = "*1;" + String(plantIndex) + ";2#";
                        MySerial.print(stringToSend);
//                        MySerial.print("*5#");
                    }
                }
            }
            
            //Turn off the automatic mode to demo
            Firebase.setInt(firebaseData, "/allWorkingModes/automaticMode/isActivated", 0);

            //Homing
            MySerial.print("*0#");
        }
            break;
        case 2:{
            Firebase.getString(firebaseData, "/allWorkingModes/runToAllThePoint/strToProcess");
            String strToProcess = firebaseData.stringData();
            String processedString = "";

            MySerial.print("*4#");

            strToProcess.concat(";");
            int numbersOfCommands = 0;
            for(int i = 0; i < strToProcess.length(); i++){
              if(strToProcess.charAt(i) == ';'){
                numbersOfCommands++;  
              }
            }

            String commandList[numbersOfCommands];
            for(int i = 0; i < numbersOfCommands; i++){
              commandList[i] = strToProcess.substring(0, strToProcess.indexOf(";", 0));
              strToProcess.remove(0, strToProcess.indexOf(";",0)+1);  
            }

            for(int i = 0; i < 36; i++){
                int positionIndex = i;

                for(int j = 0; j < numbersOfCommands; j++){
                    if(positionIndex == commandList[j].toInt()){
                        processedString = "*2;" + String(positionIndex) + "#";
                        MySerial.print(processedString);
                    }
                }
            }

//          Turn off the manual mode
            Firebase.setInt(firebaseData,"/allWorkingModes/runToAllThePoint/isActivated", 0);

            MySerial.print("*0#");
        }
            break;
        case 3:{
            //Move the Z axis down below
            MySerial.print("*4#");
          
            Firebase.getString(firebaseData,"/allWorkingModes/manualMode/posNum");
            String posNumValue = firebaseData.stringData();
            String strToSend = "";

            posNumValue.concat(";");
            int manualModeCommandList = 0;
            for(int i = 0; i < posNumValue.length(); i++){
                if(posNumValue.charAt(i) == ';'){
                    manualModeCommandList++;  
                }  
            }

            String manualModeCommand[manualModeCommandList];
            for(int i = 0; i < manualModeCommandList; i++){
                manualModeCommand[i] = posNumValue.substring(0,posNumValue.indexOf(";",0));
                posNumValue.remove(0,posNumValue.indexOf(";",0)+1);
//                Serial.println(manualModeCommand[i]);
            }

            for(int i = 0; i < 36; i++){
                int plant = i;

                for(int j = 0; j < manualModeCommandList; j++){
                    if(plant == manualModeCommand[j].toInt()){
                        strToSend = "*2;" + String(plant) + "#";
                        MySerial.print(strToSend);
                    }  
                }
            }

//          Turn off the manual mode
            Firebase.setInt(firebaseData,"/allWorkingModes/manualMode/isActivated", 0);

            MySerial.print("*0#");
            }
            break;
        case 4:{
              //kich thuoc anh 1280x720, X: 1280px ; Y: 720px
               MySerial.print("*2;0;0;asd;asd#");

               //Get image processed string
               Firebase.getString(firebaseData, "/allWorkingModes/weedDetectionMode/Direction");
               String temp = firebaseData.stringData();

               String execStr = "@" + temp + "#";
               
//               Serial.print(execStr);
               MySerial.print(execStr);

               //Turn off the image processing mode
               Firebase.setInt(firebaseData, "/allWorkingModes/weedDetectionMode/isActivated", 0);
            }
            break;
        case 5: {
               //Start the process by making sure that the machine is homed
               MySerial.print("*0#");

               for(int i = 0; i < 4; i++){
                  //Move to photo shoot points
                  String stringSend = "*5;" + String(i) + "#";
                  MySerial.print(stringSend);
              
                  //Activate the camera
                  Firebase.setInt(firebaseData, "/allWorkingModes/weedDetectionMode/cameraActivated", 1);
               }

               //Turn off the recognition mode
               Firebase.setInt(firebaseData, "/allWorkingModes/weedDetectionMode/isDetected", 0);

               //Homing the machine
               MySerial.print("*0#");
            }
            break;
    }   
} 

void connectWiFi(){
  Serial.println("Connecting Wifi...");
  for(int k=0;k<30;k++){
//    if (wifiMulti.run() == WL_CONNECTED){
//      Serial.println("Wifi is connected!!!");
//      break;
//      }
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

//int handleReadingGroundHumiditySensor(){
////    int sensorADCVal = analogRead(A0);
////    Serial.println("The ADC value of ground humidity sensor is: ");
////    Serial.println(sensorADCVal);
////    delay(5000); 
//
//      int sensorADCValSum = 0;
//      for(int i = 0; i < 10; i++){
//          int sensorADC = analogRead(A0);
//          sensorADCValSum += sensorADC;
//          delay(30);  
//      }
//
//      int resultAverage = sensorADCValSum/10;
//      return(resultAverage);
//}
  
//void firebaseStream(){
//  if (Firebase.available()) {
//    FirebaseObject event = Firebase.readEvent();
//    String eventType = event.getString("type");
//    eventType.toLowerCase();
//   
//    if (eventType == "put") {
//      Serial.print("data: ");
//      Serial.println(event.getString("data"));
//      }
//    }
//  }
