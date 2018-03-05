

#define FREQ_fM 2   // MHZ
#define PRSK_fM 1


void setup()
{
  configure_timers();
}

void loop() {

}

void configure_timers() {
  const int t_int = 4000 * 72 / 1000; //20 us
  const int t1 = 1000 * 72 / 1000;
  const int t2 = 100 * 72 / 1000;
  const int t3 = t_int / 2;
  const int t4 = 20 * 72 / 1000;
  const int n_samples = 5;

  Timer2.pause();
  pinMode(PA0, PWM);
  TIMER2_BASE->SMCR = (TIMER_SMCR_SMS_TRIGGER | TIMER_SMCR_TS_ITR2 );
  Timer2.setPrescaleFactor(1);
  Timer2.setOverflow(t_int * n_samples - 1);
  Timer2.setCompare(TIMER_CH1, t_int * n_samples - (t3 + t2 + t1));
  Timer2.refresh();

  //sh
  Timer3.pause();
  pinMode(PA6, PWM);
  TIMER3_BASE->CR2  = (TIMER_CR2_MMS_ENABLE); //enable master mode
  Timer3.setPrescaleFactor(1);
  Timer3.setOverflow(t_int - 1);
  Timer3.setCompare(TIMER_CH1, t_int / 2);
  Timer3.refresh();



  //preload the cnt registers to get timing right.
  TIMER2_BASE -> CNT =(t1+t3);
  //TIMER2_BASE -> CNT = 0;
  TIMER3_BASE -> CNT = 0;

  //Let the Timers run
  // Timer2.resume();
  Timer3.resume();

}


