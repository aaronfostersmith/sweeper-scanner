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
  ADC Timer - Timer4 operates at fM/4
*/
void configure_timers() {
  //Timer 1: Shift gate timer (SH)
  Timer1.pause();

  //Timer 2: Integration Clear Gate (ICG)
  Timer2.pause();

  //Timer 3: Master Clock (fM). Frequency range 0.8-4 Mhz
  Timer3.pause();
  pinMode(PA6, PWM);
  TIMER3_BASE->CR2  = (TIMER_CR2_MMS_ENABLE); //enable master mode 
  Timer3.setPrescaleFactor(1); //72 MHZ, 13.89 ns period
  Timer3.setCompare(TIMER_CH1, 36);
  Timer3.setOverflow(72);
  Timer3.refresh();

  //Timer 4: ADC Sample Rate. Frequency = fM/4
  //prescaler  =PRSK_fM*4    frequency = FREQ_fM/4
  //slave to Timer3: fM
  Timer4.pause();
  pinMode(PB6, PWM);
  Timer4.setSlaveFlags(TIMER_SMCR_TS_ITR2 | TIMER_SMCR_SMS_TRIGGER);
  Timer4.setPrescaleFactor(4); //72 MHZ, 13.89 ns period
  Timer4.setCompare(TIMER_CH1, 36);
  Timer4.setOverflow(72);
  Timer4.refresh();

  //Let the Timers run
  Timer1.resume();
  Timer2.resume();
  Timer3.resume();
  Timer4.resume();

}


