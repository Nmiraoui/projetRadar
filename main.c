#include "stm32f7xx.h"                  // Device header

extern void configuration_overclock(void);
extern int Init_GUIThread (void);
extern void Init_Ethernet (void);

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
  (void)argument;

  // Initialisations propres à la tâche (si besoin) ici

  while (1)
  {
    // Votre boucle de test ici (mesure de distance, balayage du servo, etc.)

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
	
	GPIOA->MODER |= (3<<8);
	
	DAC->CR |= (1<<0);
	
	// Gauge
	RCC-> AHB1ENR |= (1 << 5) ;
	RCC->APB2ENR |= (1 << 17);
	
	GPIOF-> MODER |= (2 << 12);
	GPIOF-> AFR[0] |= (3 << 24);
	
	TIM10->CCMR1 |= (6 << 4) ;
	TIM10-> CCER |= (1 << 0);
	
	TIM10->PSC = 7;
	TIM10-> ARR = 39999;
	
	TIM10->CR1 |= (1 << 0);
	
	// Potentiomètre 
	RCC->AHB2ENR |= (1<<10);
	GPIOF->MODER |= (3<<20);
	
	ADC3->CR2 |= (1<<0);
	
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