#include <stdint.h> 
#include "lab2_task1.h" 

/*
This part of the lab (task 1) had 2 main goals- 1) turn on the lights in a specific pattern
depending on the timers instead of delays
*/


int main(void)
{
  lights();
  timer_initc();
  polling_GPIO();
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


void lights() {
    volatile unsigned short delay = 0; 
    RCGCGPIO |= 0xFFFF;  // Enable Ports
    delay++;           // Delay 2 more cycles before access Timer registers 
    delay++;           // Refer to Page. 756 of Datasheet for info 
    
    GPIODIR_F = 0x11;   // Set PF0 to output 
    GPIODEN_F = 0x11;   // Set PF0 to digital port 
    
    GPIODIR_N = 0x3; // setting bits for the N ports
    GPIODEN_N = 0x3; 

}


void polling_GPIO() {
  int counter = 0;
  while (1) {
    if (GPTMRIS_0 & 0x1) { // checking if flag is true
      GPTMICR_0 |= 0x1; // clearing flag
      if (counter == 0) {
          GPIODATA_N |= 0x1;
      } else if (counter == 1) {
          GPIODATA_N |= 0x2;
      } else if (counter == 2) {
          GPIODATA_F |= 0x1;
      } else if (counter == 3) {
          GPIODATA_F |= 0x2;
      } else if (counter == 4) {
          GPIODATA_N &= ~0x1;
      } else if (counter == 5) {
          GPIODATA_N &= ~0x2;
      } else if (counter == 6) {
          GPIODATA_F &= ~0x1;
      } else if (counter == 7) {
          GPIODATA_F &= ~0x2;
      }
      if (counter == 7) {
        counter = 0;
      } else {
        counter += 1;
      }
    }
  }
}