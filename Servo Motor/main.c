#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include <stdint.h>

#define OUT		  		(*((volatile unsigned long *)0x40005100))
#define SW1		(*((volatile unsigned long *) 0x40025040))
#define SW2		(*((volatile unsigned long *) 0x40025004))


//   Function Prototypes
void PORTF_Init(void);
void PORTB_Init(void);
void PWM0MA_Init(uint16_t);

// 3. Subroutines Section
int main(void){    
  PLL_Init();
  SysTick_Init();                  // Enable SysTick
  PORTB_Init();                    // initialize motor outputs on Port B
  PORTF_Init();                    // initialize sensor inputs on Port F	
	
  while(1){
	if(SW1 == 0x00 && SW2 == 0x00)
	{
		SysTick_Wait10ms(1);
		PWM0MA_Init(667);
	}
	else if(SW1 != 0x00 && SW2 == 0x00)
	{
		SysTick_Wait10ms(1);
		PWM0MA_Init(667);
	}
	else if(SW1 == 0x00 && SW2 != 0x00)
	{
		SysTick_Wait10ms(1);
		PWM0MA_Init(667);
	}
	else
	{
		SysTick_Wait10ms(1);
		PWM0MA_Init(667);
		SysTick_Wait10ms(1);
		PWM0MA_Init(500);	
	}
	
  }
}

void PORTB_Init()
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_AMSEL_R &= 0x00;
	GPIO_PORTB_PCTL_R &= 0x00;
	GPIO_PORTB_DIR_R = 0x0F;
	GPIO_PORTB_AFSEL_R &= 0x00;
	GPIO_PORTB_DR8R_R = 0xFF;
	GPIO_PORTB_DEN_R |= 0x0F;
}

void PORTF_Init()
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x1F;
	GPIO_PORTF_AMSEL_R &= 0x00;
	GPIO_PORTF_PCTL_R &= 0x00;
	GPIO_PORTF_DIR_R = 0x0E;
	GPIO_PORTF_AFSEL_R &= 0x00;
	GPIO_PORTF_PUR_R = 0x11;
	GPIO_PORTF_DEN_R |= 0x1F;
}
	void PWM0A_Init(uint16_t duty)
{
	volatile unsigned long delay;
	SYSCTL_RCGC0_R |= 0x00100000;  // enable PWM clock 
	
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  
	delay = SYSCTL_RCGC2_R;  // allow time for clock to start
 
	GPIO_PORTB_AFSEL_R |= 0x40; // Enable PWM on PB6 regular port function
	
	GPIO_PORTB_PCTL_R &= 0x10111111;  // clearing PCTL of PB6 before setting them 
	GPIO_PORTB_PCTL_R |= 0x04000000; // 4) PCTL for PB6 on PortB, see datasheet page 688 and 1351
	
	GPIO_PORTB_AMSEL_R &= ~0x40; // disable analog functionality on PB6
  
	//GPIO_PORTB_DIR_R = 0x40;        // direction NOT NEEDED
	//GPIO_PORTB_DEN_R |= 0x40;         // enable 
	
	GPIO_PORTB_DEN_R |= 0x40;  // enable digital I/O on PB6
	
	SYSCTL_RCC_R &= ~(0x000E0000); // clearing first
	SYSCTL_RCC_R |= (0x00080000); // the writing in the 19:17 with PWMDIV 0x3 configure for /32 divider
	
	SYSCTL_RCC_R |= 0x00100000;  // 3) use PWM divider   

	//SYSCTL_RCC_R = 0x00400000 |(SYSCTL_RCC_R & (~0x000E0000));  // 3) use PWM divider   configure for /2 divider

	PWM0_0_CTL_R = 0x00000000;  // 4) re-loading down-counting mode
  
	PWM0_0_GENA_R= 0xC8;// PB6 goes low on LOAD, PB6 goes high on CMPA down
	
	PWM0_0_LOAD_R = 10000 - 1;
	
	PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
	
	PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
	
	PWM0_ENABLE_R |= 0x00000001;          // enable PB6/M0PWM0
	
	
}
