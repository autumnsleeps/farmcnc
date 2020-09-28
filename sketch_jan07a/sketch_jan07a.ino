#include <AccelStepper.h>
#include <MultiStepper.h>
//AccelStepper stepper(MotorInterface, StepPin, DirPin)
AccelStepper stepperZ(1,8,13);
//AccelStepper stepperZ(1,9,8);
AccelStepper stepperY(1,5,4);
AccelStepper stepperX(1,2,3);

MultiStepper steppers;

#define revBtnX 7
#define revBtnY 6
#define revBtnZ 11
#define waterPump 53

long initStepX = 1; //old homing process
long initStepY = 1; //old homing process
long initY = 1;
long initX = 1;
long initZ = 1;
int moveFinished = 0;
int modeNumber;
byte enableMode2 = 0;
byte enableMode3 = 0;
String workingModeAndData;
int xTempMode2 = 0;
int yTempMode2 = 0;
int xTempMode3 = 0;
int yTempMode3 = 0;
byte smallCabbage = 0;
byte bigCabbage = 0;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial1.begin(19200);
//    Serial1.begin(9600);
//    MySerial.begin(115200);
    pinMode(revBtnX, INPUT_PULLUP);
    pinMode(revBtnY, INPUT_PULLUP);
    pinMode(revBtnZ, INPUT_PULLUP);
    pinMode(waterPump, OUTPUT);
    digitalWrite(waterPump, LOW);
    stepperY.setSpeed(800); 
    stepperY.setAcceleration(300);
    stepperX.setSpeed(800); 
    stepperX.setAcceleration(300);
    stepperZ.setSpeed(700);
    stepperZ.setAcceleration(150);

    //First time turning on the machine, we should do the homing process firstly
    quickHomingProcess();
    Serial.println("--------------------------------------------------------------------------");
    Serial.println("The machine can work properly now. Enter your mode and working properties:");
    
    /*
    Xac dinh cac mode lam viec cua may:
    - Mode 1: Automatic mode. Chay auto tat ca cac diem va quay ve (0,0).
    - Mode 2: Di chuyen den toa do X, Y va dung o do, doi lenh tiep theo.
    - Mode 3: Di chuyen theo so thu tu diem tu 0 --> 41 va dung tai diem do, doi lenh tiep theo
    - Mode 4: Di chuyen theo du lieu truyen tu Raspberry Pi qua Arduino theo giao thuc UART
    */

    /*
    Thu tu cac tham so trong cau lenh serial monitor:
    - *mode hoat dong;(toa do x, toa do y)(neu co);# 
    */
    /*Example: *2;450;700#    */  
    while(1){
        if(Serial1.available()){
              workingModeAndData = Serial1.readStringUntil('#');
              Serial.println(workingModeAndData);

             if(workingModeAndData.startsWith("*")){
                workingModeAndData = workingModeAndData.substring(1,workingModeAndData.length()); //Xóa * và # ở đầu và cuối chuỗi
                workingModeAndData.concat(";");
                String value[5]; 
                
                for (int i=0; i<5; i++){
                    value[i] = workingModeAndData.substring(0,workingModeAndData.indexOf(";",0));
                    workingModeAndData.remove(0,workingModeAndData.indexOf(";",0)+1);
                }

                int modeValue = value[0].toInt();
                int xValue = value[1].toInt();
                int yValue = value[2].toInt();
                int pointNumber = value[3].toInt();
                int robotArmValue = value[4].toInt();
                switch(modeValue){
                    case 0: {
                        quickHomingProcess();
                        xTempMode2 = 0;
                        yTempMode2 = 0;
                        xTempMode3 = 0;
                        yTempMode3 = 0;
                      }
                      break;
                    case 1:{
//                          if(enableMode2 == 1){
//                              quickHomingProcess();
//                              enableMode2 = 0;
//                              xTempMode2 = 0;
//                              yTempMode2 = 0;  
//                          }
                          Serial.println("This is mode 3");
                          delay(50);
                          Serial.println("Dang di chuyen den diem" );
                          Serial.print(xValue);
                          enableMode3 = 1;
                          moveByPointNumber(xValue);
//                          if(yValue == 1){
//                              stepperZ.move(500);
//                              stepperZ.runToPosition();
//                              for(int i = 0; i < 3; i++){
//                                  digitalWrite(waterPump, LOW);
//                                  delay(300);
//                                  digitalWrite(waterPump, HIGH);
//                                  delay(300);  
//                              }
//                              stepperZ.move(-500);
//                              stepperZ.runToPosition();  
//                          }else if(yValue == 2){
//                              stepperZ.move(500);
//                              stepperZ.runToPosition();
//                              for(int i = 0; i < 5; i++){
//                                  digitalWrite(waterPump, LOW);
//                                  delay(300);
//                                  digitalWrite(waterPump, HIGH);
//                                  delay(300);  
//                              }
//                              stepperZ.move(-500);
//                              stepperZ.runToPosition();
//                          }
                          int ADCsum = 0;
                          stepperZ.move(550);
                          stepperZ.runToPosition();
                          for(int i = 0; i < 10; i++){
                             int sensorADC = analogRead(A0);
                             ADCsum += sensorADC;
                             delay(200);
                          }
                          ADCsum = ADCsum/10;
                          int percentage = map(ADCsum, 565, 120, 0, 100);
                          Serial.println(percentage);
                          if(yValue == 1 && percentage < 80){
                                digitalWrite(waterPump, HIGH);
                                delay(1000);
                                digitalWrite(waterPump, LOW);
                          }else if(yValue == 2 && percentage < 80){
                                digitalWrite(waterPump, HIGH);
                                delay(3000);
                                digitalWrite(waterPump, LOW);
                          }
                          stepperZ.move(-550);
                          stepperZ.runToPosition();
                        }
                        break;
                    case 2:{
                          Serial.println("Dang di chuyen den diem" );
                          Serial.print(xValue);
                          enableMode3 = 1;
                          moveByPointNumber(xValue);
                          //Watering
                          digitalWrite(waterPump, HIGH);
                          delay(2000);
                          digitalWrite(waterPump, LOW);    
                        }
                        break;
                    case 101:{
                          quickHomingProcess();
                          mistingMode();
                          Serial.println("Doing the misting mode!");
                        }
                        break;
                    //Move the Z axis down below
                    case 4:{
                           stepperZ.setCurrentPosition(0);
                           stepperZ.setSpeed(700);
                           stepperZ.setAcceleration(150);
                           stepperZ.move(2000);
                           stepperZ.runToPosition();                      
                        }
                        break;
                    case 5: {
                          //Recognition Mode
                          //String: *5;partNum#
                          if(xValue == 1){
                            Serial1.print("*done1#");
                          }

                          Serial.println("done sending string to nodemcu");
                     }
                        break;                  
                    default:
                        Serial.println("Please enter a valid command!");  
                }
             }else{
                 Serial.println("The program has been reached to here!");
                 String multiCommand = workingModeAndData.substring(1,workingModeAndData.length());
//                 Serial.println(multiCommand);

                 int commandNum = 0;
                 for(int i = 0; i < multiCommand.length(); i++){
                     if(multiCommand.charAt(i) == ';'){
                        commandNum++;
                     }
                 }
                 Serial.println(commandNum);

                 enableMode2 = 0;
                 enableMode3 = 0;
                 xTempMode2 = 0;
                 yTempMode2 = 0;
                 xTempMode3 = 0;
                 yTempMode3 = 0;

                 String commandList[commandNum];
                 for(int i = 0; i < commandNum; i++){
                     commandList[i] = multiCommand.substring(0,multiCommand.indexOf(";",0));
                     multiCommand.remove(0,multiCommand.indexOf(";",0)+1);
//                     Serial.println(commandList[i]);
                     String value = commandList[i];
                     String valueList[3];
                     for(int j=0; j < 3; j++){
                         valueList[j] = value.substring(0,value.indexOf(":",0));
                         value.remove(0,value.indexOf(":",0)+1); 
                     }

                     int modeVal = valueList[0].toInt();
                     int plantNumIndex = valueList[1].toInt();
                     int plantType = valueList[2].toInt();

                     if(modeVal == 102){
                         //Automatic mode
                         //Move to the plant fixed point
                         enableMode3 = 1;
                         moveByPointNumber(plantNumIndex);

                         //Check if the plant at this point is a big plant or a small plant
                         //Small plant: 1; Big plant: 2
                         if(plantType == 1){
                            Serial.println("Small plant in point");
                            Serial.println(plantNumIndex);
                            
                            stepperZ.move(500);
                            stepperZ.runToPosition();
                            delay(50);
                            stepperZ.move(-500);
                            stepperZ.runToPosition();   
                         }else if(plantType == 2){
                            Serial.println("Big plant in point");
                            Serial.println(plantNumIndex);
                            
                            stepperZ.move(500);
                            stepperZ.runToPosition();
                            delay(50);
                            stepperZ.move(-500);
                            stepperZ.runToPosition();
                         }
                     }else{
                         //Image processing mode: **1: Big plant; 0: Small plant; 2: Weed**
                         if(modeVal == 1){
                              int xValue = -(plantNumIndex*4);
                              int yValue = -((480 - plantType)*(2260/480));
                              enableMode2 = 1;
                              moveByXYCoordinates(xValue, yValue);
                              Serial.println("Water the big plant here!"); 
                         }else if(modeVal == 0){
                              int xValue = -(plantNumIndex*4);
                              int yValue = -((480 - plantType)*(2260/480));
                              enableMode2 = 1;
                              moveByXYCoordinates(xValue, yValue);
                              Serial.println("Water the small plant here!");
                         }
                     }
                 }

                 //Homing
                 quickHomingProcess();
             }
        }
    }
}

