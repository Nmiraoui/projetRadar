#include "stm32f7xx.h"                  // Device header

extern void configuration_overclock(void);
extern int Init_GUIThread (void);
extern void Init_Ethernet (void);
extern void servo_set_angle(int deg);

volatile uint32_t mesure_us = 0;
volatile uint8_t  nouvelle_mesure = 0;
volatile float     DIST = -1.0f;

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
	if(deg<0) deg = 0;
	if(deg>180) deg=180;
	
	uint32_t ccr_val =2000+((uint32_t)deg*4000)/180;
	
	TIM10->CCR1 = ccr_val;
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & (1 << 6))               // ligne 6 (PG6 = Echo)
	{
		EXTI->PR = (1 << 6);               // acquitte le flag (ecrire 1)

		if (GPIOG->IDR & (1 << 6))         // front montant : Echo = 1
		{
			GPIOI->BSRR = (1 << 1);        // LED1 (PI1) allumee
			TIM2->CR1 &= ~(1 << 0);
			TIM2->CNT = 0;
			TIM2->CR1 |= (1 << 0);         // demarre le compteur
		}
		else                               // front descendant : Echo = 0
		{
			GPIOI->BSRR = (1 << (1 + 16)); // LED1 eteinte
			TIM2->CR1 &= ~(1 << 0);        // arrete le compteur
			mesure_us = TIM2->CNT;
			DIST = mesure_us * 17.0f / 1000.0f;  // formule HC-SR04 (cm)
			nouvelle_mesure = 1;
		}
	}
}

float distance_cm(void)
{
    return DIST;
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
	RCC->APB2ENR |= (1 << 17); // Activie TIM10
	
	GPIOF-> MODER |= (2 << 12); //PF6 in AF mode
	GPIOF-> AFR[0] |= (3 << 24); //PF6 in AF3 mode 
	
	TIM10->CCMR1 |= (6 << 4); //TIM10_CH1 in PWM mode 1
	TIM10-> CCER |= (1 << 0); //connects OC1 to TIM10_CH1
	
	TIM10->PSC = 79; //prescaler
	TIM10-> ARR = 49999; //periode
	TIM10->CCR1 = 4000; // Duty cycle                                                                                                           
	
	TIM10->CR1 |= (1 << 0); //starts counter
	
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
	TIM11->CCR1 = 8; // Duty cycle
	
	TIM11->CR1 |= (1 << 0); //starts counter

	// Interruption Echo (HC-SR04) sur PG6
	RCC->AHB1ENR |= (1 << 6);          // GPIOG
	RCC->APB2ENR |= (1 << 14);         // SYSCFG
	RCC->APB1ENR |= (1 << 0);          // TIM2

	GPIOG->MODER &= ~(3 << 12);        // PG6 en entree
	GPIOG->PUPDR &= ~(3 << 12);
	GPIOG->PUPDR |=  (2 << 12);        // pull-down

	SYSCFG->EXTICR[1] &= ~(0xF << 8);
	SYSCFG->EXTICR[1] |=  (6 << 8);    // EXTI6 -> port G

	EXTI->RTSR |= (1 << 6);
	EXTI->FTSR |= (1 << 6);
	EXTI->IMR  |= (1 << 6);

	TIM2->PSC = 99;                    // APB1 : 100 MHz / 100 = 1 us par tick
	TIM2->ARR = 0xFFFFFFFF;
	TIM2->EGR = 1;                     // charge le prescaler
	TIM2->CNT = 0;                     // le timer reste arrete (demarre dans l'interruption)

	NVIC_SetPriority(EXTI9_5_IRQn, 5);
	NVIC_EnableIRQ(EXTI9_5_IRQn);

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