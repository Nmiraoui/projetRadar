#include "stm32f7xx.h"                  // Device header

extern void configuration_overclock(void);
extern int Init_GUIThread (void);
extern void Init_Ethernet (void);
extern void servo_set_angle(int deg);

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif       


/*----------------------------------------------------------------------------
 *      Tâche de test (vide) : à compléter par les étudiants
 *---------------------------------------------------------------------------*/
osThreadId_t tid_Test;

static void Thread_Test (void *argument)
{
	int deg;
	int Echo;
  (void)argument;

  // Initialisations propres à la tâche (si besoin) ici

  while (1)
  {
    // Votre boucle de test ici (mesure de distance, balayage du servo, etc.)
		//servo_set_angle(deg);
		//Echo = GPIOI->IDR | (1<<0);
    osDelay(100);   // période de la boucle : 100 ms
  }
}

static int Init_TestThread (void)
{
  tid_Test = osThreadNew(Thread_Test, NULL, NULL);
  if (tid_Test == NULL) {
    return -1;
  }
  return 0;
}

void servo_set_angle(int deg)
{
	
}


int main(void)
{
  configuration_overclock(); //pour pouvoir utiliser l'écran LCD
	/**********************************
	Début Initialisation périphérique
	**********************************/

	// Switch 
	RCC -> AHB1ENR |= (1<<8); //activation GPIO I
	GPIOI->MODER |= (1<<2); //set PI0 as an output	
	
	// Slider
	RCC->APB1ENR |= (1<<29);
	RCC->AHB1ENR |= (1<<0);
	
	GPIOA->MODER |= (3<<8); //PA4 output
	
	DAC->CR |= (1<<0);
	
	// Timer 50 Hz - Servo
	RCC-> AHB1ENR |= (1 << 5) ; // Active GPIOF
	RCC->AHB2ENR |= (1<<14); // enable syscfg
	//RCC->APB2ENR |= (1 << 17); // Activie TIM10
	
	//GPIOF-> MODER |= (2 << 12); //PF6 in AF mode
	//GPIOF-> AFR[0] |= (3 << 24); //PF6 in AF3 mode 
	
	//TIM10->CCMR1 |= (6 << 4); //TIM10_CH1 in PWM mode 1
	//TIM10-> CCER |= (1 << 0); //connects OC1 to TIM10_CH1
	
	//TIM10->PSC = 79; //prescaler
	//TIM10-> ARR = 49999; //periode
	//TIM10->CCR1 = 4000; // Duty cycle                                                                                                           
	
	//TIM10->CR1 |= (1 << 0); //starts counter
	
	// Potentiomètre 
	RCC->AHB2ENR |= (1<<10);
	GPIOF->MODER |= (3<<20); 
	
	ADC3->CR2 |= (1<<0);
	
	//Timer 1 Hz - Sensor
	// GPIOF activated
	RCC->APB2ENR |= (1 << 18); // Activie TIM11
	
	GPIOF-> MODER |= (2 << 14); //PF7 in AF mode
	GPIOF-> AFR[0] |= (3 << 28); //PF7 in AF3 mode 
	
	TIM11->CCMR1 |= (6 << 4); //TIM11_CH1 in PWM mode 1
	TIM11-> CCER |= (1 << 0); //connects OC1 to TIM10_CH1
	
	TIM11->PSC = 399; //prescaler
	TIM11-> ARR = 49999; //periode
	TIM11->CCR1 = 5; // Duty cycle
	
	TIM11->CR1 |= (1 << 0); //starts counter
	
	
	
	/**********************************
	Fin Initialisation périphérique
	**********************************/

	/**********************************
	Utilisation RTOS (Keil RTX5)
	**********************************/
	#ifdef RTE_CMSIS_RTOS2
	  /* Initialize CMSIS-RTOS2 */
	  osKernelInitialize ();

	  /* Create thread functions that start executing */
		Init_Ethernet();
		Init_GUIThread();
		Init_TestThread();   // tâche de test (boucle avec osDelay)

	  /* Start thread execution */
	  osKernelStart();
	#endif

  /* Infinite loop  (ne s'exécute pas avec le RTOS!!!)*/
  while (1)
  {
  }
}