void quickHomingProcess(){
    int speedHomingY = 1000;
    int accelerationHomingY = 300;
    int speedHomingX = 800;
    int accelerationHomingX = 300;
    int speedHomingZ = 700;
    int accelerationHomingZ = 150;

    while(digitalRead(revBtnZ)){
        stepperZ.setMaxSpeed(speedHomingZ);
        stepperZ.setAcceleration(accelerationHomingZ);
        stepperZ.moveTo(-initZ);
        initZ = initZ + 1;
        stepperZ.run();
    }
     
    while(digitalRead(revBtnY)){
        stepperY.setMaxSpeed(speedHomingY);
        stepperY.setAcceleration(accelerationHomingY);
        stepperY.moveTo(initY);
        initY = initY + 1;
//        accelerationHomingY = accelerationHomingY - 150;
//        speedHomingY = speedHomingY + 100;
        stepperY.run();
    }

    while(digitalRead(revBtnX)){
        stepperX.setMaxSpeed(speedHomingX);
        stepperX.setAcceleration(accelerationHomingX);
        stepperX.moveTo(initX);
        initX = initX + 1;
//        speedHomingX = speedHomingX + 100;
        stepperX.run();
//        stepperX.moveTo(initStepX);
//        initStepX++;
//        stepperX.run();
//        Serial.println(initStepX);
    }
    
    stepperY.setCurrentPosition(0);
    stepperY.setMaxSpeed(1000);
    stepperY.setAcceleration(1000);
    delay(5);
    stepperX.setCurrentPosition(0);
    stepperX.setMaxSpeed(500);
    stepperX.setAcceleration(500);
    delay(5);
    Serial.println("The homing process is complete!");
}

