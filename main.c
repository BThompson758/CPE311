//Lab 3 Code, sent to GitHub to be saved and re-used
//Intended for Intel ISSM w/ Eclipse

//Files necessary for ISSM in Eclipse
#include "qm_gpio.h"
#include "qm_pinmux.h"
#include "qm_pin_functions.h"
#include "qm_adc.h"
#include "clk.h"

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
#define GRN_LED_PIN_FN (QM_PIN_17_FN_GPIO_17)

//Pin macros for the LED
#define YEL_LED_PIN (18)
#define YEL_LED_PIN_ID (QM_PIN_ID_18)
#define YEL_LED_PIN_FN (QM_PIN_18_FN_GPIO_18)


#define ADC_BITS (4096)
#define SENS_OUT_V (5000)
#define ZERO_OFFSET (500)
#define TEMP_COEFF (10)


//Read digitized value & convert to Celcius


static void pin_mux_setup(void){
    qm_pmux_select(BTN_PIN_ID, BTN_PIN_FN);
    qm_pmux_select(YEL_LED_PIN_ID, YEL_LED_PIN_FN);
    qm_pmux_select(RED_LED_PIN_ID, RED_LED_PIN_FN);
    qm_pmux_select(GRN_LED_PIN_ID, GRN_LED_PIN_FN);

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
    static qm_gpio_port_config_t cfg;
    static qm_gpio_state_t button_state;
    static qm_gpio_state_t sensor_state;

    static qm_adc_config_t adc_cfg;
    static qm_adc_xfer_t xfer;
    static qm_adc_channel_t chan = QM_ADC_CH_3;
    static uint16_t sample_buffer = 0;
    static int32_t temperature = 0;

    //set up the pins
    pin_mux_setup();

    //storing bit pattern in bit direction register - written to hardware later
    //BTN_PIN is input, LED_PIN is output
    cfg.direction = ~BIT(BTN_PIN) | BIT(YEL_LED_PIN);
    cfg.direction = ~BIT(SEN_PIN) | BIT(RED_LED_PIN) | BIT(GRN_LED_PIN);

    //Looks for GPIO and a pointer to config settings - passes to hardware
    qm_gpio_set_config(QM_GPIO_0, &cfg);

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
        qm_gpio_read_pin(QM_GPIO_0, BTN_PIN, &button_state);

        //If button has been pressed
        if(button_state){
            //Turns yellow LED on
            qm_gpio_set_pin(QM_GPIO_0, YEL_LED_PIN);
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

        qm_adc_convert(QM_ADC_0, &xfer, NULL);
        clk_sys_udelay(250000);

        temperature = ((sample_buffer * (SENS_OUT_V / ADC_BITS)) - ZERO_OFFSET) / TEMP_COEFF;
        QM_PRINTF("%d", temperature);
        QM_PRINTF("\t\r");
    }

    //Returns 0 if no errors
    return 0;
}
