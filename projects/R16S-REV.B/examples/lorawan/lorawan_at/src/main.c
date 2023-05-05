#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "timer.h"
#include "radio.h"
#include "log.h"
#include "tremo_uart.h"
#include "tremo_lpuart.h"
#include "tremo_gpio.h"
#include "tremo_rcc.h"
#include "tremo_delay.h"
#include "tremo_pwr.h"
#include "rtc-board.h"
#include "linkwan_ica_at.h"
#include "linkwan.h"
#include "lwan_config.h"

#define LORAWAN_APP_PORT 100

void LoraTxData(lora_AppData_t* AppData);
void LoraRxData(lora_AppData_t* AppData);
uint8_t BoardGetBatteryLevel();
void BoardGetUniqueId(uint8_t* id);
uint32_t BoardGetRandomSeed();

log_level_t g_log_level = LL_ERR | LL_WARN | LL_DEBUG;
static LoRaMainCallback_t LoRaMainCallbacks
    = { BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed, LoraTxData, LoraRxData };

void uart_log_init(uint32_t baudrate)
{

    uart_config_t uart_config;

    // set iomux
    gpio_set_iomux(GPIOB, GPIO_PIN_0, 1); // UART0_RX:GP16
    gpio_set_iomux(GPIOB, GPIO_PIN_1, 1); // UART0_TX:GP17
    gpio_init(GPIOB, GPIO_PIN_0, GPIO_MODE_INPUT_PULL_UP);
    gpio_init(GPIOB, GPIO_PIN_1, GPIO_MODE_OUTPUT_PP_HIGH);

    // uart init
    uart_config_init(&uart_config);
    uart_config.fifo_mode = ENABLE;
    uart_config.mode      = UART_MODE_TXRX;
    uart_config.baudrate  = baudrate;
    uart_init(CONFIG_DEBUG_UART, &uart_config);

    uart_set_rx_fifo_threshold(CONFIG_DEBUG_UART, UART_RX_FIFO_LEVEL_1_2);
    uart_cmd(CONFIG_DEBUG_UART, ENABLE);
    uart_config_interrupt(CONFIG_DEBUG_UART, UART_INTERRUPT_RX_DONE | UART_INTERRUPT_RX_TIMEOUT, ENABLE);

    NVIC_SetPriority(UART0_IRQn, 2);
    NVIC_EnableIRQ(UART0_IRQn);
}

void board_init()
{
    rcc_enable_oscillator(RCC_OSC_XO32K, true);

    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOA, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOB, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOD, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_LPUART, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_UART0, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_PWR, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_RTC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_SAC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_LORA, true);
    #ifdef PRINT_BY_DMA
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_SYSCFG, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_DMA0, true);
    #endif

    //delay_ms(100);
    //pwr_xo32k_lpm_cmd(true);

    RtcInit();
}

void* _sbrk(int nbytes)
{
    // variables from linker script
    extern char _heap_bottom[];
    extern char _heap_top[];

    static char* heap_ptr = _heap_bottom;

    if ((unsigned int)(heap_ptr + nbytes) <= (unsigned int)_heap_top) {
        void* base = heap_ptr;
        heap_ptr += nbytes;
        return base;
    } else {
        return (void*)(-1);
    }
}

uint8_t BoardGetBatteryLevel()
{
    return 0;
}

void BoardGetUniqueId(uint8_t* id)
{
}

uint32_t BoardGetRandomSeed()
{
    return 0;
}

void LoraTxData(lora_AppData_t* AppData)
{
    AppData->BuffSize = sprintf((char*)AppData->Buff, "linklora asr data++");
    LOG_PRINTF(LL_DEBUG, "tx: %s\r\n", AppData->Buff);
    AppData->Port = LORAWAN_APP_PORT;
}

void LoraRxData(lora_AppData_t* AppData)
{
    AppData->Buff[AppData->BuffSize] = '\0';
    LOG_PRINTF(LL_DEBUG, "rx: port = %d, len = %d\r\n", AppData->Port, AppData->BuffSize);
    int i;
    for (i = 0; i < AppData->BuffSize; i++) {
        LOG_PRINTF(LL_DEBUG, "0x%x ", AppData->Buff[i]);
    }
    LOG_PRINTF(LL_DEBUG, "\r\n");
}

int main(void)
{
    LWanSysConfig_t default_sys_config = LWAN_SYS_CONFIG_DEFAULT;
    uint32_t baudrate;
    uint16_t log_level;

    // Target board initialization
    board_init();

    /* UART0 is used as AT and log port, UART0 does not support wake up the MCU
     * from low power mode, so we block the the low power here */
    TimerLowPowerBlock(0x01);

    lwan_sys_config_init(&default_sys_config);
    lwan_sys_config_get(SYS_CONFIG_BAUDRATE, &baudrate);
    uart_log_init(baudrate);

    lwan_sys_config_get(SYS_CONFIG_LOGLVL, &log_level);
    log_set_level((1<<log_level)-1);

    lora_init(&LoRaMainCallbacks);
    lora_fsm();
}

#ifdef USE_FULL_ASSERT
void assert_failed(void* file, uint32_t line)
{
    (void)file;
    (void)line;

    while (1) { }
}
#endif