void homingZAxis(){
    while(digitalRead(revBtnZ)){
        stepperZ.setMaxSpeed(800);
        stepperZ.setAcceleration(300);
        stepperZ.moveTo(-initZ);
        initZ++;
        stepperZ.run();
    } 
}

void mistingMode(){
    stepperZ.setCurrentPosition(0);
    stepperZ.setSpeed(700);
    stepperZ.setAcceleration(150);
    stepperZ.move(2300);
    stepperZ.runToPosition();
}

void moveAxisZ(){
    stepperZ.setMaxSpeed(700);
    stepperZ.setAcceleration(150);
    stepperZ.move(600);
    stepperZ.runToPosition();
    delay(1000);
    stepperZ.move(-600);
    stepperZ.runToPosition();
    delay(1000);
}

void automaticMode(){
    int i, j, k;

    Serial.println(0);
    delay(300);

    for(i = 0; i < 13; i++){
        stepperY.move(1140);
        stepperY.runToPosition();
        Serial.println(i+1);
        delay(1000);
    }
        
    stepperX.move(-1500);
    stepperX.runToPosition();
    Serial.println(14);
    delay(1000);

    for (j = 0; j < 13; j++){
        stepperY.move(-1140);
        stepperY.runToPosition();
        Serial.println(j+15);
        delay(1000);  
    }

    stepperX.move(-1500);
    stepperX.runToPosition();
    Serial.println(28);
    delay(1000);

    for (k = 0; k < 13; k++){
        stepperY.move(1140);
        stepperY.runToPosition();
        Serial.println(k+29);
        delay(1000);
    }

    stepperY.setMaxSpeed(800);
    stepperY.setAcceleration(200);
    delay(100);
    stepperX.setMaxSpeed(600);
    stepperX.setAcceleration(200);
    delay(100);
    
    moveFinished = 1;
    if(moveFinished == 1){
        Serial.println("The process has been done successfully!"); 
    }
    delay(5);
    quickHomingProcess();
}

