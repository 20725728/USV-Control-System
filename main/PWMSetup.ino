void updatePWM() {
  Right_Throttle();
  Left_Throttle();
//  Serial.write("LEFT: ");
//  Serial.print(leftSide.analog);
//  Serial.write("    RIGHT: ");
//  Serial.print(rightSide.analog);
//  Serial.println();
//  Serial.write("LEFT: ");
//  Serial.print(leftSide.duty_Cycle);
//  Serial.write("    RIGHT: ");
//  Serial.print(rightSide.duty_Cycle);
//  Serial.println();
  
}

void power_thrusters(){
  TC2->TC_CHANNEL[1].TC_RA = (RC*rightSide.duty_Cycle)/100;
  TC2->TC_CHANNEL[0].TC_RA = (RC*leftSide.duty_Cycle)/100;
}

/**
   left                 * right
   max ~655             * max ~720
   min ~285             * min ~352
   mid~480              * mid ~538
   440 - 505 stop       * 485 - 572 stop
*/


void Right_Throttle(){
  float analog_Ratio = 0;
  float difference = 0;
  rightSide.analog = analogRead(POT_RH_input);
  if(rightSide.analog > 720){                    // Full Forward
    rightSide.duty_Cycle = 95;
  }else  if(rightSide.analog >565){               //Forward
    analog_Ratio = (float)(720 - rightSide.analog)/(float)(720 - 565);
    difference = (float)(95 - 75) * analog_Ratio;
    rightSide.duty_Cycle = 95 - difference;
  }else if(rightSide.analog < 355){             // Full Reverse
    rightSide.duty_Cycle = 50;
  }else if(rightSide.analog < 500){         //Reverse
    analog_Ratio = (float)(500 - rightSide.analog)/(float)(500-355);
    difference = (float)(75-50) * analog_Ratio;
    rightSide.duty_Cycle = 75 - difference;
  }else{                                 //Neutral
    rightSide.duty_Cycle = 75;
  }
  TC2->TC_CHANNEL[1].TC_RA = (RC*rightSide.duty_Cycle)/100;
}

void Left_Throttle(){
  float analog_Ratio = 0;
  float difference = 0;
  leftSide.analog = analogRead(POT_LH_input);
  if(leftSide.analog > 655){                    // Full Forward
    leftSide.duty_Cycle = 95;
  }else  if(leftSide.analog >505){               //Forward
    analog_Ratio = (float)(655 - leftSide.analog)/(float)(665 - 505);
    difference = (float)(95 - 75) * analog_Ratio;
    leftSide.duty_Cycle = 95 - difference;
  }else if(leftSide.analog < 285){             // Full Reverse
    leftSide.duty_Cycle = 50;
  }else if(leftSide.analog < 440){         //Reverse
    analog_Ratio = (float)(440 - leftSide.analog)/(float)(440-285);
    difference = (float)(75-50) * analog_Ratio;
    leftSide.duty_Cycle = 75 - difference;
  }else{                                 //Neutral
    leftSide.duty_Cycle = 75;
  }
  TC2->TC_CHANNEL[0].TC_RA = (RC*leftSide.duty_Cycle)/100;
}

void pwmPin3() {
  PMC->PMC_PCER1 |= PMC_PCER1_PID34;                        // TC7 power ON - Timer Counter 2 channel 1 IS TC7 - See page 38

  PIOC->PIO_PDR |= PIO_PDR_P28;                             // The pin is no more driven by GPIO
  PIOC->PIO_ABSR |= PIO_PC28B_TIOA7;                        // Periperal type B  - See page 859

  TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1    // MCK/2, clk on rising edge
                              | TC_CMR_WAVE                 // Waveform mode
                              | TC_CMR_WAVSEL_UP_RC         // UP mode with automatic trigger on RC Compare
                              | TC_CMR_ACPA_CLEAR           // Clear TIOA7 on RA compare match  -- See page 883
                              | TC_CMR_ACPC_SET;            // Set TIOA7 on RC compare match
  /// Mck = 84 MHz
  TC2->TC_CHANNEL[1].TC_RC = RC;  //<*********************  Frequency = (Mck/2)/TC_RC  Hz = 500 Hz
  TC2->TC_CHANNEL[1].TC_RA = 63000; //<********************   Duty cycle = (TC_RA/TC_RC) * 100

  TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;                  // Interrupt on RC compare match
  NVIC_EnableIRQ(TC7_IRQn);

  TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN;  // Software trigger TC7 counter and enable
}

