/*
  fM - Master Clock: 0.8 - 4 MHz --> Timer3
  SH - shift gate --> Timer1
  ICG - Integration Clear Gate --> Timer2 generates an isr which starts ADC timer and DMA
  ADC must match data rate = fM/4 --> Timer4 used as external interrupt for ADC

  resources: https://github.com/pingumacpenguin/STM32-O-Scope
           https://tcd1304.wordpress.com/
*/

#define FREQ_fM 2   // MHZ
#define PRSK_fM 1

#define FREQ_SH
const int8_t analogInPin = PB0;   // Analog input pin: any of LQFP44 pins (PORT_PIN), 10 (PA0), 11 (PA1), 12 (PA2), 13 (PA3), 14 (PA4), 15 (PA5), 16 (PA6), 17 (PA7), 18 (PB0), 19  (PB1)

//fM is the master clock and runs on Timer3
//
void setup()
{
  configure_timers();
}

void loop() {

}
/*
   ADC is configured to take samples  triggered by Timer4 at FREQ_fM/4.
   DMA is used in circular mode to continuously transfer peripheral to memory
*/
//void configure_ADC () {
//
//  pinMode(analogInPin, INPUT_ANALOG);
//
//  Timer4.pause();
//
//  //Single-channel continuous conversion mode on ADC1 using TIMER4 to start conversion
//  //get which ADC chanel relates to the selected analog pin.
//  int pinMapADCin = PIN_MAP[analogInPin].adc_channel;
//  adc_set_reg_seqlen(ADC1, 1); //what is this?
//  ADC1->regs->SQR3 = pinMapADCin; //what is this?
//  adc_set_sample_rate(ADC1, ADC_SMPR_1_5); //sample rate <1.5 ADC cycles //DO I NEED THIS?
//  ADC1->regs->CR2 |= ADC_CR2_CONT; //continuous mode
//  ADC1->regs->CR2 |= ADC_CR2_DMA; //enable DMA
//  adc_set_extsel(ADC1, ADC_ADC12_TIM4_CC4); //TRIGGER ADC CONVERSION ON TIMER4 COMPARE
//  adc_set_exttrig(ADC1, true);
//  regs->CR2 |= ADC_CR2_SWSTART; // start conversion
//
//  Timer4.resume(); // let timer 4 run
//}

/*
   fM - Master Clock: 0.8 - 4 MHz --> Timer3
  SH - shift gate --> Timer1
  ICG - Integration Clear Gate --> Timer2 generates an isr which starts ADC timer and DMA
  ADC Timer - Timer3 operates at fM/4
*/
void configure_timers() {
  const int t_int = 4000 * 72 / 1000; //4 us
  const int t1 = 1000 * 72 / 1000;
  const int t2 = 100 * 72 / 1000;
  const int t3 = t_int / 2;
  const int t4 = 20 * 72 / 1000;
  const int n_samples = 3694;
  const int PRSK_ICG = 18;

  //Timer 1:  Master Clock (fM). Frequency range 0.8-4 Mhz
  Timer1.pause();
  pinMode(PA8, PWM);
  TIMER1_BASE->CR2  = (TIMER_CR2_MMS_ENABLE); //enable master mode
  Timer1.setPrescaleFactor(PRSK_fM); //72 MHZ, 13.89 ns period
  Timer1.setOverflow(FREQ_fM * 18 / PRSK_fM -1);
  Timer1.setCompare(TIMER_CH1, (FREQ_fM * 18 / PRSK_fM - 1) / 2);
  Timer1.refresh();


  //Timer 2: Integration Clear Gate (ICG)
  //ICG must go high with a delay (t1) of minimum 1000 ns after SH goes low.
  //ICG must go high when fM is high (I’m not sure this is actually needed).
  //fM is used as trigger
  Timer2.pause();
  pinMode(PA0, PWM);
  Timer2.setSlaveFlags(TIMER_SMCR_TS_ITR0 | TIMER_SMCR_SMS_TRIGGER);
  Timer2.setPrescaleFactor(PRSK_ICG);
  Timer2.setOverflow((t_int * n_samples)/PRSK_ICG - 1);
  Timer2.setCompare(TIMER_CH1, (t_int * n_samples - (t3 + t2 + t1))/PRSK_ICG);
  Timer2.refresh();



  //Timer 3:  Shift gate timer (SH)
  //SH must go high with a delay (t2) of between 100 and 1000 ns after ICG goes low.
  //SH must stay high for (t3) a minium of 1000 ns.
  //fM (Timer1) is used as a trigger
  Timer3.pause();
  pinMode(PA6, PWM);
  Timer3.setSlaveFlags(TIMER_SMCR_TS_ITR0 | TIMER_SMCR_SMS_TRIGGER);
  Timer3.setPrescaleFactor(PRSK_fM);
  Timer3.setOverflow(t_int - 1);
  Timer3.setCompare(TIMER_CH1, t_int / 2);
  Timer3.refresh();


/*
  //Timer 4: ADC Sample Rate. Frequency = fM/4
  //prescaler  =PRSK_fM*4    frequency = FREQ_fM/4
  //slave to Timer3: fM
  //fM (Timer1) is used as a prescaler
  Timer4.pause();
  //pinMode(PB6, PWM);
  Timer4.setSlaveFlags(TIMER_SMCR_TS_ITR0 | TIMER_SMCR_SMS_TRIGGER);
  Timer4.setPrescaleFactor(PRSK_fM * 4); //72 MHZ, 13.89 ns period
  Timer4.setCompare(TIMER_CH1, (36 / PRSK_fM - 1) / 2);
  Timer4.setOverflow(36 / PRSK_fM - 1);
  Timer4.refresh();
*/

  //preload slave timers to get timing right
  TIMER2_BASE -> CNT = ( t3+t1-t_int/2)/PRSK_ICG;//t_int/2 is 90 deg phase shift for sh
  TIMER3_BASE -> CNT = 0;


  //Let the Timers run (all timers triggered by timer1)
  Timer1.resume();

}


