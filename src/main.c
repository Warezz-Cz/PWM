#include "main.h"
#include "milis.h"
#include <stdbool.h>
#include <stm8s.h>
// #include "delay.h"
#include "daughterboard.h"
#include "sonboard.h"
#include "uart1.h"
#include <stdio.h>

void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    init_milis();
    init_uart1();

    GPIO_Init(DB_S1_PORT, DB_S1_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(DB_S2_PORT, DB_S2_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(DB_S3_PORT, DB_S3_PIN, GPIO_MODE_IN_PU_NO_IT);

    // OC1 -- output channel 1
    GPIO_Init(DB_PWM_B_PORT, DB_PWM_B_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    // OC2 -- output channel 2
    GPIO_Init(DB_PWM_G_PORT, DB_PWM_G_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    // OC3 -- output channel 3
    GPIO_Init(DB_PWM_R_PORT, DB_PWM_R_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 99);
    TIM2_OC1Init(
        TIM2_OCMODE_PWM1,        // PWM1 a PMW2 jsou nazvájem negované
        TIM2_OUTPUTSTATE_ENABLE, // povolí fyzický výstup
        0,                       // počáteční hodnota compare registeru
        TIM2_OCPOLARITY_HIGH // nastavení polarity - LED se spíná logickou 1
    );
    TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0,
                 TIM2_OCPOLARITY_HIGH);
    TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0,
                 TIM2_OCPOLARITY_HIGH);
    TIM2_Cmd(ENABLE);

    // ošetření nežádoucích jevů při změně PWM (compare)
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_OC2PreloadConfig(ENABLE);
    TIM2_OC3PreloadConfig(ENABLE);
}
int main(void)
{
    bool show;
    uint32_t btn_time = 0;
    uint32_t time = 0;
    uint32_t hold_time = 0;
    uint8_t r = 0, g = 0, b = 0;
    bool btn_r_press = 0, btn_g_press = 0, btn_b_press = 0;

    typedef enum {
        no_change,
        display_change
    } status_type;
    status_type status = no_change;

    init();

    while (1) {
        if (milis() - time > 100) {
            time = milis();
            if (status == display_change) {
            printf("R: %d| G: %d| B: %d\n", r, g, b);
            status = no_change;
            }
        }
        if (milis() - btn_time > 20) {
            btn_time = milis();
            if (PUSH(DB_S1) && !btn_b_press) {
                status = display_change;
                b += 10;
                if (b > 100) {
                    b = 0;
                }
            }
            if (PUSH(DB_S2) && !btn_g_press) {
                status = display_change;
                g += 10;
                if (g > 100) {
                    g = 0;
                }
            }
            if (PUSH(DB_S3) && !btn_r_press) {
                status = display_change;
                r += 10;
                if (r > 100) {
                    r = 0;
                }
            }
            btn_r_press = PUSH(DB_S3);
            btn_b_press = PUSH(DB_S1);
            btn_g_press = PUSH(DB_S2);

            TIM2_SetCompare1(b);
            TIM2_SetCompare2(g);
            TIM2_SetCompare3(r);
        }
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
