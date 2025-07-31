#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "GUI.h"
#include "WM.h"
#include "FRAMEWIN.h"
#include "LCDConf.h"
#include "M48XTouchPanel.h"
#include "Application/Source/Generated/Resource.h"

#ifdef __USE_SD__
#include "diskio.h"
#include "ff.h"
#endif

extern volatile GUI_TIMER_TIME OS_TimeMS;
extern int ts_writefile(void);
extern int ts_readfile(void);
extern void MainTask(void);

volatile int g_enable_Touch;

// Game state definition
typedef enum {
    STATE_WELCOME,
    STATE_PLAY,
    STATE_PAUSE,
    STATE_GAME_OVER,
    STATE_HIGH_SCORE
} GameState;

GameState currentState = STATE_WELCOME;
int time_value = 0;
TEXT_Handle hTextClock = 0;
char timeStr[6];

void UpdateClock(void) {
    sprintf(timeStr, "%02d", time_value);
    if (hTextClock) {
        TEXT_SetText(hTextClock, timeStr);
    }
}

#ifdef __USE_SD__
#if defined (__GNUC__)
BYTE Buff[1024] __attribute__ ((aligned(32)));
#elif defined ( __ICCARM__ )
#pragma data_alignment=32
BYTE Buff[1024];
#else
__align(32) BYTE Buff[1024];
#endif

FIL hFile;

unsigned long get_fattime(void) {
    return 0x00000;
}

#define TEST_SDH SDH0

void SDH0_IRQHandler(void) {
    unsigned int volatile isr;

    if (TEST_SDH->GINTSTS & SDH_GINTSTS_DTAIF_Msk)
        TEST_SDH->GCTL |= SDH_GCTL_GCTLRST_Msk;

    isr = TEST_SDH->INTSTS;
    if (isr & SDH_INTSTS_BLKDIF_Msk) {
        SD0.DataReadyFlag = TRUE;
        TEST_SDH->INTSTS = SDH_INTSTS_BLKDIF_Msk;
    }

    if (isr & SDH_INTSTS_CDIF_Msk) {
        int volatile delay = SystemCoreClock / 1000000 * 10;
        for (; delay > 0UL; delay--) __NOP();
        isr = TEST_SDH->INTSTS;

        if (isr & SDH_INTSTS_CDSTS_Msk) {
            printf("\n***** card remove !\n");
            SD0.IsCardInsert = FALSE;
            memset(&SD0, 0, sizeof(SDH_INFO_T));
        } else {
            printf("***** card insert !\n");
            SDH_Open(TEST_SDH, CardDetect_From_GPIO);
            SDH_Probe(TEST_SDH);
        }
        TEST_SDH->INTSTS = SDH_INTSTS_CDIF_Msk;
    }

    if (isr & SDH_INTSTS_CRCIF_Msk) {
        TEST_SDH->INTSTS = SDH_INTSTS_CRCIF_Msk;
    }
    if (isr & SDH_INTSTS_DITOIF_Msk) {
        printf("***** ISR: data in timeout !\n");
        TEST_SDH->INTSTS |= SDH_INTSTS_DITOIF_Msk;
    }
    if (isr & SDH_INTSTS_RTOIF_Msk) {
        printf("***** ISR: response in timeout !\n");
        TEST_SDH->INTSTS |= SDH_INTSTS_RTOIF_Msk;
    }
}
#endif

static void _SYS_Init(void) {
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_SetCoreClock(FREQ_192MHZ);
    CLK_SetSysTickClockSrc(CLK_CLKSEL0_STCLKSEL_HCLK_DIV2);
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(EBI_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);

    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);

    CLK->AHBCLK |= (1 << 14);
    outpw(0x40007004, 0x16);

    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

