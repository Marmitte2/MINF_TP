/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  Auteur : Loïc Marmy
  Date : 25.11.2026
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "Mc32DriverAdc.h"
#include "Mc32DriverLcd.h"
#include "bsp.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// définit le nombre de LEDs utilisées pour le chenillard
#define NB_LEDS     8

// tableau contenant les LEDs 0 à 7 du chenillard
const BSP_LED ledsTab[NB_LEDS] = {BSP_LED_0, BSP_LED_1, BSP_LED_2, BSP_LED_3, 
                                  BSP_LED_4, BSP_LED_5, BSP_LED_6, BSP_LED_7};

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    // Etat d'initialisation
    appData.state = APP_STATE_INIT;

    // Initialisation de l'affichage LCD
    lcd_init();
    // Initialisation de l'ADC
    BSP_InitADC10();
    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    // Variable d'entrée pour la première fois
    static bool firstTime = true;
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            // Allume le backlight
            lcd_bl_on();
            // Va à la ligne 1
            lcd_gotoxy(1, 1);
            // Ecrit sur le LCD
            printf_lcd("TP0 LED+AD <2026>");
            // Va a ligne 2
            lcd_gotoxy(1, 2);
            // Affiche sur le LCD
            printf_lcd("LMY");
            
            // Demarre le Timer 0
            DRV_TMR0_Start();
            // Allume toutes les LEDs
            LEDS_ON();
            // VA dans l'état d'attente
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }

        case APP_STATE_WAIT:
        {
            // Ne fait rien
            break;
        }
        case APP_STATE_SERVICE_TASKS:
        {
            // Si on rentre pour la première fois dans l'état
            if(firstTime)
            {
                // Eteint toutes les LEDs
                LEDS_OFF();
                // Remet la variable a false
                firstTime = false;
            }
            
            // Lit les valeur de l'ADC
            appData.adcRes = BSP_ReadAllADC();
            
            // Va a la ligne 3
            lcd_gotoxy(1, 3);
            // Ecrit sur le LCD
            printf_lcd("Ch0 %4d / Ch1 %4d", appData.adcRes.Chan0, appData.adcRes.Chan1);
            
            // Demarre le chenillard
            Chenillard();
            // Va a l'état d'attente
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
void APP_TMR0_Callback(void)
{
    // Va dans l'état de service
    APP_UpdateState(APP_STATE_SERVICE_TASKS);
}
 
void APP_UpdateState(APP_STATES newState)
{
    // Change d'état
    appData.state = newState;
}

void LEDS_ON(void)
{
    uint8_t i;
    
    // Boucle for pour afficher toutes les LEDs
    for(i = 0; i < NB_LEDS; i++)
    {
        BSP_LEDOn(ledsTab[i]);
    }
}

void LEDS_OFF(void)
{
    uint8_t i;
    
    // Boucle for pour éteindre toutes les LEDs
    for(i = 0; i < NB_LEDS; i++)
    {
        BSP_LEDOff(ledsTab[i]);
    }
}

void Chenillard(void)
{
    // Compteur pour atteindre 200ms
    static uint8_t compteur = 0;
    // Numero de la LED allumée
    static uint8_t led = 0;
    
    // Incrémente le compteur de temps
    compteur++;
    
    // Si 200ms sont atteintes
    if(compteur == 2)
    {
        // Remet le compteur a zéro
        compteur = 0;
        // Eteint la LED actuelle
        BSP_LEDOff(ledsTab[led]);
        
        // Incrémente la LED
        led++;
        
        // Si le compteur vaut 8 (plus grand que LED7)
        if(led == 8)
        {
            // remet le compteur a zéro
            led = 0;
        }
        // Allume la LED correspondante
        BSP_LEDOn(ledsTab[led]);
    }
}
/*******************************************************************************
 End of File
 */
