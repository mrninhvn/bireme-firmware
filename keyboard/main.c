

#include <stdint.h>
#include <assert.h> 
#include "bireme.h"
#include "bireme_gzll_config.h"
#include "bat_level.h"
#include "nrf_gzll.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
// #include "nrf_drv_config.h"
#include "nrf_delay.h"

/*****************************************************************************/
/** Configuration */
/*****************************************************************************/

const nrf_drv_rtc_t rtc_maint = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
const nrf_drv_rtc_t rtc_deb = NRF_DRV_RTC_INSTANCE(1); /**< Declaring an instance of nrf_drv_rtc for RTC1. */


// Define payload length
#define TX_PAYLOAD_LENGTH ROWS ///< 5 byte payload length when transmitting

// Data and acknowledgement payloads
static uint8_t data_payload[TX_PAYLOAD_LENGTH];                ///< Payload to send to Host.
static uint8_t ack_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH]; ///< Placeholder for received ACK payloads from Host.

// Debounce time (dependent on tick frequency)
#define DEBOUNCE 5
#define ACTIVITY 500

// Key buffers
static uint8_t keys[ROWS], keys_snapshot[ROWS], keys_buffer[ROWS];
static uint32_t debounce_ticks, activity_ticks;
static volatile bool debouncing = false;

// Debug helper variables
static volatile bool init_ok, enable_ok, push_ok, pop_ok, tx_success;

