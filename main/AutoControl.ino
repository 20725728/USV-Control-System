void navigate(){
  double distance;
  double bearing_error;
  double target_bearing;
  double maxBearing;
  double minBearing;

  GPSpt target = points[targetIndex];
  distance = GetDistance(pos.latitudeDecimalDegrees, pos.longitudeDecimalDegrees, target.latitudeDecimalDegrees, target.longitudeDecimalDegrees);
  target_bearing = GetBearing(pos.latitudeDecimalDegrees, pos.longitudeDecimalDegrees, target.latitudeDecimalDegrees, target.longitudeDecimalDegrees);
  double bearingE_1 = target_bearing - pos.course;
  double bearingE_2 = abs(target_bearing - pos.course) - 360;
  if(abs(bearingE_1) < abs(bearingE_2)){
    bearing_error = bearingE_1;
  }else{
    bearing_error = bearingE_2;
  }  
  if(distance < throttle_down_distance){
    throttle_percentage = distance/(2*throttle_down_distance);
    if(distance < arrival_threshold){
      NextPoint();
    }
  }else{
    throttle_percentage = 95;
  }
  if(abs(bearing_error) < full_steer_angle){
    steering_percentage = ((float)abs(bearing_error)/(float)full_steer_angle) * 100;
  }else{
    steering_percentage = 100;
  }
  maxBearing = max(pos.course, target_bearing);
  minBearing = min(pos.course, target_bearing);
  if((maxBearing == target_bearing) ^ ((maxBearing-minBearing)>180)){
    writeError(distance, bearing_error, target_bearing, "RIGHT");
    steer_right();
  }else{
    steer_left();
    writeError(distance, bearing_error, target_bearing, "LEFT");
  }
  
  power_thrusters();
  if(Debug == 1){
    writeError(distance, bearing_error, target_bearing, "debug");  
  }else{
    writeNavigation(distance, bearing_error, target_bearing);
  }
  writeThrottle();

}



void steer_left(){
  int throttle_difference = 2*throttle_percentage;
  rightSide.duty_Cycle = 75 + (25*((float)throttle_percentage/100));
  leftSide.duty_Cycle = rightSide.duty_Cycle - (25 * ((throttle_difference * ((float)steering_percentage/100))/100));
  if(rightSide.duty_Cycle >=100){
    rightSide.duty_Cycle = 95;
  }
  if(leftSide.duty_Cycle >= 100){
    leftSide.duty_Cycle = 95;
  }
}

void steer_right(){
  int throttle_difference = 2*throttle_percentage;
  leftSide.duty_Cycle = 75 + (25*((float)throttle_percentage/100));
  rightSide.duty_Cycle = leftSide.duty_Cycle - (25* ((throttle_difference * ((float)steering_percentage/100))/100));
  if(rightSide.duty_Cycle >=100){
    rightSide.duty_Cycle = 95;
  }
  if(leftSide.duty_Cycle >= 100){
    leftSide.duty_Cycle = 95;
  }
}