void moveByXYCoordinates(int xCoords, int yCoords){
    if(enableMode2 = 1){
        int xTempResultMode2 = xCoords - xTempMode2;
        int yTempResultMode2 = yCoords - yTempMode2;
        if((yTempResultMode2)!=0){
            stepperY.move(yTempResultMode2);
            stepperY.runToPosition();
        }
        if((xTempResultMode2)!=0){
            stepperX.move(xTempResultMode2);
            stepperX.runToPosition();  
        }
        stepperY.setCurrentPosition(0);
        stepperY.setMaxSpeed(1000);
        stepperY.setAcceleration(300);
        delay(50);
        stepperX.setCurrentPosition(0);
        stepperX.setMaxSpeed(600);
        stepperX.setAcceleration(400);
    }
    xTempMode2 = xCoords;
    yTempMode2 = yCoords;
}

//Mode 3: Move by point orders

void moveByXYCoordinatesDependOnPointNum(int xCol, int yCol){
    if(enableMode3 = 1){
        int xTempResultMode3 = xCol - xTempMode3;
        int yTempResultMode3 = yCol - yTempMode3;
        if((yTempResultMode3)!=0){
            stepperY.move(yTempResultMode3);
            stepperY.runToPosition();
        }
        if((xTempResultMode3)!=0){
            stepperX.move(xTempResultMode3);
            stepperX.runToPosition();  
        }
        stepperY.setCurrentPosition(0);
        stepperY.setMaxSpeed(1000);
        stepperY.setAcceleration(300);
        delay(50);
        stepperX.setCurrentPosition(0);
        stepperX.setMaxSpeed(600);
        stepperX.setAcceleration(400);
    }
    xTempMode3 = xCol;
    yTempMode3 = yCol;
}

//This function will operate the third case of this program's functionabilities
void moveByPointNumber(int pointNum){
    if (pointNum < 0 || pointNum > 41){
        Serial.println("Please enter the valid point (from 0 to 41):");  
    }else{
        if(pointNum == 0){
            int xPointNum = 0;
            int yPointNum = 0;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 1){
            int xPointNum = 0;
            int yPointNum = -1235;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 2){
            int xPointNum = 0;
            int yPointNum = -2470;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 3){
            int xPointNum = 0;
            int yPointNum = -3705;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 4){
            int xPointNum = 0;
            int yPointNum = -4940;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 5){
            int xPointNum = 0;
            int yPointNum = -6175;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 6){
            int xPointNum = 0;
            int yPointNum = -7410;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 7){
            int xPointNum = 0;
            int yPointNum = -8645;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 8){
            int xPointNum = 0;
            int yPointNum = -9880;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 9){
            int xPointNum = 0;
            int yPointNum = -11115;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 10){
            int xPointNum = 0;
            int yPointNum = -12350;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 11){
            int xPointNum = 0;
            int yPointNum = -13585;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 12){
            int xPointNum = -1500;
            int yPointNum = -13585;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 13){
            int xPointNum = -1500;
            int yPointNum = -12350;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 14){
            int xPointNum = -1500;
            int yPointNum = -11115;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 15){
            int xPointNum = -1500;
            int yPointNum = -9880;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 16){
            int xPointNum = -1500;
            int yPointNum = -8645;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 17){
            int xPointNum = -1500;
            int yPointNum = -7410;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 18){
            int xPointNum = -1500;
            int yPointNum = -6175;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 19){
            int xPointNum = -1500;
            int yPointNum = -4940;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 20){
            int xPointNum = -1500;
            int yPointNum = -3705;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 21){
            int xPointNum = -1500;
            int yPointNum = -2470;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 22){
            int xPointNum = -1500;
            int yPointNum = -1235;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 23){
            int xPointNum = -1500;
            int yPointNum = 0;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 24){
            int xPointNum = -3000;
            int yPointNum = 0;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 25){
            int xPointNum = -3000;
            int yPointNum = -1235;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 26){
            int xPointNum = -3000;
            int yPointNum = -2470;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 27){
            int xPointNum = -3000;
            int yPointNum = -3705;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 28){
            int xPointNum = -3000;
            int yPointNum = -4940;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 29){
            int xPointNum = -3000;
            int yPointNum = -6175;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 30){
            int xPointNum = -3000;
            int yPointNum = -7410;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 31){
            int xPointNum = -3000;
            int yPointNum = -8645;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 32){
            int xPointNum = -3000;
            int yPointNum = -9880;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 33){
            int xPointNum = -3000;
            int yPointNum = -11115;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 34){
            int xPointNum = -3000;
            int yPointNum = -12350;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }else if(pointNum == 35){
            int xPointNum = -3000;
            int yPointNum = -13585;
            moveByXYCoordinatesDependOnPointNum(xPointNum,yPointNum);
        }  
    }  
}

void loop() {
  // put your main code here, to run repeatedly:

}