#ifdef __USE_SD__
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE7MFP_Msk | SYS_GPE_MFPL_PE6MFP_Msk |
                       SYS_GPE_MFPL_PE3MFP_Msk | SYS_GPE_MFPL_PE2MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE7MFP_SD0_CMD | SYS_GPE_MFPL_PE6MFP_SD0_CLK |
                      SYS_GPE_MFPL_PE3MFP_SD0_DAT1 | SYS_GPE_MFPL_PE2MFP_SD0_DAT0);

    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB5MFP_SD0_DAT3 | SYS_GPB_MFPL_PB4MFP_SD0_DAT2);

    SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD13MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD13MFP_SD0_nCD);
    CLK->AHBCLK |= CLK_AHBCLK_SDH0CKEN_Msk;
#endif

    SystemCoreClockUpdate();
    SYS_LockReg();
}

void TMR0_IRQHandler(void) {
    OS_TimeMS++;
#if GUI_SUPPORT_TOUCH
    if (OS_TimeMS % 10 == 0 && g_enable_Touch)
        GUI_TOUCH_Exec();
#endif
    TIMER_ClearIntFlag(TIMER0);
    TIMER_GetCounter(TIMER0);
}

void APPW_X_FS_Init(void) {}

int main(void) {
    _SYS_Init();
    UART_Open(UART0, 115200);
    g_enable_Touch = 0;

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_SetPriority(TMR0_IRQn, 1);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);

    GPIO_SetMode(PA, BIT0 | BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PG, BIT2 | BIT3 | BIT4, GPIO_MODE_INPUT);
    GPIO_SetMode(PC, BIT9 | BIT10, GPIO_MODE_INPUT);

    GUI_Init();
    Init_TouchPanel();
    g_enable_Touch = 1;

    APPW_X_Setup();
    APPW_Init(APPW_PROJECT_PATH);
    APPW_CreatePersistentScreens();
    APPW_CreateRoot(&ID_SCREEN_WELCOME_RootInfo, WM_HBKWIN);
    currentState = STATE_WELCOME;

    while (1) {
        while (GUI_Exec()) APPW_Exec();
        APPW_Exec();
        GUI_X_Delay(1);

        if (PA0 == 0) {
            switch (currentState) {
                case STATE_WELCOME:
                    APPW_CreateRoot(&ID_SCREEN_PLAY_RootInfo, WM_HBKWIN);
                    currentState = STATE_PLAY;
                    time_value = 0;
                    APPW_SetVarData(ID_VAR_TIME, time_value);
                    if (hTextClock) WM_DeleteWindow(hTextClock);
                    hTextClock = TEXT_CreateEx(160, 133, 70, 21, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "00");
                    TEXT_SetFont(hTextClock, &GUI_Font20_1);
                    TEXT_SetTextAlign(hTextClock, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    UpdateClock();
                    CLK_SysTickDelay(200000);
                    break;
                case STATE_PAUSE:
                    APPW_CreateRoot(&ID_SCREEN_PLAY_RootInfo, WM_HBKWIN);
                    currentState = STATE_PLAY;
                    break;
                case STATE_GAME_OVER:
                    APPW_CreateRoot(&ID_SCREEN_HIGH_SCORE_RootInfo, WM_HBKWIN);
                    currentState = STATE_HIGH_SCORE;
                    break;
                case STATE_HIGH_SCORE:
                    APPW_CreateRoot(&ID_SCREEN_PLAY_RootInfo, WM_HBKWIN);
                    currentState = STATE_PLAY;
                    break;
                default:
                    break;
            }
        }

        if (currentState == STATE_PLAY) {
            int prev_time = time_value;

            if (PA0 == 0 || PG2 == 0 || PG4 == 0) {
                time_value++;
                if (time_value > 10) time_value = 10;
								printf("hello");
            }
            if ((PA1 == 0) || (PG3 == 0) || (PC10 == 0) || (PC9 == 0)) {
                time_value--;
                if (time_value < 0) time_value = 0;
            }

            if (time_value != prev_time) {
                APPW_SetVarData(ID_VAR_TIME, time_value);
                UpdateClock();
            }
            CLK_SysTickDelay(200000);
        }
    }
}
