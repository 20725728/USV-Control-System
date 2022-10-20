
double GetBearing(double latitude1, double longitude1, double latitude2, double longitude2){
  double phi1 = latitude1 * (PI/180);
  double phi2 = latitude2 * (PI/180);
  double delta_phi = (latitude2 - latitude1) * (PI/180);
  double delta_lambda = (longitude2 - longitude1) * (PI/180);
  double theta = (double)atan2( sin(delta_lambda)*cos(phi2), cos(phi1)*sin(phi2) - sin(phi1)*cos(phi2)*cos(delta_lambda));
  
  double bearing = ((theta * 180/PI));
  if(bearing<0){
    return bearing+360;
  }
  return bearing;
}

//return the distance between two GPS points in meters
double GetDistance(double latitude1, double longitude1, double latitude2, double longitude2){
  double radius = 6.371E6;
  double phi1 = latitude1 * (PI/180);
  double phi2 = latitude2 * (PI/180);
  double delta_phi = (latitude2 - latitude1) * (PI/180);
  double delta_lambda = (longitude2 - longitude1) * (PI/180);
  
  double a = sq(sin(delta_phi/2)) + cos(phi1) * cos(phi2) * sq(sin(delta_lambda/2));
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double dist = radius * c;

  return dist;
}

void NextPoint(){
  points[targetIndex].passed = true;
  Serial.print("\nPoint passed\n");
  targetIndex ++;
  Serial.println("Target Index :"+String(targetIndex));
  if(targetIndex >=20 || points[targetIndex].orderPos <=0){
    state = 3;
  }
}
