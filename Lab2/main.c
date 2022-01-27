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

#define SEN_PIN (11)
#define SEN_PIN_ID (QM_PIN_ID_11)
#define SEN_PIN_FN (QM_PIN_11_FN_GPIO_11)

//For red LED
#define RED_LED_PIN (16)
#define RED_LED_PIN_ID (QM_PIN_ID_16)
#define RED_LED_PIN_FN (QM_PIN_16_FN_GPIO_16)

//For green LED
#define GRN_LED_PIN (17)
#define GRN_LED_PIN_ID (QM_PIN_ID_17)
#define GRN_LED_PIN_FN (QM_PIN_16_FN_GPIO_17)

//Pin macros for the LED
#define YEL_LED_PIN (18)
#define YEL_LED_PIN_ID (QM_PIN_ID_18)
#define YEL_LED_PIN_FN (QM_PIN_18_FN_GPIO_18)

static void pin_mux_setup(void){
    qm_pmux_select(BTN_PIN_ID, BTN_PIN_FN);
    qm_pmux_select(YEL_LED_PIN_ID, YEL_LED_PIN_FN);
    qm_pmux_select(RED_LED_PIN_ID, RED_LED_PIN_FN);
    qm_pmux_select(GRN_LED_PIN_ID, GRN_LED_PIN_FN);
}

int main(void) {
    //structure to store settings for port configuration
    static qm_gpio_port_config_t cfg;
    static qm_gpio_state_t button_state;
    static qm_gpio_state_t sensor_state;
    
    //set up the pins
    pin_mux_setup();
    
    //storing bit pattern in bit direction register - written to hardware later
    //BTN_PIN is input, LED_PIN is output
    cfg.direction = ~BIT(BTN_PIN) | BIT(YEL_LED_PIN);
    cfg.direction = ~BIT(SEN_PIN) | BIT(RED_LED_PIN) | BIT(GRN_LED_PIN);
    
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
        
        //checks & saves sensor state
        qm_gpio_read_pin(QM_GPIO_0, SEN_PIN, &sensor_state);
        
        //if motion is detected
        if(sensor_state){
            qm_gpio_set_pin(QM_GPIO_0, RED_LED_PIN);
            qm_gpio_clear_pin(QM_GPIO_0, GRN_LED_PIN);
        }
        //if motion is not detected
        else {
            qm_gpio_clear_pin(QM_GPIO_0, RED_LED_PIN);
            qm_gpio_set_pin(QM_GPIO_0, GRN_LED_PIN);
        }
    }
    
    //Returns 0 if no errors
    return 0;
}
