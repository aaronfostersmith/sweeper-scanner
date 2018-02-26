/*
fM - Master Clock: 0.8 - 4 MHz --> Timer3
SH - shift gate --> Timer1
ICG - Integration Clear Gate --> Timer2 generates an isr which starts ADC timer and DMA
ADC must match data rate = fM/4 --> Timer4 used as external interrupt for ADC
*/

#define FREQ_fM 2   // MHZ
#define PRSK_fM 1 

//fM is the master clock and runs on Timer3
//
void setup()
{
  // setup PA6 (Timer3 channel 1) to generate 1 ms period PWM with 50% DC
  pinMode(PA6, PWM);
  Timer3.pause();
  Timer3.setPrescaleFactor(PRSK_fM); //72 MHZ, 13.89 ns period
  Timer3.setCompare(TIMER_CH1, 36/(PRSK_fM*FREQ_fM));
  Timer3.setOverflow(((72/FREQ_fM)-1)/PRSK_fM);
  Timer3.refresh();
  Timer3.resume(); // let timer 3 run
}

void loop(){
  delay(1);
}

