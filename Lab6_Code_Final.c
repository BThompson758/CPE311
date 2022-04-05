//Lab 3 Code, sent to GitHub to be saved and re-used
//Intended for Intel ISSM w/ Eclipse

//Files necessary for ISSM in Eclipse
#include "qm_gpio.h"
#include "qm_pinmux.h"
#include "qm_pin_functions.h"
#include "qm_adc.h"
#include "clk.h"

//Constants for compiler, pin definitions
//#define BTN_PIN (9)
//#define BTN_PIN_ID (QM_PIN_ID_9)
//#define BTN_PIN_FN (QM_PIN_9_FN_GPIO_9)

//#define SEN_PIN (11)
//#define SEN_PIN_ID (QM_PIN_ID_11)
//#define SEN_PIN_FN (QM_PIN_11_FN_GPIO_11)

/*
//For red LED
#define RED_LED_PIN (16)
#define RED_LED_PIN_ID (QM_PIN_ID_16)
#define RED_LED_PIN_FN (QM_PIN_16_FN_GPIO_16)
//For green LED
#define GRN_LED_PIN (17)
#define GRN_LED_PIN_ID (QM_PIN_ID_17)
#define GRN_LED_PIN_FN (QM_PIN_17_FN_GPIO_17)
//Pin macros for the LED
#define YEL_LED_PIN (18)
#define YEL_LED_PIN_ID (QM_PIN_ID_18)
#define YEL_LED_PIN_FN (QM_PIN_18_FN_GPIO_18)
*/

#define ADC_BITS (4096)
#define SENS_OUT_V (5000)
#define ZERO_OFFSET (500)
#define TEMP_COEFF (10)


uint32_t button_state;
uint32_t sensor_state;

//initializes variables to GPIOs
uint32_t RED_LED_PIN = 0x10000;
uint32_t GRN_LED_PIN = 0x20000;
uint32_t YEL_LED_PIN = 0x40000;
uint32_t BTN_PIN = 0x0200;
uint32_t SEN_PIN = 0x0800;

//Initializes variables to memory addresses
uint32_t PMUX_SEL0 = 0xB0800930;
uint32_t DR = 0xB0000C00;
uint32_t DDR = 0xB0000C04;
uint32_t EXT_DR = 0xB0000C50;

static void pin_mux_setup(void){


    //Set all pins to function 0

    //Moves 0 into the PMUX_SEL0 and PMUX_SEL0+4 addressess corresponding to the button and sensor states
    __asm__(".intel_syntax noprefix \n\t"

            "mov edx, 0x00000000 \n\t"
            "mov edi, PMUX_SEL0 \n\t"
            "mov [edi], edx \n\t"
            "mov [edi+4], edx \n\t"

            ".att_syntax \n\t");
    /*
    qm_pmux_select(BTN_PIN_ID, BTN_PIN_FN);
    qm_pmux_select(YEL_LED_PIN_ID, YEL_LED_PIN_FN);
    qm_pmux_select(RED_LED_PIN_ID, RED_LED_PIN_FN);
    qm_pmux_select(GRN_LED_PIN_ID, GRN_LED_PIN_FN);
    */
    
    qm_pmux_select(QM_PIN_ID_3, QM_PIN_3_FN_AIN_3);
    qm_pmux_input_en(QM_PIN_ID_3, true);
}


//setup_adc
static void setup_adc(void){
    clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_ADC | CLK_PERIPH_ADC_REGISTER);
    clk_adc_set_div(100);
    qm_adc_set_mode(QM_ADC_0, QM_ADC_MODE_NORM_CAL);
    qm_adc_calibrate(QM_ADC_0);
}

