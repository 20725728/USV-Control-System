

void readGPSPoints(){
  File gpsFile = SD.open("points.txt");
  char pointsBuf[540];
  if(gpsFile){
    int fileLength = gpsFile.available();
    gpsFile.read(pointsBuf, fileLength);
    Serial.println(pointsBuf);
    Serial.println("fileLength: "+String(fileLength));
    decodeGPSBuf(pointsBuf, (fileLength - 20)/22);
  }else{
    Serial.println("Error getting gpsPoints");
  }
}

void decodeGPSBuf(char buf[], int numberPoints){
  int buffpos = 0;
  for(int i = 0;i<numberPoints;i++){
    buffpos = i*22;
    Serial.println(buf[buffpos]);
    points[i].orderPos = buf[buffpos]*10 + buf[buffpos+1];
    buffpos+=2;
    points[i].latitudeDecimalDegrees = (double) ((double)charToInt(buf, buffpos, buffpos+8) /100000);
    buffpos+=8;
    if(buf[buffpos] == 'S'){
      points[i].latitudeDecimalDegrees *= -1;
    }
    buffpos+=1;
    points[i].longitudeDecimalDegrees =  (double) ((double)charToInt(buf, buffpos, buffpos+8) /100000);
    buffpos+=8;
    if( buf[buffpos] == 'W'){
      points[i].longitudeDecimalDegrees *= -1;
    }
    Serial.println("Point "+String(i)+": "+String(points[i].latitudeDecimalDegrees,7)+"\t"+String(points[i].longitudeDecimalDegrees,7));
  }
}
