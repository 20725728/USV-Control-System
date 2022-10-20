void BangBang(){
  double distance;
  double bearing_error;
  double target_bearing;
  double maxBearing;
  double minBearing;

  GPSpt target = points[targetIndex];
  distance = GetDistance(pos.latitudeDecimalDegrees, pos.longitudeDecimalDegrees, target.latitudeDecimalDegrees, target.longitudeDecimalDegrees);
  target_bearing = GetBearing(pos.latitudeDecimalDegrees, pos.longitudeDecimalDegrees, target.latitudeDecimalDegrees, target.longitudeDecimalDegrees);
  bearing_error = target_bearing - pos.course;
  if(distance < arrival_threshold){
    points[targetIndex].passed = true;
    targetIndex ++;
  }
  maxBearing = max(pos.course, target_bearing);
  minBearing = min(pos.course, target_bearing);
  if(abs(bearing_error) > 20){
    if((maxBearing == target_bearing) ^ ((maxBearing-minBearing)>180)){
      turnRight();
    }else{
      turnLeft();
    }
  }else{
    forward();
  }
  power_thrusters();
}

void turnLeft(){
  leftSide.duty_Cycle = 0.55;
  rightSide.duty_Cycle = 0.95;
}

void turnRight(){
  rightSide.duty_Cycle = 0.55;
  leftSide.duty_Cycle = 0.95;
}

void forward(){
  rightSide.duty_Cycle = 0.95;
  leftSide.duty_Cycle = 0.95;
}

void reverse(){
  rightSide.duty_Cycle = 0.55;
  leftSide.duty_Cycle = 0.55;
}