void pwmPin5() {
  /*Set power management controller register
   * PMC - power controller register                                
   * PMC_PCER1 - Peripheral clock enable register 1                 Pg 563
   * PMC_PCER1_PID33 - Enable peripheral clock PID33 - Timer Channel 6
  */
  PMC->PMC_PCER1 |= PMC_PCER1_PID33;                     // TC6 power ON - Timer Counter 2 channel 2 IS TC6 - See page 38

  /*Disable pin input/output
   * PIOC - parallel input output controller C
   * PIO_PDR - PIO Disable Register                                 Pg 634
   * PIO_PDR_P25 - Disable P25 (PC25 = TIOA6)
   * PIO_ABSR - Peripheral AB select register                       Pg 656
   * PIO_PC25B_TIOA6 - set PC25 = peripheral B function (TIOA6)
   */
  PIOC->PIO_PDR |= PIO_PDR_P25;                          // The pin is no more driven by GPIO
  PIOC->PIO_ABSR |= PIO_PC25B_TIOA6;                     // Periperal type B  - See page 859

  /* Set Timer counter settings                 TC UI               Pg 879
   * TC_CMR - Channel mode register                                 Pg 883
   * TC_CMR_TCCLKS_TIMER_CLOCK1 - Set timer clock 1 (Mck/2) & counter incremented on rising edge
   * TC_CMR_WAVE - Enable waveform mode
   * TC_CMR_WAVSEL_UP_RC - UP mode with automatic trigger on RC compare
   * TC_CMR_ACPA_CLEAR - Clear effect on TIOA on RA compare
   * TC_CMR_ACPC_SET - Set effect on TIOA on RC compare
   * 
   * TC_RC - Register C                                             Pg 891
   * TC_RA - Register A                                             Pg 889
   * 
   * TC_IER - Interrupt enable register                             Pg 894
   * TC_IER_CPCS - enable interrupt on RC Compare match
   * 
   * TC_CCR - Channel Control Register                              Pg 880
   * TC_CCR_SWTRG - A software trigger is performed: the counter is reset and the clock is started.
   * TC_CCR_CLKEN - enable clock
   */
  TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1  // MCK/2, clk on rising edge
                              | TC_CMR_WAVE               // Waveform mode
                              | TC_CMR_WAVSEL_UP_RC        // UP mode with automatic trigger on RC Compare
                              | TC_CMR_ACPA_CLEAR          // Clear TIOA6 on RA compare match  -- See page 883
                              | TC_CMR_ACPC_SET;           // Set TIOA6 on RC compare match
  /// Mck = 84 MHz
  TC2->TC_CHANNEL[0].TC_RC = RC;  //<*********************  Frequency = (Mck/2)/TC_RC  Hz = 500Hz
  TC2->TC_CHANNEL[0].TC_RA = 63000; //<********************   Duty cycle = (TC_RA/TC_RC) * 100

  TC2->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;                 // Interrupt on RC compare match
  NVIC_EnableIRQ(TC6_IRQn);

  TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // Software trigger TC6 counter and enable
}

void  TC6_Handler() {
  /*Interrupt triggered on RC compare match
  * TC_SR - Status Register                                         Pg - 892        
  * Read the status registe to clear the RC compare and RA compare match flags.
   */
  TC2->TC_CHANNEL[0].TC_SR;
}


void TC7_Handler() {
  TC2->TC_CHANNEL[1].TC_SR;
}
