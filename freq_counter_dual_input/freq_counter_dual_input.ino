// Frequency timer
// Author: Nick Gammon
// Date: 10th February 2012

// Input: Pin D2
#define input_a_pin 8
#define input_b_pin 3

volatile boolean first_input_a;
volatile boolean triggered_input_a;
volatile boolean first_input_b;
volatile boolean triggered_input_b;
volatile unsigned long overflowCount;
volatile unsigned long startTime_input_a;
volatile unsigned long finishTime_input_a;
volatile unsigned long startTime_input_b;
volatile unsigned long finishTime_input_b;

// here on rising edge
void isr_input_a () 
{
  unsigned int counter = TCNT1;  // quickly save it
  
  // wait until we noticed last one
  if (triggered_input_a)
    return;

  if (first_input_a)
    {
    startTime_input_a = (overflowCount << 16) + counter;
    first_input_a = false;
    return;  
    }
    
  finishTime_input_a = (overflowCount << 16) + counter;
  triggered_input_a = true;
  detachInterrupt(digitalPinToInterrupt(input_a_pin));   
}  // end of isr_input_a

void isr_input_b () 
{
  unsigned int counter = TCNT1;  // quickly save it
  
  // wait until we noticed last one
  if (triggered_input_b)
    return;

  if (first_input_b)
    {
    startTime_input_b = (overflowCount << 16) + counter;
    first_input_b = false;
    return;  
    }
    
  finishTime_input_b = (overflowCount << 16) + counter;
  triggered_input_b = true;
  detachInterrupt(digitalPinToInterrupt(input_b_pin));
}  // end of isr_input_b

// timer overflows (every 65536 counts)
ISR (TIMER1_OVF_vect) 
{
  overflowCount++;
}  // end of TIMER1_OVF_vect


void prepareForInterrupts_input_a ()
  {
  // get ready for next time
  EIFR = bit (INTF0);  // clear flag for interrupt 0
  first_input_a = true;
  triggered_input_a = false;  // re-arm for next time
  attachInterrupt(digitalPinToInterrupt(input_a_pin), isr_input_a, RISING);  
  }  // end of prepareForInterrupts_input_a

void prepareForInterrupts_input_b ()
  {
  // get ready for next time
  EIFR = bit (INTF1);  // clear flag for interrupt 1
  first_input_b = true;
  triggered_input_b = false;  // re-arm for next time
  attachInterrupt(digitalPinToInterrupt(input_b_pin), isr_input_b, RISING);     
  }  // end of prepareForInterrupts_input_b
  

void setup () 
  {
  Serial.begin(115200);       
  Serial.println("Frequency Counter");
  
  // reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  // Timer 1 - interrupt on overflow
  TIMSK1 = bit (TOIE1);   // enable Timer1 Interrupt
  // zero it
  TCNT1 = 0;  
  overflowCount = 0;  
  // start Timer 1
  TCCR1B =  bit (CS10);  //  no prescaling

  // set up for interrupts
  prepareForInterrupts_input_a();
  prepareForInterrupts_input_b();
  
  } // end of setup

void loop () 
  {

  if (!triggered_input_a){
    ;
  } else {
    unsigned long elapsedTime_input_a = finishTime_input_a - startTime_input_a;
    float freq_input_a = F_CPU / float (elapsedTime_input_a);  // each tick is 62.5 ns at 16 MHz
    Serial.print ("Frequency a: ");
    Serial.print (freq_input_a);
    Serial.print (" Hz.   ");
    // so we can read it  
    delay (100);
    prepareForInterrupts_input_a ();   
  }

  if (!triggered_input_b){
    Serial.println();
    return;
  } else {
    unsigned long elapsedTime_input_b = finishTime_input_b - startTime_input_b;
    float freq_input_b = F_CPU / float (elapsedTime_input_b);  // each tick is 62.5 ns at 16 MHz
    Serial.print ("Frequency b: ");
    Serial.print (freq_input_b);
    Serial.println (" Hz.");
    // so we can read it  
    delay (100);
    prepareForInterrupts_input_b (); 
  }
}   // end of loop
