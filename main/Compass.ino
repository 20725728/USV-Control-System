void CalibrateCompass(){
  char report[80];
  compass.read();
  running_min.x = min(running_min.x, compass.m.x);
  running_min.y = min(running_min.y, compass.m.y);
  running_min.z = min(running_min.z, compass.m.z);

  running_max.x = max(running_max.x, compass.m.x);
  running_max.y = max(running_max.y, compass.m.y);
  running_max.z = max(running_max.z, compass.m.z);
   snprintf(report, sizeof(report), "min: {%+6d, %+6d, %+6d}    max: {%+6d, %+6d, %+6d}",
    running_min.x, running_min.y, running_min.z,
    running_max.x, running_max.y, running_max.z);
  Serial.println(report);
}

double GetHeading(){
  if(calibrate = 1){
    compass.m_min = (LSM303::vector<int16_t>){running_min.x, running_min.y, running_min.z};
    compass.m_max = (LSM303::vector<int16_t>){running_max.x, running_max.y, running_max.z};
    calibrate = 2;
  }
  char report[80];
  compass.read();
  Serial.print("Reading Heading");
//  snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
//    compass.a.x, compass.a.y, compass.a.z,
//    compass.m.x, compass.m.y, compass.m.z);
  Serial.println(compass.heading());
  return compass.heading();
}