// Setup switch pins with pullups
static void gpio_config(void)
{
    nrf_gpio_cfg_sense_input(C01, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_gpio_cfg_sense_input(C02, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_gpio_cfg_sense_input(C03, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_gpio_cfg_sense_input(C04, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_gpio_cfg_sense_input(C05, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_gpio_cfg_sense_input(C06, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_gpio_cfg_sense_input(C07, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);

    nrf_gpio_cfg_output(R01);
    nrf_gpio_cfg_output(R02);
    nrf_gpio_cfg_output(R03);
    nrf_gpio_cfg_output(R04);
    nrf_gpio_cfg_output(R05);
}

// Return the key states of one row
static uint8_t read_row(uint32_t row)
{
    uint8_t buff = 0;
    uint32_t input = 0;
    nrf_gpio_pin_set(row);
    input = NRF_GPIO->IN;
    buff = (buff << 1) | ((input >> C01) & 1);
    buff = (buff << 1) | ((input >> C02) & 1);
    buff = (buff << 1) | ((input >> C03) & 1);
    buff = (buff << 1) | ((input >> C04) & 1);
    buff = (buff << 1) | ((input >> C05) & 1);
    buff = (buff << 1) | ((input >> C06) & 1);
    buff = (buff << 1) | ((input >> C07) & 1);
    buff = (buff << 1);
    nrf_gpio_pin_clear(row);
    return buff;
}

// Return the key states
static void read_keys(void)
{
    keys_buffer[0] = read_row(R01);
    keys_buffer[1] = read_row(R02);
    keys_buffer[2] = read_row(R03);
    keys_buffer[3] = read_row(R04);
    keys_buffer[4] = read_row(R05);
    return;
}

static bool compare_keys(uint8_t* first, uint8_t* second, uint32_t size)
{
    for(int i=0; i < size; i++)
    {
        if (first[i] != second[i])
        {
          return false;
        }
    }
    return true;
}

static bool empty_keys(void)
{
    for(int i=0; i < ROWS; i++)
    {
        if (keys_buffer[i])
        {
          return false;
        }
    }
    return true;
}

// Assemble packet and send to receiver
static void send_data(void)
{
    for(int i=0; i < ROWS; i++)
    {
        data_payload[i] = keys[i];
    }
    nrf_gzll_add_packet_to_tx_fifo(PIPE_NUMBER, data_payload, TX_PAYLOAD_LENGTH);
}

// 8Hz held key maintenance, keeping the reciever keystates valid
static void handler_maintenance(nrf_drv_rtc_int_type_t int_type)
{
    send_data();
}

// 1000Hz debounce sampling
static void handler_debounce(nrf_drv_rtc_int_type_t int_type)
{
    read_keys();

    // debouncing, waits until there have been no transitions in 5ms (assuming five 1ms ticks)
    if (debouncing)
    {
        // if debouncing, check if current keystates equal to the snapshot
        if (compare_keys(keys_snapshot, keys_buffer, ROWS))
        {
            // DEBOUNCE ticks of stable sampling needed before sending data
            debounce_ticks++;
            if (debounce_ticks == DEBOUNCE)
            {
                for(int j=0; j < ROWS; j++)
                {
                    keys[j] = keys_snapshot[j];
                }
                send_data();
            }
        }
        else
        {
            // if keys change, start period again
            debouncing = false;
        }
    }
    else
    {
        // if the keystate is different from the last data
        // sent to the receiver, start debouncing
        if (!compare_keys(keys, keys_buffer, ROWS))
        {
            for(int k=0; k < ROWS; k++)
            {
                keys_snapshot[k] = keys_buffer[k];
            }
            debouncing = true;
            debounce_ticks = 0;
        }
    }

    // looking for 500 ticks of no keys pressed, to go back to deep sleep
    if (empty_keys())
    {
        activity_ticks++;
        if (activity_ticks > ACTIVITY)
        {
            nrf_drv_rtc_disable(&rtc_maint);
            nrf_drv_rtc_disable(&rtc_deb);
            nrf_gpio_pin_set(R01);
            nrf_gpio_pin_set(R02);
            nrf_gpio_pin_set(R03);
            nrf_gpio_pin_set(R04);
            nrf_gpio_pin_set(R05);
        }

    }
    else
    {
        activity_ticks = 0;
    }

}

// Low frequency clock configuration
static void lfclk_config(void)
{
    nrf_drv_clock_init();

    nrf_drv_clock_lfclk_request(NULL);
}

// RTC peripheral configuration
static void rtc_config(void)
{
    //Initialize RTC instance
    nrf_drv_rtc_init(&rtc_maint, NULL, handler_maintenance);
    nrf_drv_rtc_init(&rtc_deb, NULL, handler_debounce);

    //Enable tick event & interrupt
    nrf_drv_rtc_tick_enable(&rtc_maint,true);
    nrf_drv_rtc_tick_enable(&rtc_deb,true);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc_maint);
    nrf_drv_rtc_enable(&rtc_deb);
}

int main()
{
    // Initialize Gazell
    nrf_gzll_init(NRF_GZLL_MODE_DEVICE);

    // Attempt sending every packet up to 100 times
    nrf_gzll_set_max_tx_attempts(100);

    // Addressing
    nrf_gzll_set_base_address_0(0x01020304);
    nrf_gzll_set_base_address_1(0x05060708);

    // Enable Gazell to start sending over the air
    nrf_gzll_enable();

    // Configure 32kHz xtal oscillator
    lfclk_config();

    // Configure RTC peripherals with ticks
    rtc_config();

    // Configure all keys as inputs with pullups
    gpio_config();

    // Set the GPIOTE PORT event as interrupt source, and enable interrupts for GPIOTE
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    NVIC_EnableIRQ(GPIOTE_IRQn);


    // Main loop, constantly sleep, waiting for RTC and gpio IRQs
    while(1)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}

// This handler will be run after wakeup from system ON (GPIO wakeup)
void GPIOTE_IRQHandler(void)
{
    if(NRF_GPIOTE->EVENTS_PORT)
    {
        //clear wakeup event
        NRF_GPIOTE->EVENTS_PORT = 0;

        //enable rtc interupt triggers
        nrf_drv_rtc_enable(&rtc_maint);
        nrf_drv_rtc_enable(&rtc_deb);

        nrf_gpio_pin_clear(R01);
        nrf_gpio_pin_clear(R02);
        nrf_gpio_pin_clear(R03);
        nrf_gpio_pin_clear(R04);
        nrf_gpio_pin_clear(R05);

        //TODO: proper interrupt handling to avoid fake interrupts because of matrix scanning
        //debouncing = false;
        //debounce_ticks = 0;
        activity_ticks = 0;
    }
}



/*****************************************************************************/
/** Gazell callback function definitions  */
/*****************************************************************************/

void  nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    uint32_t ack_payload_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    if (tx_info.payload_received_in_ack)
    {
        // Pop packet and write first byte of the payload to the GPIO port.
        nrf_gzll_fetch_packet_from_rx_fifo(pipe, ack_payload, &ack_payload_length);
    }
}

// no action is taken when a packet fails to send, this might need to change
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{

}

// Callbacks not needed
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{}
void nrf_gzll_disabled()
{}