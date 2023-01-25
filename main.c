
#include <stdint.h> 
#include "header_2.h" 

/*
This part of the lab (task 1) had 2 main goals- 1) turn on the lights in a specific pattern
depending on the timers instead of delays
*/

enum TL_States { TL_init, TL_stop, TL_warn, TL_go } TL_State;

int main(void)
{
  timer_initc();
  led_init();
  TL_State = TL_init;
  while (1) {
    Traffic_Light_System();
  }
  return 0;
}

void timer_initc() {
  RCGCTIMER |= 0x1; // Enable the appropriate TIMERn bit in the RCGCTIMER register
  GPTMCTL_0 &= ~0x1; // Disable the timer using the GPTMCTL register
  GPTMCFG_0 = 0x00; // Write 0x0000.0000 to the GPTMCFG register, 
  GPTMCFG_0 &= 0x00; //select 32-bit mode using the GPTMCFG register
  
  GPTMTAMR_0 &= ~0x10; // Configure the TACDIR bit of the GPTMTAMR register to count down
  GPTMTAMR_0 |= 0x2; // Puts in periodic timer mode
  GPTMTAILR_0 = 16000000;// Load the value 16,000,000 into the GPTMTAILR to achieve a 1 Hz blink rate using the 16 MHz oscillator
  GPTMCTL_0 |= 0x1;// Enable the timer using the GPTMCTL register
}

void led_init() {
  volatile unsigned short delay = 0;
  delay++; // Delay 2 more cycles before access Timer registers
  delay++; // Refer to Page. 756 of Datasheet for info

  GPIOAMSEL_E &= ~0x1F;          // disable analog function of PE0/1/2/3/4
  GPIOAFSEL_E &= ~0x1F;          // set PE/1/2/3/4 regular port function 
  GPIODIR_E |= 0x1C;             // set PE2/3/4 to output 
  GPIODIR_E &= ~0x3;             // set PE0/1 to input
  GPIODEN_E |= 0x1F;             // enable digital output on PE0/1/2/3/4
}


void polling_GPIO() {
  int counter = 0;
  while (1) {
    if (GPTMRIS_0 & 0x1) { // checking if flag is true
      GPTMICR_0 |= 0x1; // clearing flag
      if (counter == 0) {
          Red_on();
      } else if (counter == 1) {
          Red_off();
      } else if (counter == 2) {
          Yellow_on();
      } else if (counter == 3) {
          Yellow_off();
      } else if (counter == 4) {
          Green_on();
      } else if (counter == 5) {
          Green_off();
      }
      if (counter == 5) {
        counter = 0;
      } else {
        counter += 1;
      }
    }
  }
}


void Red_on(void) 
{ 
    GPIODATA_E |= 0x4; 
} 
 
void Red_off(void) 
{ 
    GPIODATA_E &= ~0x4; 
} 

void Yellow_on(void) 
{ 
    GPIODATA_E |= 0x8; 
    
} 

void Yellow_off(void) 
{ 
    GPIODATA_E &= ~0x8; 
} 

void Green_on(void) 
{ 
    GPIODATA_E |= 0x10; 
} 
 
void Green_off(void) 
{ 
    GPIODATA_E &= ~0x10; 
} 

bool system_button_pressed() {
  int count = 0;
  while (GPIODATA_E & 0x1) {
    if (GPTMRIS_0 & 0x1) {
        GPTMICR_0 |= 0x1;
        count +=1;
    }
    if (count == 2) {
        return true;
    }
  }
  return false;
}

bool pedestrian_button_pressed() {
  int count = 0;
  while (GPIODATA_E & 0x2) {
    if (GPTMRIS_0 & 0x1) {
        GPTMICR_0 |= 0x1;
        count +=1;
    }
    if (count == 2) {
        return true;
    }
  }
  return false;
}

void Traffic_Light_System()
{
  switch(TL_State) {   // State Transitions
     case TL_init: 
        bool flag = system_button_pressed();
        if (flag) {
          TL_State = TL_stop;
        }
       break;
     case TL_go:
        bool system_flag = system_button_pressed();
        bool pedestrian_flag = pedestrian_button_pressed();
        if (system_flag) {
           TL_State = TL_init;
        }
        else if (pedestrian_button_pressed) {
           TL_State = TL_warn;
        } else {
          TL_State = TL_stop;
        }
        break;
     case TL_warn:
        if (GPIODATA_E & 0x1) {
          TL_State = TL_init;
        }
        else {
          TL_State = TL_stop;
        }
        break;
     case TL_stop:
        if (GPIODATA_E & 0x1) {
           TL_State = TL_init;
        }
        else {
          TL_State = TL_go;
        }
        break;
     default:
        TL_State = TL_init;
        break;
  } // State Transitions

  switch(TL_State) {   // State actions
     case TL_go:
        Red_off();
        Yellow_off();
        Green_on();
        break;

     case TL_warn:
        Green_off();
        Red_off();
        Yellow_on();
        break;
     
     case TL_stop:
        Green_off();
        Yellow_off();
        Red_on();
        break;
     default:
        Red_off();
        Yellow_off();
        Green_off();
        break;
   } // State actions
   
}