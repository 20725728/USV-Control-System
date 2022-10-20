//20725728
//<Includes>
  #include <SD.h>
  #include <SPI.h>
  #include <Wire.h>
  #include <LSM303.h>
  #include <Time.h>
  #include <math.h>
  #include "MyStructures.h"
  #include "MyHeaders.h"
//</Includes>

//<Global Variables>
  int state = 1; //1-wait for GPS; 2-Navigate; 3-Standyby
  int Manual_Control = 0; // 1=manual Control enabled;
  int Debug = 1;
  bool runProgram = true;
  int steercounter=0;

  /* D13 - S2 (manual/auto)
   * D8 - S3 (green)
   * D4 - S1 (Power)
   * D12 - led on
   * D11 - led active 
   * D9 - led M
   * D7 - led GPs
   * D6 - led A
  */
  int manualS = 13;
  int compassS = 8;
  int powerS = 4;
  int LED_ON = 12;
  int LED_ACTIVE = 11;
  int LED_MAN = 9;
  int LED_GPS = 7;
  int LED_AUTO = 6;

  //Navigation ----------------------------------
  int throttle_down_distance = 7; //(m) distance at which the vessel reduces below full throttle speed.
  int full_steer_angle = 20;//(degrees) the angle beyond which the vessel will use full steering.
  int throttle_percentage = 100; //percentage full throttle applied (full throttle doesnt seem to work)
  int steering_percentage = 100; //percentage steering lock applied.
  int arrival_threshold = 7; //distance at which it is considered that the vessel reached the point.
  
  //GPS ------------------------------------------
  GPSrx pos;
  bool GPS_connected = false;
  char validGPS = 'V';
  int targetIndex = 0;
  
  //PWM ------------------------------------------
  float RC = 84000;
  int POT_LH_input = A1;
  int POT_RH_input = A0;
  PWMControl rightSide;
  PWMControl leftSide;
  
  //SDCard -----------------------------------------
  // CS pin is pin 53,
  const int chipSelect = 53; //Set CS interface
  File root;
  String fileName = "datalog.txt";
  
  //SD Read --------------------------------------
  GPSpt points[20];

  //Compass --------------------------------------
  LSM303 compass;
  LSM303::vector<int16_t> running_min = {-517, -1212, -1124}, running_max = {2672, 1956, 1895};
  int calibrate = 1; //0 - calibrate compass 1- set compass limits 2 - operating mode
  

  //Debug ----------------------------------------
  int addPointsCnt = 0;
//</Global Variables>

void setup(){
  pwmPin3();
  pwmPin5();
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  initialize_SD();
  SetupIOs();
  readGPSPoints();
  compass.m_min = (LSM303::vector<int16_t>){running_min.x, running_min.y, running_min.z};
    compass.m_max = (LSM303::vector<int16_t>){running_max.x, running_max.y, running_max.z};
  //receiveAllGPSPoints();
}


void loop() {
  double test;
  // put your main code here, to run repeatedly: 
  SwitchesAndLEDs();
  if(calibrate == 0){
    CalibrateCompass();
  }else{
    if(runProgram){
      if(Manual_Control == 1){
        updatePWM();
        writeThrottle();
        //printThrottle();
        receiveGPSdata();
      }else{
        
        
        switch(state){
          case 1:
              test = GetHeading();
              //Serial.println(test);
              receiveGPSdata();
              Serial.print("Waiting for GPS\n");
              if(GPS_connected){
                state = 2;
              }
            break;
          case 2:
              receiveGPSdata();
              navigate();
              delay(100);
            break;
          case 3:
              delay(500);
            break;
        }
      }
    }
    delay(200);
  }
}

void Listen(){
    int byteRX = Serial.read();
    if(byteRX != -1){
      switch(byteRX){
        case 'A'://switch to automatic control
          Manual_Control = 0;
          break;
        case 'M'://switch to manual control
          Manual_Control = 1;
          break;
      }
    }
}

void SetupIOs(){
  
  pinMode(manualS, INPUT);
  pinMode(compassS, INPUT);
  pinMode(compassS, INPUT);
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_ACTIVE, OUTPUT);
  pinMode(LED_MAN, OUTPUT);
  pinMode(LED_GPS, OUTPUT);
  pinMode(LED_AUTO, OUTPUT);
  /* D13 - S2 (manual/auto)
   * D8 - S3 (compass calibration)
   * D4 - S1 (Power)
   * D12 - led on
   * D11 - led active
   * D9 - led M
   * D7 - led GPs
   * D6 - led A
  */
}

void SwitchesAndLEDs(){
  if(digitalRead(compassS)){
    calibrate = 0;
  }else{
    if(calibrate == 0){
      calibrate = 1;
    }
  }
  if(digitalRead(powerS)){
    digitalWrite(LED_ON, HIGH);
    runProgram = true;
  }else{//stop the main loop running and set the motors to neutral
    digitalWrite(LED_ON, LOW);
    runProgram = false;
    leftSide.duty_Cycle = 75;
    rightSide.duty_Cycle = 75;
    power_thrusters();
  }
  if(digitalRead(manualS)){
    digitalWrite(LED_MAN, HIGH);
    digitalWrite(LED_AUTO, LOW);
    Manual_Control = 1;
  }else{
    digitalWrite(LED_MAN, LOW);
    digitalWrite(LED_AUTO, HIGH);
    Manual_Control = 0;
  }
  if(digitalRead(LED_ACTIVE)){
    digitalWrite(LED_ACTIVE, LOW);
  }else{
    digitalWrite(LED_ACTIVE, HIGH);
  }
  if(GPS_connected){
    digitalWrite(LED_GPS,HIGH);
  }else{
    digitalWrite(LED_GPS,LOW);
  }
}
