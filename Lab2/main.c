//Lab 2 Code, sent to GitHub to be saved and re-used
//Intended for Intel ISSM w/ Eclipse

//Files necessary for ISSM in Eclipse
#include "qm_gpio.h"
#include "qm_pinmux.h"
#include "qm_pin_functions.h"

//Constants for compiler, pin definitions
#define BTN_PIN (9)
#define BTN_PIN_ID (QM_PIN_ID_9)
#define BTN_PIN_FN (QM_PIN_9_FN_GPIO_9)

//Pin macros for the LED
#define YEL_LED_PIN (18)
#define YEL_LED_PIN_ID (QM_PIN_ID_18)
#define YEL_LED_PIN_FN (QM_PIN_18_FN_GPIO_18)

static void pin_mux_setup(void){
    qm_pmux_select(BTN_PIN_ID, BTN_PIN_FN);
    qm_pmux_select(YEL_LED_PIN_ID, YEL_LED_PIN_FN);
}

int main(void) {
    //structure to store settings for port configuration
    static qm_gpio_port_config_t cfg;
    static qm_gpio_state_t button_state;
    
    //set up the pins
    pin_mux_setup();
    
    //storing bit pattern in bit direction register - written to hardware later
    //BTN_PIN is input, LED_PIN is output
    cfg.direction = ~BIT(BTN_PIN) | BIT(YEL_LED_PIN);
    
    //Looks for GPIO and a pointer to config settings - passes to hardware
    qm_gpio_set_config(QM_GPIO_0, &cfg);
    
    //Polling - checks state and determines next action. Looks at button state and either sets or removes yellow LED
    while(1){
        //Reads value of button & stores it
        qm_gpio_read_pin(QM_GPIO_0, BTN_PIN, &button_state);
        
        //If button has been pressed
        if(button_state){
            //Turns yellow LED on
            qm_gpio_set_pin(QM_GPIO_0, YEL_LED_PIN)
        }
        //If button has not been pressed
        else{
            //Turns yellow LED off
            qm_gpio_clear_pin(QM_GPIO_0, YEL_LED_PIN);
        }
    }
    
    
    
    
    
    
    return 0;
}
