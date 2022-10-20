struct GPSpt{
  int orderPos;
  long latitude;
  double latitudeDecimalDegrees = 0;
  long longitude;
  double longitudeDecimalDegrees = 0;
  bool passed = false;
};

struct GPSrx {
  int date;
  int UTC;
  long latitude;
  double latitudeDecimalDegrees;
  long longitude;
  double longitudeDecimalDegrees;
  double knots;
  double course;
};

struct PWMControl {
  int analog = 505;             //the middle for both of the POTs
  float duty_Cycle = 75;      //75% is neutral.
};
