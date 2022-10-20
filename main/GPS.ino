void receiveGPSdata() {
  bool msgReceived = false;
  char rxData[100];
  int rxCnt = 0;
  int rxByte = 0;
  while (!msgReceived) {

    while (rxByte != 10) {
      rxByte = Serial1.read();
      if (rxByte != -1) {
        rxData[rxCnt] = rxByte;
        rxCnt ++;
      }
    }
    //Serial.write(rxData,rxCnt);
    if (rxData[3]=='R' && rxData[4] == 'M' && rxData[5] == 'C') {
      updateStructRMC(rxData,rxCnt);
      //printStruct();
      if(Debug != 1){
        saveGPSPoints();
      }
      msgReceived = true;
    }
    rxCnt = 0;
    rxByte = 0;
  }
}

int charToInt(char data[], int startIndex, int endIndex){
  int output = 0;
  int power = endIndex-startIndex-2;
  for(int i = startIndex+1;i<endIndex;i++){
    output += (data[i] - 48) * pow(10,power);
    power--;
  }
  return output;
}

float charToDecimals(char data[], int startIndex, int endIndex){
  float output = 0;
  int power = 1;
  for(int i = startIndex+1;i < endIndex; i++){
    output += (data[i] - 48) / pow(10,power);
    power++;
  }
  return output;
}

void printStruct(){
  long latPos = -3393849;
  long longPos = 1847265;
  
//  String toString = "Struct:\n\tTime (UTC): "+pos.UTC+"\n\tLatitude: "+pos.lat+" "+pos.n_s+"\n\tLongitude: "+pos.longi+" "+pos.e_w+
//  "\n\tSpeed: "+pos.knots+" knots on bearing: "+pos.course+" degrees\n\tMagnetic Variation: "+pos.magVar+" "+pos.mag_e_w+"\n\tMode: "+pos.mode;
  Serial.println("Struct:");
  Serial.println("\tTime (UTC): "+String(pos.UTC)+"\tDate: "+String(pos.date));
  Serial.println("DM.M:");
  Serial.println("\tLatitude: "+String(pos.latitude)+"\tLongitude: "+String(pos.longitude));
  Serial.println("Decimal Degrees:");
  Serial.println("\tLatitude: "+String(pos.latitudeDecimalDegrees)+"\tLongitude: "+String(pos.longitudeDecimalDegrees));

  long errorDeg = GetDistance(latPos,longPos, pos.latitudeDecimalDegrees, pos.longitudeDecimalDegrees);
  float bearingError = GetBearing(latPos,longPos, pos.latitudeDecimalDegrees, pos.longitudeDecimalDegrees);
  float errorMetre = (float)((float)errorDeg/100000)*(111139);
  Serial.println("\tError(Deg):\t"+String(errorDeg)+"\tAngle: "+String(bearingError));
  Serial.println("\tError(m):\t"+String(errorMetre));
  Serial.println("\tSpeed: "+String(pos.knots)+"\tBearing: "+String(pos.course)+"\n\n\n");
}

void findAllInstancesOf(int* fill, char data[], char toFind, int arraySize){
  int indexCnt = 0;
  for(int i =0;i<arraySize;i++){
    if(data[i] == toFind){
      fill[indexCnt] = i;
      indexCnt ++;
    }
  }
}

void updateStructRMC(char data[], int cnt) {
  //assume fixed length variables in the message
  //Time    Lat           N/S   Long          E/W     speed   course    date
  //hhmmss  ddmm.mmmm     N     dddmm.mmmm    E       x.xx    xxx.xx    ddmmyy 
  int commas[12];
  int points[6];
  
  findAllInstancesOf(commas,data,',',cnt);
  findAllInstancesOf(points,data,'.',cnt);
  pos.UTC = charToInt(data,commas[0],points[0]);
  validGPS = data[commas[1]+1];
  if(validGPS == 'A'){
    
    pos.latitude = charToInt(data,commas[2],points[1])*10000 + charToInt(data,points[1],commas[3]);
    pos.latitudeDecimalDegrees = (long)(pos.latitude/1000000) + (double)(pos.latitude%1000000)/600000;
    if(data[commas[3]+1] == 'S'){
      pos.latitudeDecimalDegrees *= -1;
    }
    pos.longitude = charToInt(data,commas[4],points[2])*10000 + charToInt(data,points[2],commas[5]);
    pos.longitudeDecimalDegrees = (long)(pos.longitude/1000000) + (double)(pos.longitude%1000000)/600000;
    if(data[commas[5]+1] == 'W'){
      pos.longitude *= -1;
    }
    pos.knots = charToInt(data,commas[6],points[3]) + charToDecimals(data, points[3],commas[7]);
    pos.course = GetHeading();
    pos.date = charToInt(data,commas[8],commas[9]);
    if(!GPS_connected){
      GPS_connected = true;
      //fileName = String(pos.date)+"_"+String(pos.UTC)+".txt";
      //writeHeader();      
    }
    //printStruct();
  }else{
    Serial.write("No Satellite Connection\n");
  }
}
