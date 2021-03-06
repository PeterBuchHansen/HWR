//Interrupt attempt
#include <EnableInterrupt.h>

//Varuables for motorController A
//---------------------------------
int current_speed_A = 0; //Variable to hold the speed value
int last_Pos_A = 0; //Variable to hold the last possition
int desired_speed_A = 0; //Variable to hold the decired speed
int desired_speed_B = 0; //Variable to hold the decired speed
int speed_Calc_A = 0;

//General P controller values
//-----------------------------
double errorVal_A = 0.0; //The direct error value
double P_SPD = 160.0; //Speed P
int P_A = 0; //PID output control value for motor A

//Variables for tick counter on motor
//-------------------------------------
int ENC_A1 = 2; //Pin for encoder 1 on Motor A
int ENC_B1 = 4; //Pin for encoder 1 on Motor B
int posVal_A = 0;

//Variables for the value where the timer will interrupt
//------------------------------------------------------
int timer1_counter;

//General variables
//-----------------
int led = 13;
int or_Interrupt = 2;

//Pin difinition for motorboard
//-----------------------------
int dirA = 12;
int pwmA = 3;
int dirB = 13;
int pwmB = 11;


//-----------------------------
//------- Void setup ----------
//-----------------------------
void setup() {
  //General pinMode
  //---------------
  pinMode(led, OUTPUT);
  pinMode(2, INPUT);

  //Pin modes for the encoders
  pinMode(ENC_A1, INPUT);     //set the pin to input
  digitalWrite(ENC_A1, HIGH); //use the internal pullup resistor

  // Attach a PinChange Interrupt to our pin on the
  // rising edge used to handle the interrupts from
  // the two encoders.
  //-----------------------------------------------
  enableInterrupt(ENC_A1, enc_Int_MotorA, CHANGE);

  //Setting up the serial communication for debugging
  //-------------------------------------------------
  Serial.begin(9600);

  //Configuration of the motorboard port A
  //--------------------------------------
  pinMode(dirA, OUTPUT); //Pin controlling the direction of the motor
  pinMode(pwmA, OUTPUT); //Pin for controlling the speed of the motor
  digitalWrite(dirA, LOW); //Setting the direction of the motor with either HIGH or LOW
  analogWrite(pwmA, 0); //Initializing the motor at speed 0

  //Configuration of the motorboard port B
  //--------------------------------------
  pinMode(dirB, OUTPUT); //Pin controlling the direction of the motor
  pinMode(pwmB, OUTPUT); //Pin for controlling the speed of the motor
  digitalWrite(dirB, LOW); //Setting the direction of the motor with either HIGH or LOW
  analogWrite(pwmB, 0); //Initializing the motor at speed 0

  // initialize timer1 used for the PID motor controller
  //----------------------------------------------------
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz (10ms)

  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts

  settings();
}

//-----------------------------------------------------------------
//---Interrupt service routine for handling the timer interrupt---
//-----------------------------------------------------------------
ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TCNT1 = timer1_counter;   // preload timer
  motorControl();
}


//------------------------- P controller ---------------------------
// This controller is set so that it assures constand speed on motor A
//--------------------------------------------------------------------
void motorControl()
{
  current_speed_A = (posVal_A - last_Pos_A);   //Calculating the speed of motor A
  last_Pos_A = posVal_A;                       //Saving current tick-count

  //Calculating error values
  errorVal_A = desired_speed_A - current_speed_A; //The speed error

  //Calculation of the PWM value used to drive the motor
  P_A = (int)(P_SPD * errorVal_A);

  //Decides if the motor should turn right or left
  if (P_A < 0)
  {
    digitalWrite(dirA, HIGH);
  }
  else
  {
    digitalWrite(dirA, LOW);
  }

  //Limits the maximum output
  if (abs(P_A) > 255)
  {
    P_A = 255;
  }
  analogWrite(pwmA, abs(P_A));
  analogWrite(pwmB, desired_speed_B);
}


//--------------------Tick interrupt routine-----------------------
// This is the interrupt service routine that handles the interrupt
// from the encoder on motor A.
//-----------------------------------------------------------------
void enc_Int_MotorA()
{
  noInterrupts(); //Disables interrupts

      posVal_A++;

  interrupts();//Enable interrupts
}

void settings()
{
  //Sets the speed of the motor A (1-8)
  desired_speed_A = 2;
  //Sets the PWM of motor B - NO CONTROL
  desired_speed_B = 53;
}

void loop() {
  
  //Serial.print(PID_A);
  //Serial.print(", ");
  //Serial.print(errorVal_A);
  //Serial.print(", ");
  //Serial.println(errorSum_A);
  //Serial.print(", ");
  //Serial.println(current_speed_A);
  
  //delay(100);

}
