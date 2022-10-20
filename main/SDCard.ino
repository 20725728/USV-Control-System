void initialize_SD(){
  Serial.print("Initializing SD card..."); //Serial port output data
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) { //If communication from the CS port fails with the SD card, the serial port outputs information: Card failed, or not present
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized."); //SD card communication success, serial output card information initialized.
  //root = SD.open("/");
  //printDirectory(root, 0);
  //writeHeader();
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

//void createNextFile(File dir){
//  int fileCnt = 1;
//  while(true){
//    File entry = dir.openNextFile();
//    if(!entry){
//      //no more files
//      String fileName 
//    }
//  }
//}