int main(void) {
    
    //structure to store settings for port configuration
    //static qm_gpio_port_config_t cfg;
    //static qm_gpio_state_t button_state;
    //static qm_gpio_state_t sensor_state;

    static qm_adc_config_t adc_cfg;
    static qm_adc_xfer_t xfer;
    static qm_adc_channel_t chan = QM_ADC_CH_3;
    static uint16_t sample_buffer = 0;
    static int32_t temperature = 0;

    //set up the pins
    pin_mux_setup();

    //storing bit pattern in bit direction register - written to hardware later
    //BTN_PIN is input, LED_PIN is output
    //cfg.direction = 0;
    //cfg.direction = BIT(YEL_LED_PIN) | BIT(RED_LED_PIN) | BIT(GRN_LED_PIN);

    //Looks for GPIO and a pointer to config settings - passes to hardware
    //qm_gpio_set_config(QM_GPIO_0, &cfg);

    __asm__(".intel_syntax noprefix \n\t"
            //Sets pins D11, D12, D13 (GPIOs 16, 17, and 18)
            //Change bits 17, 18, and 19 & or them
            //0001 0000 0000 0000 0000 OR 0010 0000 0000 0000 0000 OR 0100 0000 0000 0000 0000
            "mov edx, RED_LED_PIN \n\t"
            "or edx, GRN_LED_PIN \n\t"
            "or edx, YEL_LED_PIN \n\t"
            "mov edi, DDR \n\t"
            "mov [edi], edx \n\t"

            ".att_syntax \n\t");

    setup_adc();

    adc_cfg.window = 20;
    adc_cfg.resolution = QM_ADC_RES_12_BITS;
    qm_adc_set_config(QM_ADC_0, &adc_cfg);

    xfer.ch = &chan;
    xfer.ch_len = 1;
    xfer.samples = &sample_buffer;
    xfer.samples_len = 1;

    QM_PUTS("Displaying temperature in Celsius");

    //Polling - checks state and determines next action. Looks at button state and either sets or removes yellow LED
    while(1){
        //Reads value of button & stores it
        //qm_gpio_read_pin(QM_GPIO_0, BTN_PIN, &button_state);

        /*
         Loads button_state into edi and ands the EXT_DR address with the BTN_PIN value
         Moves the result of the 'and' operation stored in edx to the address at edi (button_state)
         */
        __asm__(".intel_syntax noprefix \n\t"
                "lea edi, button_state \n\t "
                "mov edx, EXT_DR \n\t"
                "and edx, BTN_PIN \n\t"
                "mov [edi], edx \n\t"
                ".att_syntax \n\t");



        //If button has been pressed
        if(button_state){
            //qm_gpio_set_pin(QM_GPIO_0, YEL_LED_PIN);
            //Sets pin D12, GPIO 18, 0100 0000 0000 0000 0000 = 0x40000

            /*
             Moves the address of DR into edx, and 'ORs' it with the YEL_LED_PIN GPIO, storing it in edx
             Moves DR into edi
             Moves the result of the 'or' operation stored in edx into the address of edi (DR)
             Turns yellow LED on
             */
            __asm__(".intel_syntax noprefix \n\t"
                    "mov edx, DR \n\t"
                    "or  edx, YEL_LED_PIN \n\t"
                    "mov edi, DR \n\t"
                    "mov [edi], edx \n\t"
                    ".att_syntax \n\t");
        }
        //If button has not been pressed
        else{
            
            /*
             Moves DR into edx and the GPIO for YEL_LED_PIN into ebx
             Flips bits for YEL_LED_PIN GPIO and 'ANDs' it with DR, stores it in edx - zeroes out values
             Moves DR memory address into edi
             Moves the edx into the address at edi (DR)
             Turns yellow LED off
             */
            __asm__(".intel_syntax noprefix \n\t"
                    "mov edx, DR \n\t"
                    "mov ebx, YEL_LED_PIN \n\t"
                    "not ebx \n\t"
                    "and edx, ebx \n\t"
                    "mov edi, DR \n\t"
                    "mov [edi], edx \n\t"
                    ".att_syntax \n\t");
        }

        //checks & saves sensor state
        //qm_gpio_read_pin(QM_GPIO_0, SEN_PIN, &sensor_state);

        /*
         Loads sensor_state into edi and ands the EXT_DR address with the SEN_PIN value
         Moves the result of the 'and' operation stored in edx to the address at edi (sensor_state)
         */
        __asm__(".intel_syntax noprefix \n\t"
                        "lea edi, sensor_state \n\t "
                        "mov edx, EXT_DR \n\t"
                        "and edx, SEN_PIN \n\t"
                        "mov [edi], edx \n\t"
                        ".att_syntax \n\t");


        //if motion is detected
        if(sensor_state){
            //qm_gpio_set_pin(QM_GPIO_0, RED_LED_PIN);
            //qm_gpio_clear_pin(QM_GPIO_0, GRN_LED_PIN);
            //turn red pin D13 GPIO16 on and the green pin D11 GPIO17 off

            /*
             Moves the memory address DR into edi and the GRN_LED_PIN GPIO into ebx
             Flips bits for GRN_LED_PIN GPIO and 'ANDs' it with current sensor_state value, stores result in edx (zeroes out values)
             Moves the memory address DR into edi and moves the value at edx into the address at edi (DR)
             Turns green LED off
             */
            __asm__(".intel_syntax noprefix \n\t"
                                "mov edi, DR \n\t"
                                "mov ebx, GRN_LED_PIN \n\t"
                                "not ebx \n\t"
                                "and edx, ebx \n\t"
                                "mov edi, DR \n\t"
                                "mov [edi], edx \n\t"
                                ".att_syntax \n\t");


            /*
             Moves the memory address DR into edi
             'ORs' the current edx value (sensor_state) with the RED_LED_PIN GPIO and stores the result in edx (Only RED_LED_PIN GPIO should be active)
             Moves the memory address DR into edi
             Moves edx (only RED_LED_PIN GPIO) into the memory addresss at edi (DR)
             Turns red LED on
             */
            __asm__(".intel_syntax noprefix \n\t"
                                "mov edi, DR \n\t"
                                "or  edx, RED_LED_PIN \n\t"
                                "mov edi, DR \n\t"
                                "mov [edi], edx \n\t"
                                ".att_syntax \n\t");
        }
        
        //if motion is not detected
        else {
            //qm_gpio_clear_pin(QM_GPIO_0, RED_LED_PIN);
            //qm_gpio_set_pin(QM_GPIO_0, GRN_LED_PIN);

            //turn green pin D11 GPIO17 on and red pin D13 GPIO16 off
            
            /*
             Moves the memory address DR into edi
             'ORs' the current edx value (sensor_state) with the GRN_LED_PIN GPIO and stores the result in edx (Only GRN_LED_PIN GPIO should be active)
             Moves the memory address DR into edi
             Moves edx (only GRN_LED_PIN GPIO) into the memory addresss at edi (DR)
             Turns green LED on
             */
            __asm__(".intel_syntax noprefix \n\t"
                                            "mov edi, DR \n\t"
                                            "or  edx, GRN_LED_PIN \n\t"
                                            "mov edi, DR \n\t"
                                            "mov [edi], edx \n\t"
                                            ".att_syntax \n\t");

            /*
             Moves the memory address DR into edi and the RED_LED_PIN GPIO into ebx
             Flips bits for RED_LED_PIN GPIO and 'ANDs' it with current sensor_state value, stores result in edx (zeroes out values)
             Moves the memory address DR into edi and moves the value at edx into the address at edi (DR)
             Turns red LED off
             */
            __asm__(".intel_syntax noprefix \n\t"
                                            "mov edi, DR \n\t"
                                            "mov ebx, RED_LED_PIN \n\t"
                                            "not ebx \n\t"
                                            "and edx, ebx \n\t"
                                            "mov edi, DR \n\t"
                                            "mov [edi], edx \n\t"
                                            ".att_syntax \n\t");
        }

        qm_adc_convert(QM_ADC_0, &xfer, NULL);
        clk_sys_udelay(250000);

        temperature = ((sample_buffer * (SENS_OUT_V / ADC_BITS)) - ZERO_OFFSET) / TEMP_COEFF;
        QM_PRINTF("%d", temperature);
        QM_PRINTF("\t\r");
    }

    //Returns 0 if no errors
    return 0;
}
