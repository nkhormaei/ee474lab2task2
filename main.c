/* 
 * Sidharth Daga, Nick Khormaei
 * 1964629, 2033863
 * 1/24/23
 * 
 */ 
#include <stdint.h>
#include <stdbool.h>
#include "header_2.h" 

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
  volatile unsigned short delay = 0;
  delay++; // Delay 2 more cycles before access Timer registers
  delay++; // Refer to Page. 756 of Datasheet for info
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
  RCGCGPIO  |= 0xFFFF;
  GPIOAMSEL_E &= ~0x1F;          // disable analog function of PE0/1/2/3/4
  GPIOAFSEL_E &= ~0x1F;          // set PE/1/2/3/4 regular port function 
  GPIODIR_E |= 0x1C;             // set PE2/3/4 to output 
  GPIODIR_E &= ~0x3;             // set PE0/1 to input
  GPIODEN_E |= 0x1F;             // enable digital output on PE0/1/2/3/4
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

int five_seconds() {
  int count = 0;
  int sys = 0;
  int ped = 0;
  while (count < 5) {
    while (GPIODATA_E & 0x1) {
      if (GPTMRIS_0 & 0x1) {
        GPTMICR_0 |= 0x1;
        count +=1;
        sys +=1;
      }
      if (sys == 2) {
        return 1;
      }
    }
    while (GPIODATA_E & 0x2) {
      if (GPTMRIS_0 & 0x1) {
        GPTMICR_0 |= 0x1;
        count +=1;
        ped +=1;
      }
      if (ped == 2) {
        return 2;
      }
    }
    if (GPTMRIS_0 & 0x1) {
        GPTMICR_0 |= 0x1;
        count +=1;
    }
  }
  return 0; 
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
        int result_go = five_seconds();
        if (result_go == 1) {
           TL_State = TL_init;
        }
        else if (result_go == 2) {
           TL_State = TL_warn;
        } else {
          TL_State = TL_stop;
        }
        break;
     case TL_warn:
        int result_warn = five_seconds();
        if (result_warn == 1) {
          TL_State = TL_init;
        }
        else {
          TL_State = TL_stop;
        }
        break;
     case TL_stop:
       int result_stop = five_seconds();
        if (result_stop == 1) {
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