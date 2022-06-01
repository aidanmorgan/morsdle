#include "stm32l4xx_hal_conf.h"
#include "morse.h"
#include "cbuff.h"
#include "morsdle.h"
#include "renderer.h"
#include "waveshare_display.h"
#include "waveshare_stm.h"

static uint8_t imagebuffer[448 * 600];

static morse_t h_morse = &(struct morse) {};
static morsdle_game_t *h_game = &(morsdle_game_t) {.word = "ratio"};

static renderer_t h_renderer = &(renderer) {
        .game_mode = MORSDLE_GAME_SINGLE_LETTER
};

wavesharestm_pin_t WAVESHARESTM_BUSY_PIN = (wavesharestm_pin_t) {};
wavesharestm_pin_t WAVESHARESTM_CS_PIN = (wavesharestm_pin_t) {};
wavesharestm_pin_t WAVESHARESTM_DC_PIN = (wavesharestm_pin_t) {};
wavesharestm_pin_t WAVESHARESTM_RST_PIN = (wavesharestm_pin_t) {};

SPI_HandleTypeDef* WAVESHARE_SPI_HANDLE = (SPI_HandleTypeDef)NULL;
UART_HandleTypeDef* WAVESHARE_UART_HANDLE = (UART_HandleTypeDef)NULL;


// this is the waveshare implementation of the handle
static display_impl_t display_impl = (display_impl_t) {
        .buffer = imagebuffer,
        .render_dirty_region = wavesharestm_render_impl,
        .wake = wavesharestm_wake,
        .sleep = wavesharestm_sleep,
        .init = wavesharestm_init
};

static canvas_t h_canvas = &(struct canvas) {
        .display_impl = &display_impl,
        .height = 600,
        .width = 448
};


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, PIN0|PIN1, GPIO_PIN_RESET);

    // configure the DC and RST pins, connected to PORTA:0 and PORTA:1
    GPIO_InitStruct.Pin = PIN0|PIN1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    // configure the BUSY pin, which is an digital read for our software, currently connected to GPIOB:0
    GPIO_InitStruct.Pin = PIN0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    WAVESHARESTM_BUSY_PIN.port = PORTB;
    WAVESHARESTM_BUSY_PIN.pin = PIN0;

    WAVESHARESTM_DC_PIN.port = PORTA;
    WAVESHARESTM_DC_PIN.pin = PIN0;

    WAVESHARESTM_RST_PIN.port = PORTA;
    WAVESHARESTM_RST_PIN.pin = PIN1;
}

/* SPI1 init function */
void MX_SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;

    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }

    WAVESHARE_SPI_HANDLE = SPI1;
}

// called by the HAL during initialisation
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // PORT A5, A7 correspond to SPI_SCK and SPI1_MOSI
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


int main(void) {
    // Initialise the hardware
    HAL_Init();

    // initialise the clock
    SystemClock_Config();

    // initialise the GPIO pins we are using
    MX_GPIO_Init();

    // initialise SPI1 interface
    MX_SPI1_Init();

    // initialise the morse processor
    morse_init(h_morse);

    // initialise the  morsdle game engine
    morsdle_init_game(h_game);

    // plug the waveshare handle into the display container and then initialise
    canvas_init(h_canvas);
    // initialise the renderer that connects the morsdle game with the display
    renderer_init(h_canvas, h_renderer);

    // initialise the waveshare display for a STM
    h_canvas->display_impl->init();
    h_canvas->display_impl->state = WAVESHARE_DISPLAY_INITIALISED;

    // now all my stuff is initialised, lets get the STM32 intiialised
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();

    while (1) {
        // this probably only needs to happen every 2 * MORSE_DIT ms (dit to high, dit to low)
        bool converted = morse_convert(h_morse, 0);

        if (converted) {
            morse_action_event_t result = (morse_action_event_t){};
            // process the signal buffer and see if there are any letters completed, if there are then we need to
            // send them to the game engine for processing
            if (morse_decode(h_morse, &result)) {
                if(result.type == MORSE_ACTION_ADD_LETTER) {
                    morsdle_add_letter(h_game, result.ch);
                }
                else if(result.type == MORSE_ACTION_BACKSPACE) {
                    morsdle_remove_letter(h_game);
                }
                else if(result.type == MORSE_ACTION_RESET) {
                    // TODO : implement me
                }
                else {
                    // no-op!
                }
            }
        }

        // are there any game events that may require processing
        if (morsdle_has_events(h_game)) {
            morsdle_game_event_t ev;

            // we aren't going to bother drawing unless there's an event, and even then they may not dirty the display
            // so collect any dirty regions into a render_pass and then see if we actually need to redraw.
            struct render_pass render_pass;
            render_pass_init(h_canvas, &render_pass);

            while (morsdle_has_events(h_game)) {
                if (morsdle_read_event(h_game, &ev)) {
                    renderer_handle_event(h_canvas, h_renderer, &render_pass, &ev);
                }
            }

            render_pass_end(&render_pass);
        }
    }

    return 0;
}

void Error_Handler(void)
{
    while(1) {
        printf("_Error_Handler\r\n");
    }
}

void assert_failed(uint8_t *file, uint32_t line) {
}
}


void rising_edge_irq(void) {
    morse_append_signal(h_morse, SIGNAL_HIGH, HAL_GetTick());
}

void falling_edge_irq(void) {
    morse_append_signal(h_morse, SIGNAL_LOW, HAL_GetTick());
}
