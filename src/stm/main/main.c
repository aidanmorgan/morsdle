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

wavesharestm_pin_t WAVESHARESTM_BUSY_PIN = (wavesharestm_pin_t) {
        .port = NULL,
        .pin = 0
};

wavesharestm_pin_t WAVESHARESTM_CS_PIN = (wavesharestm_pin_t) {
        .port = NULL,
        .pin = 0

};

wavesharestm_pin_t WAVESHARESTM_DC_PIN = (wavesharestm_pin_t) {
        .port = NULL,
        .pin = 0
};


wavesharestm_pin_t WAVESHARESTM_RST_PIN = (wavesharestm_pin_t) {
        .port = NULL,
        .pin = 0
};

SPI_HandleTypeDef WAVESHARE_SPI_HANDLE = (SPI_HandleTypeDef)NULL;
UART_HandleTypeDef WAVESHARE_UART_HANDLE = (UART_HandleTypeDef)NULL;


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
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&RCC_OscInitStruct) ;

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK;
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, WAVESHARESTM_RST_PIN.pin|WAVESHARESTM_DC_PIN.pin|WAVESHARESTM_CS_PIN.pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : PAPin PAPin PAPin */
    GPIO_InitStruct.Pin = WAVESHARESTM_RST_PIN.pin|WAVESHARESTM_DC_PIN.pin|WAVESHARESTM_CS_PIN.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = WAVESHARESTM_BUSY_PIN.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WAVESHARESTM_BUSY_PIN.port, &GPIO_InitStruct);
}

/* SPI1 init function */
void MX_SPI1_Init(void)
{
    WAVESHARE_SPI_HANDLE.Instance = SPI1;
    WAVESHARE_SPI_HANDLE.Init.Mode = SPI_MODE_MASTER;
    WAVESHARE_SPI_HANDLE.Init.Direction = SPI_DIRECTION_2LINES;
    WAVESHARE_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;
    WAVESHARE_SPI_HANDLE.Init.CLKPolarity = SPI_POLARITY_LOW;
    WAVESHARE_SPI_HANDLE.Init.CLKPhase = SPI_PHASE_1EDGE;
    WAVESHARE_SPI_HANDLE.Init.NSS = SPI_NSS_SOFT;
    WAVESHARE_SPI_HANDLE.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    WAVESHARE_SPI_HANDLE.Init.FirstBit = SPI_FIRSTBIT_MSB;
    WAVESHARE_SPI_HANDLE.Init.TIMode = SPI_TIMODE_DISABLE;
    WAVESHARE_SPI_HANDLE.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    WAVESHARE_SPI_HANDLE.Init.CRCPolynomial = 10;
    HAL_SPI_Init(&WAVESHARE_SPI_HANDLE);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(spiHandle->Instance==SPI1)
    {
        /* USER CODE BEGIN SPI1_MspInit 0 */

        /* USER CODE END SPI1_MspInit 0 */
        /* SPI1 clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**SPI1 GPIO Configuration
        PA5     ------> SPI1_SCK
        PA7     ------> SPI1_MOSI
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI1_MspInit 1 */

        /* USER CODE END SPI1_MspInit 1 */
    }
}


int main(void) {
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
    MX_USART1_UART_Init();
    MX_SPI1_Init();

    while (1) {
        // this probably only needs to happen every 2 * MORSE_DIT ms (dit to high, dit to low)
        bool converted = morse_convert(h_morse, 0);

        if (converted) {
            char result;
            // process the signal buffer and see if there are any letters completed, if there are then we need to
            // send them to the game engine for processing
            if (morse_decode(h_morse, &result)) {
                morsdle_add_letter(h_game, result);
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


void rising_edge_irq(void) {
    morse_append_signal(h_morse, SIGNAL_HIGH, HAL_GetTick());
}

void falling_edge_irq(void) {
    morse_append_signal(h_morse, SIGNAL_LOW, HAL_GetTick());
}
