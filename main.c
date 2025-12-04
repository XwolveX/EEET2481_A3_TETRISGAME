#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "GUI.h"
#include "WM.h"
#include "FRAMEWIN.h"
#include "LCDConf.h"
#include "M48XTouchPanel.h"
#include "TEXT.h"
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

// System Initialization Function
static void _SYS_Init(void) {
    // Unlock protected system registers
    SYS_UnlockReg();

    // Enable internal high-speed RC oscillator (HIRC)
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    // Wait for HIRC to stabilize
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    // Set HCLK (system clock) source to HIRC and divide by 1
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    // Enable external high-speed crystal (HXT)
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    // Wait for HXT to stabilize
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    // Set core clock to 192 MHz using PLL
    CLK_SetCoreClock(FREQ_192MHZ);
    // Set SysTick clock source to HCLK divided by 2
    CLK_SetSysTickClockSrc(CLK_CLKSEL0_STCLKSEL_HCLK_DIV2);

    // Set peripheral clocks: APB0 and APB1 divide by 2
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;

    // Enable clocks for UART0, EBI (External Bus Interface), and TMR0
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(EBI_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);

    // Set UART0 clock source to HXT and divide by 1
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    // Set Timer0 clock source to HXT (no divider)
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);

    // Enable FMC ISP function for embedded flash (manual register write)
    CLK->AHBCLK |= (1 << 14);
    outpw(0x40007004, 0x16);  // Low-level hardware config for EBI/LCD

    // Configure UART0 RXD and TXD (PB12, PB13) multi-function pins
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

#ifdef __USE_SD__
    // Configure SD0 interface pins for PE and PB
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE7MFP_Msk | SYS_GPE_MFPL_PE6MFP_Msk |
                       SYS_GPE_MFPL_PE3MFP_Msk | SYS_GPE_MFPL_PE2MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE7MFP_SD0_CMD | SYS_GPE_MFPL_PE6MFP_SD0_CLK |
                      SYS_GPE_MFPL_PE3MFP_SD0_DAT1 | SYS_GPE_MFPL_PE2MFP_SD0_DAT0);

    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB5MFP_SD0_DAT3 | SYS_GPB_MFPL_PB4MFP_SD0_DAT2);

    // Configure SD card detect pin (PD13)
    SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD13MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD13MFP_SD0_nCD);

    // Enable SDH0 module clock
    CLK->AHBCLK |= CLK_AHBCLK_SDH0CKEN_Msk;
#endif

    // Update SystemCoreClock variable
    SystemCoreClockUpdate();

    // Lock protected system registers
    SYS_LockReg();
}

// Global flag to indicate that the PA.0 button was released (used for game input)
int PA0_Released = 0;

// Timer 0 Interrupt Handler
void TMR0_IRQHandler(void) {
    // Increment the GUI system time (used by emWin GUI library)
    OS_TimeMS++;

#if GUI_SUPPORT_TOUCH
    // Every 10ms, if touch input is enabled, process touch panel input
    if (OS_TimeMS % 10 == 0 && g_enable_Touch)
        GUI_TOUCH_Exec();
#endif

    // Clear the interrupt flag to acknowledge the timer interrupt
    TIMER_ClearIntFlag(TIMER0);

    // Read current timer counter (optional, may help with debug or timing)
    TIMER_GetCounter(TIMER0);
}

void Init_SW1_Interrupt(void) {
    // Configure PA.0 as input
    PA->MODE &= ~(0x3 << 0);      // Clear mode bits for PA.0 (bit 1:0) to set as input (00)

    // Configure interrupt type for PA.0
    PA->INTTYPE &= ~(1 << 0);     // Set PA.0 to edge-triggered (0 = edge-triggered, 1 = level-triggered)

    // Enable falling-edge interrupt for PA.0
    PA->INTEN |= (1 << 0);        // Enable falling-edge interrupt on PA.0 (bit 0 = falling edge)

    // Clear any pending interrupt flag for PA.0
    PA->INTSRC |= (1 << 0);       // Write 1 to clear interrupt source flag for PA.0

    // Enable the GPIO Port A interrupt in the NVIC
    NVIC_EnableIRQ(GPA_IRQn);     // Enable NVIC interrupt for GPIO Port A
}

// GPIO Port A Interrupt Handler
void GPA_IRQHandler(void) {
    // Check if interrupt was triggered by pin PA.0
    if (PA->INTSRC & (1 << 0)) {
        // Clear the interrupt flag for PA.0
        PA->INTSRC |= (1 << 0);

        // Set the flag indicating PA.0 was released (used in main loop for input)
        PA0_Released = 1;
    }
}

// File system initialization stub for AppWizard (no implementation needed here)
void APPW_X_FS_Init(void) {}

// --- High Score System Definitions ---

// Maximum number of high score entries to store
#define MAX_HIGH_SCORES 10

// Maximum length of player name (including null-terminator)
#define NAME_LENGTH 6

// Define a structure to represent a single high score entry
typedef struct {
    char name[NAME_LENGTH];  // Player's name (character array with fixed length)
    int score;               // Player's score
    int minutes;             // Minutes part of the time taken
    int seconds;             // Seconds part of the time taken
    int level;               // Level reached by the player
} HighScoreEntry;

// Declare an array to store multiple high score entries
HighScoreEntry highScores[MAX_HIGH_SCORES];  // Array to store the top high scores

// Track the current number of valid entries in the highScores array
int highScoreCount = 0;  // Number of high score entries currently stored

// GUI text handles for displaying high score information on screen
// Each entry uses 4 text handles: name, score, time, and level
TEXT_Handle hHighScoreTexts[MAX_HIGH_SCORES * 4];  // Array of text handles for GUI display

// Initialize the high score table with default/test values
void initHighScores() {
    // Entry 0: ALICE
    strcpy(highScores[0].name, "ALICE");
    highScores[0].score = 50;
    highScores[0].minutes = 5;
    highScores[0].seconds = 30;
    highScores[0].level = 6;

    // Entry 1: BOBBY
    strcpy(highScores[1].name, "BOBBY");
    highScores[1].score = 45;
    highScores[1].minutes = 4;
    highScores[1].seconds = 45;
    highScores[1].level = 5;

    // Entry 2: CAROL
    strcpy(highScores[2].name, "CAROL");
    highScores[2].score = 40;
    highScores[2].minutes = 4;
    highScores[2].seconds = 20;
    highScores[2].level = 5;

    // Entry 3: DAVID
    strcpy(highScores[3].name, "DAVID");
    highScores[3].score = 35;
    highScores[3].minutes = 3;
    highScores[3].seconds = 55;
    highScores[3].level = 4;

    // Entry 4: EMILY
    strcpy(highScores[4].name, "EMILY");
    highScores[4].score = 2;
    highScores[4].minutes = 1;
    highScores[4].seconds = 10;
    highScores[4].level = 1;

    // Set the count of valid high score entries
    highScoreCount = 5;
}

// Function to sort the highScores array in descending order based on score
void sortHighScores() {
    // Outer loop: repeat for each element (except the last one, already in place)
    for (int i = 0; i < highScoreCount - 1; i++) {
        // Inner loop: compare adjacent elements and bubble the larger score to the front
        for (int j = 0; j < highScoreCount - i - 1; j++) {
            // If the current score is less than the next score, swap them
            if (highScores[j].score < highScores[j + 1].score) {
                // Swap the two entries
                HighScoreEntry temp = highScores[j];
                highScores[j] = highScores[j + 1];
                highScores[j + 1] = temp;
            }
        }
    }
}

// Function to add a new high score entry to the highScores list
void addHighScore(const char* name, int score, int minutes, int seconds, int level) {
    HighScoreEntry newEntry;  // Temporary variable to hold the new entry

    // Initialize the name field with zeros to ensure it’s null-terminated
    memset(newEntry.name, 0, NAME_LENGTH);

    // Copy up to 5 characters from the input name to newEntry.name
    // Ensure we do not exceed NAME_LENGTH - 1 to leave space for the null terminator
    for (int i = 0; i < 5 && i < NAME_LENGTH - 1; i++) {
        newEntry.name[i] = name[i];
    }
    newEntry.name[NAME_LENGTH - 1] = '\0';  // Ensure the string is null-terminated

    // Assign the remaining fields
    newEntry.score = score;
    newEntry.minutes = minutes;
    newEntry.seconds = seconds;
    newEntry.level = level;

    // Add the new entry to the array
    if (highScoreCount < MAX_HIGH_SCORES) {
        // If there's space, add to the end
        highScores[highScoreCount] = newEntry;
        highScoreCount++;
    } else {
        // If the list is full, replace the last entry (lowest score assumed)
        highScores[MAX_HIGH_SCORES - 1] = newEntry;
    }

    // Sort the updated list so the highest scores come first
    sortHighScores();
}


// Function to check if a given score qualifies as a high score
int isHighScore(int score) {
    // If there is still space in the high score list, any score qualifies
    if (highScoreCount < MAX_HIGH_SCORES) {
        return 1;  // True: it's a high score
    }

    // If the list is full, check if the score is higher than the lowest high score
    return score > highScores[MAX_HIGH_SCORES - 1].score;
    // Returns 1 (true) if the new score is higher than the lowest stored score,
    // otherwise returns 0 (false)
}


// Function to clear all high score text elements from the display
void clearHighScoreDisplay() {
    // Loop through all text handles (each high score uses 4: name, score, time, level)
    for (int i = 0; i < MAX_HIGH_SCORES * 4; i++) {
        // Check if the handle is valid (non-zero means it's an active GUI object)
        if (hHighScoreTexts[i] != 0) {
            // Delete the text window from the GUI
            WM_DeleteWindow(hHighScoreTexts[i]);
            // Reset the handle to 0 to indicate it's cleared
            hHighScoreTexts[i] = 0;
        }
    }
}

// Function to display the high scores on the screen using emWin GUI
void displayHighScores() {
    // Clear any existing high score text objects on the screen
    clearHighScoreDisplay();

    // Static array for column title text handles (RANK, NAME, SCORE, TIME, LVL)
    static TEXT_Handle hTitles[5] = {0};

    // Clear any existing title text elements
    for (int i = 0; i < 5; i++) {
        if (hTitles[i] != 0) {
            WM_DeleteWindow(hTitles[i]);
            hTitles[i] = 0;
        }
    }

    // Create and configure the title headers
    hTitles[0] = TEXT_CreateEx(5, 50, 30, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_USER + 100, "RANK");
    TEXT_SetFont(hTitles[0], GUI_FONT_8X8);
    TEXT_SetTextColor(hTitles[0], GUI_YELLOW);

    hTitles[1] = TEXT_CreateEx(40, 50, 50, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_USER + 101, "NAME");
    TEXT_SetFont(hTitles[1], GUI_FONT_8X8);
    TEXT_SetTextColor(hTitles[1], GUI_YELLOW);

    hTitles[2] = TEXT_CreateEx(100, 50, 40, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_USER + 102, "SCORE");
    TEXT_SetFont(hTitles[2], GUI_FONT_8X8);
    TEXT_SetTextColor(hTitles[2], GUI_YELLOW);

    hTitles[3] = TEXT_CreateEx(150, 50, 40, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_USER + 103, "TIME");
    TEXT_SetFont(hTitles[3], GUI_FONT_8X8);
    TEXT_SetTextColor(hTitles[3], GUI_YELLOW);

    hTitles[4] = TEXT_CreateEx(200, 50, 30, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_USER + 104, "LVL");
    TEXT_SetFont(hTitles[4], GUI_FONT_8X8);
    TEXT_SetTextColor(hTitles[4], GUI_YELLOW);

    // Loop through all high score entries and display each one
    for (int i = 0; i < highScoreCount && i < MAX_HIGH_SCORES; i++) {
        int yPos = 70 + i * 18;  // Vertical position for each row
        char tempStr[20];       // Temporary buffer for formatting text

        // --- Rank ---
        sprintf(tempStr, "%d", i + 1);  // Rank number
        hHighScoreTexts[i * 4] = TEXT_CreateEx(10, yPos, 20, 15, WM_HBKWIN, WM_CF_SHOW, 0,
                                               GUI_ID_USER + 200 + i * 4, tempStr);
        TEXT_SetFont(hHighScoreTexts[i * 4], GUI_FONT_6X8);
        TEXT_SetTextColor(hHighScoreTexts[i * 4], GUI_WHITE);
        TEXT_SetTextAlign(hHighScoreTexts[i * 4], GUI_TA_CENTER);

        // --- Name ---
        hHighScoreTexts[i * 4 + 1] = TEXT_CreateEx(40, yPos, 50, 15, WM_HBKWIN, WM_CF_SHOW, 0,
                                                   GUI_ID_USER + 201 + i * 4, highScores[i].name);
        TEXT_SetFont(hHighScoreTexts[i * 4 + 1], GUI_FONT_6X8);
        TEXT_SetTextColor(hHighScoreTexts[i * 4 + 1], GUI_WHITE);

        // --- Score ---
        sprintf(tempStr, "%d", highScores[i].score);
        hHighScoreTexts[i * 4 + 2] = TEXT_CreateEx(100, yPos, 40, 15, WM_HBKWIN, WM_CF_SHOW, 0,
                                                   GUI_ID_USER + 202 + i * 4, tempStr);
        TEXT_SetFont(hHighScoreTexts[i * 4 + 2], GUI_FONT_6X8);
        TEXT_SetTextColor(hHighScoreTexts[i * 4 + 2], GUI_WHITE);
        TEXT_SetTextAlign(hHighScoreTexts[i * 4 + 2], GUI_TA_CENTER);

        // --- Time (mm:ss) ---
        sprintf(tempStr, "%02d:%02d", highScores[i].minutes, highScores[i].seconds);
        hHighScoreTexts[i * 4 + 3] = TEXT_CreateEx(150, yPos, 40, 15, WM_HBKWIN, WM_CF_SHOW, 0,
                                                   GUI_ID_USER + 203 + i * 4, tempStr);
        TEXT_SetFont(hHighScoreTexts[i * 4 + 3], GUI_FONT_6X8);
        TEXT_SetTextColor(hHighScoreTexts[i * 4 + 3], GUI_WHITE);
        TEXT_SetTextAlign(hHighScoreTexts[i * 4 + 3], GUI_TA_CENTER);

        // --- Level ---
        sprintf(tempStr, "%d", highScores[i].level);
        TEXT_Handle hLevelText = TEXT_CreateEx(200, yPos, 30, 15, WM_HBKWIN, WM_CF_SHOW, 0,
                                               GUI_ID_USER + 300 + i, tempStr);
        TEXT_SetFont(hLevelText, GUI_FONT_6X8);
        TEXT_SetTextColor(hLevelText, GUI_WHITE);
        TEXT_SetTextAlign(hLevelText, GUI_TA_CENTER);
    }

    // Instruction at the bottom of the screen
    TEXT_Handle hInstruction = TEXT_CreateEx(44, 280, 160, 15, WM_HBKWIN, WM_CF_SHOW, 0,
                                             GUI_ID_USER + 400, "Press SW1 to play again");
    TEXT_SetFont(hInstruction, GUI_FONT_6X8);
    TEXT_SetTextColor(hInstruction, GUI_GREEN);
    TEXT_SetTextAlign(hInstruction, GUI_TA_CENTER);
}

	// Game state definition	
// Enumeration representing the different states of the game
typedef enum {
    STATE_WELCOME,       // Initial welcome screen
    STATE_PLAY,          // Game is actively running
    STATE_PAUSE,         // Game is paused
    STATE_GAME_OVER,     // Game over screen
    STATE_HIGH_SCORE     // High score display screen
} GameState;

// Variable to hold the current game state
GameState currentState = STATE_WELCOME;

// General-purpose time value (could be used for transitions, delays, etc.)
int time_value = 0;

// Define a constant for identifying the game clock timer
#define CLOCK_TIMER_ID 1

// GUI text handle for displaying the clock/timer
TEXT_Handle hTextClock;

// GUI text handle for displaying the "PAUSED" message
TEXT_Handle pauseText;

// GUI text handles for notifications or system messages
TEXT_Handle notificationText1;
TEXT_Handle notificationText2;

// Handle for the software timer used to track elapsed game time
WM_HTIMER hClockTimer = 0;

// Elapsed time during gameplay (tracked in minutes and seconds)
int game_seconds = 0;  // Seconds part of the elapsed game time
int game_minutes = 0;  // Minutes part of the elapsed game time

// Flag to indicate whether the clock is currently running
// 0 = paused, 1 = running
int clock_running = 0;

// Callback function for the game clock timer
static void _cbClockTimer(WM_MESSAGE *pMsg) {
    char timeStr[10];  // Buffer to store formatted time string (MM:SS)

    switch (pMsg->MsgId) {
        case WM_TIMER:
            // Only update the clock if the game is actively running
            if (clock_running) {
                game_seconds++;  // Increment seconds

                // Handle rollover to minutes
                if (game_seconds >= 60) {
                    game_seconds = 0;
                    game_minutes++;
                }

                // Format time as MM:SS and update clock display
                sprintf(timeStr, "%02d:%02d", game_minutes, game_seconds);
                TEXT_SetText(hTextClock, timeStr);
            }

            // Restart the timer to tick again in 1000 ms (1 second)
            WM_RestartTimer(pMsg->Data.v, 1000);
            break;

        default:
            // Default processing for any unhandled messages
            WM_DefaultProc(pMsg);
            break;
    }
}


// Define the dimensions and layout of the Tetris-style game grid
#define GRID_WIDTH 14         // Number of columns in the game grid
#define GRID_HEIGHT 30        // Number of rows in the game grid
#define GRID_UNIT_SIZE 10     // Size (in pixels) of each grid cell
#define GRID_OFFSET_X 10      // Horizontal offset from the left edge of the screen
#define GRID_OFFSET_Y 10      // Vertical offset from the top edge of the screen

// GUI text handles for in-game information display
TEXT_Handle hTextScore;       // Display for player's score
TEXT_Handle hTextLines;       // Display for lines cleared
TEXT_Handle hTextLevel;       // Display for current level

// Windows for different game UI components
WM_HWIN hGameWin;             // Handle to the main game window (play area)
WM_HWIN hNextWin;             // Handle to the "Next block" preview window

// 2D array representing the game grid (each cell holds block info or 0 for empty)
unsigned char gameGrid[GRID_HEIGHT][GRID_WIDTH];

// Structure to define a Tetromino block (Tetris piece)
typedef struct {
    char shape[4][4][4];    // 3D array: [rotation][row][col]
                            // Defines the 4x4 grid for each of the 4 rotation states (0–3)
    unsigned char color;    // Block color identifier (1–7 for different colors)
} Tetromino;

// Array of 7 Tetromino pieces (I, O, T, S, Z, J, L)
Tetromino tetrominos[7];


// Structure representing a block currently in play (falling)
typedef struct {
    int type;       // Tetromino type index (0–6)
    int rotation;   // Current rotation state (0–3)
    int x, y;       // Position of the top-left corner in the grid
} CurrentBlock;

// Instance for the block currently controlled by the player
CurrentBlock currentBlock;

// Instance for the "next" block to be shown in preview window
CurrentBlock nextBlock;

// Game progress and statistics
int score = 0;         // Player’s current score
int highScore = 0;     // Highest score achieved (could be saved/loaded)
int level = 1;         // Current game level (affects speed)
int linesCleared = 0;  // Total number of lines cleared in the current session


void initTetrominos() {
    // === I-Block (Cyan) ===
    memset(tetrominos[0].shape, 0, sizeof(tetrominos[0].shape));
    // Horizontal (Rotation 0)
    tetrominos[0].shape[0][1][0] = 1;
    tetrominos[0].shape[0][1][1] = 1;
    tetrominos[0].shape[0][1][2] = 1;
    tetrominos[0].shape[0][1][3] = 1;
    // Vertical (Rotation 1)
    tetrominos[0].shape[1][0][2] = 1;
    tetrominos[0].shape[1][1][2] = 1;
    tetrominos[0].shape[1][2][2] = 1;
    tetrominos[0].shape[1][3][2] = 1;
    // Horizontal (Rotation 2, same shape with lower row)
    tetrominos[0].shape[2][2][0] = 1;
    tetrominos[0].shape[2][2][1] = 1;
    tetrominos[0].shape[2][2][2] = 1;
    tetrominos[0].shape[2][2][3] = 1;
    // Vertical (Rotation 3)
    tetrominos[0].shape[3][0][1] = 1;
    tetrominos[0].shape[3][1][1] = 1;
    tetrominos[0].shape[3][2][1] = 1;
    tetrominos[0].shape[3][3][1] = 1;
    tetrominos[0].color = 1;

    // === J-Block (Blue) ===
    memset(tetrominos[1].shape, 0, sizeof(tetrominos[1].shape));
    // + shape (Rotation 0)
    tetrominos[1].shape[0][0][0] = 1;
    tetrominos[1].shape[0][1][0] = 1;
    tetrominos[1].shape[0][1][1] = 1;
    tetrominos[1].shape[0][1][2] = 1;
    // ? shape (Rotation 1)
    tetrominos[1].shape[1][0][1] = 1;
    tetrominos[1].shape[1][0][2] = 1;
    tetrominos[1].shape[1][1][1] = 1;
    tetrominos[1].shape[1][2][1] = 1;
    // + shape (Rotation 2)
    tetrominos[1].shape[2][1][0] = 1;
    tetrominos[1].shape[2][1][1] = 1;
    tetrominos[1].shape[2][1][2] = 1;
    tetrominos[1].shape[2][2][2] = 1;
    // ? shape (Rotation 3)
    tetrominos[1].shape[3][0][1] = 1;
    tetrominos[1].shape[3][1][1] = 1;
    tetrominos[1].shape[3][2][1] = 1;
    tetrominos[1].shape[3][2][0] = 1;
    tetrominos[1].color = 2;

    // === L-Block (Orange) ===
    memset(tetrominos[2].shape, 0, sizeof(tetrominos[2].shape));
    // + shape (Rotation 0)
    tetrominos[2].shape[0][0][2] = 1;
    tetrominos[2].shape[0][1][0] = 1;
    tetrominos[2].shape[0][1][1] = 1;
    tetrominos[2].shape[0][1][2] = 1;
    // ? shape (Rotation 1)
    tetrominos[2].shape[1][0][1] = 1;
    tetrominos[2].shape[1][1][1] = 1;
    tetrominos[2].shape[1][2][1] = 1;
    tetrominos[2].shape[1][2][2] = 1;
    // + shape (Rotation 2)
    tetrominos[2].shape[2][1][0] = 1;
    tetrominos[2].shape[2][1][1] = 1;
    tetrominos[2].shape[2][1][2] = 1;
    tetrominos[2].shape[2][2][0] = 1;
    // ? shape (Rotation 3)
    tetrominos[2].shape[3][0][0] = 1;
    tetrominos[2].shape[3][0][1] = 1;
    tetrominos[2].shape[3][1][1] = 1;
    tetrominos[2].shape[3][2][1] = 1;
    tetrominos[2].color = 3;

    // === O-Block (Yellow) ===
    memset(tetrominos[3].shape, 0, sizeof(tetrominos[3].shape));
    // Same 2x2 shape for all 4 rotations
    for (int r = 0; r < 4; r++) {
        tetrominos[3].shape[r][1][1] = 1;
        tetrominos[3].shape[r][1][2] = 1;
        tetrominos[3].shape[r][2][1] = 1;
        tetrominos[3].shape[r][2][2] = 1;
    }
    tetrominos[3].color = 4;

    // === S-Block (Green) ===
    memset(tetrominos[4].shape, 0, sizeof(tetrominos[4].shape));
    // Zigzag (Rotation 0)
    tetrominos[4].shape[0][1][1] = 1;
    tetrominos[4].shape[0][1][2] = 1;
    tetrominos[4].shape[0][2][0] = 1;
    tetrominos[4].shape[0][2][1] = 1;
    // Rotation 1
    tetrominos[4].shape[1][0][1] = 1;
    tetrominos[4].shape[1][1][1] = 1;
    tetrominos[4].shape[1][1][2] = 1;
    tetrominos[4].shape[1][2][2] = 1;
    // Rotations 2 and 3 are the same as 0 and 1
    memcpy(tetrominos[4].shape[2], tetrominos[4].shape[0], sizeof(tetrominos[4].shape[0]));
    memcpy(tetrominos[4].shape[3], tetrominos[4].shape[1], sizeof(tetrominos[4].shape[1]));
    tetrominos[4].color = 5;

    // === T-Block (Purple) ===
    memset(tetrominos[5].shape, 0, sizeof(tetrominos[5].shape));
    // T shape (Rotation 0)
    tetrominos[5].shape[0][1][0] = 1;
    tetrominos[5].shape[0][1][1] = 1;
    tetrominos[5].shape[0][1][2] = 1;
    tetrominos[5].shape[0][2][1] = 1;
    // Rotation 1
    tetrominos[5].shape[1][0][1] = 1;
    tetrominos[5].shape[1][1][1] = 1;
    tetrominos[5].shape[1][1][2] = 1;
    tetrominos[5].shape[1][2][1] = 1;
    // Rotation 2
    tetrominos[5].shape[2][1][0] = 1;
    tetrominos[5].shape[2][1][1] = 1;
    tetrominos[5].shape[2][1][2] = 1;
    tetrominos[5].shape[2][0][1] = 1;
    // Rotation 3
    tetrominos[5].shape[3][0][1] = 1;
    tetrominos[5].shape[3][1][0] = 1;
    tetrominos[5].shape[3][1][1] = 1;
    tetrominos[5].shape[3][2][1] = 1;
    tetrominos[5].color = 6;

    // === Z-Block (Red) ===
    memset(tetrominos[6].shape, 0, sizeof(tetrominos[6].shape));
    // Z shape (Rotation 0)
    tetrominos[6].shape[0][1][0] = 1;
    tetrominos[6].shape[0][1][1] = 1;
    tetrominos[6].shape[0][2][1] = 1;
    tetrominos[6].shape[0][2][2] = 1;
    // Rotation 1
    tetrominos[6].shape[1][0][2] = 1;
    tetrominos[6].shape[1][1][1] = 1;
    tetrominos[6].shape[1][1][2] = 1;
    tetrominos[6].shape[1][2][1] = 1;
    // Rotations 2 and 3 same as 0 and 1
    memcpy(tetrominos[6].shape[2], tetrominos[6].shape[0], sizeof(tetrominos[6].shape[0]));
    memcpy(tetrominos[6].shape[3], tetrominos[6].shape[1], sizeof(tetrominos[6].shape[1]));
    tetrominos[6].color = 7;
}


   // Ð?nh nghia màu cho các tetromino
// Color mapping for each block type (index corresponds to Tetromino color value)
GUI_COLOR blockColors[8] = {
    GUI_BLACK,      // 0: Empty cell
    GUI_CYAN,       // 1: I-Block
    GUI_BLUE,       // 2: J-Block
    GUI_ORANGE,     // 3: L-Block
    GUI_YELLOW,     // 4: O-Block
    GUI_GREEN,      // 5: S-Block
    GUI_MAGENTA,    // 6: T-Block
    GUI_RED         // 7: Z-Block
};

// Function to check if the current block can be placed on the grid without collisions
int isValidPosition(CurrentBlock block) {
    // Loop through the 4x4 shape matrix of the block
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            // Check if this cell is filled in the current rotation
            if (tetrominos[block.type].shape[block.rotation][y][x]) {
                int gridX = block.x + x;  // Translate to grid coordinates
                int gridY = block.y + y;

                // Check boundaries: must stay within game grid
                if (gridX < 0 || gridX >= GRID_WIDTH || gridY < 0 || gridY >= GRID_HEIGHT) {
                    return 0;  // Invalid position (out of bounds)
                }

                // Check for collision with existing blocks in the grid
                if (gameGrid[gridY][gridX] != 0) {
                    return 0;  // Invalid position (occupied)
                }
            }
        }
    }

    // No collisions and within bounds ? valid position
    return 1;
}
	
// Character buffer to hold player's name (5 characters + null terminator)
char playerName[6] = "AAAAA";  // Initial default name

// Array storing the current letter index (0–25) for each of the 5 characters
int currentLetter[5] = {0, 0, 0, 0, 0};  // 0 corresponds to 'A'

// Index of the currently selected character (0–4)
int selectedChar = 0;

// Available characters for name selection
const char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// GUI text handles to display each character of the name
TEXT_Handle hNameChars[5];  // One text handle per letter slot

// Function to create and display the character selection UI for entering player name
void showNameInputUI() {
    int startX = 72, startY = 210;  // Starting position for the name input display
    int spacing = 20;               // Horizontal space between each character slot

    // Loop through each character slot (5 total)
    for (int i = 0; i < 5; i++) {
        // Prepare string for the current letter (1 char + null terminator)
        char letterStr[2] = { ALPHABET[currentLetter[i]], '\0' };

        // Create a text widget for each character in the name
        hNameChars[i] = TEXT_CreateEx(startX + i * spacing, startY, 20, 20,
                                      WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0 + i, letterStr);

        // Set font and style for character display
        TEXT_SetFont(hNameChars[i], GUI_FONT_16B_ASCII);
        TEXT_SetTextColor(hNameChars[i], GUI_WHITE);
        TEXT_SetTextAlign(hNameChars[i], GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
}

// Function to update the displayed letters and highlight the selected character
void updateNameUI() {
    for (int i = 0; i < 5; i++) {
        // Update displayed character based on the currentLetter index
        char letterStr[2] = { ALPHABET[currentLetter[i]], '\0' };
        TEXT_SetText(hNameChars[i], letterStr);

        // Highlight the currently selected character with a different background
        if (i == selectedChar) {
            TEXT_SetBkColor(hNameChars[i], GUI_BLUE);   // Selected character gets a blue background
        } else {
            TEXT_SetBkColor(hNameChars[i], GUI_BLACK);  // Others use black background
        }
    }
}

// Function to generate the next falling block and handle game over if placement fails
void generateNewBlock() {
    // Transfer the next block into the active/current block
    currentBlock.type = nextBlock.type;
    currentBlock.rotation = nextBlock.rotation;

    // Set initial position (centered horizontally, top of the grid)
    currentBlock.x = GRID_WIDTH / 2 - 2;  // Adjust for 4x4 shape
    currentBlock.y = 0;

    // Generate the next block randomly (0 to 6 ? 7 Tetromino types)
    nextBlock.type = rand() % 7;
    nextBlock.rotation = 0;

    // Check if the new current block is in a valid position
    if (!isValidPosition(currentBlock)) {
        // No valid space to spawn ? game over
        currentState = STATE_GAME_OVER;

        // Show game over screen using emWin APPWizard
        APPW_CreateRoot(&ID_GAME_OVER_RootInfo, WM_HBKWIN);

        // Prepare name input UI for high score entry
        selectedChar = 0;
        for (int i = 0; i < 5; i++) currentLetter[i] = 0;

        showNameInputUI();  // Create the character entry UI
        updateNameUI();     // Highlight and display current name

        // Stop the game timer
        clock_running = 0;
        return;
    }
}


// Function to initialize the block system at the start of a game
void initBlocks() {
    // Generate a random block to start as the "next" block
    nextBlock.type = rand() % 7;
    nextBlock.rotation = 0;

    // Promote it to current block and prepare the next one
    generateNewBlock();  // Handles initial block + game over check
}


// Move the current block one cell to the left, if the position is valid
void moveLeft() {
    CurrentBlock newPos = currentBlock;  // Copy current block state
    newPos.x--;                          // Decrement x to move left

    // If the new position is valid (no collision or out-of-bounds), apply the move
    if (isValidPosition(newPos)) {
        currentBlock = newPos;
    }
}


// Move the current block one cell to the right, if the position is valid
void moveRight() {
    CurrentBlock newPos = currentBlock;  // Copy current block state
    newPos.x++;                          // Increment x to move right

    // Apply the move only if it's valid
    if (isValidPosition(newPos)) {
        currentBlock = newPos;
    }
}

// Rotate the current block clockwise, if the new orientation is valid
void rotateBlock() {
    CurrentBlock newPos = currentBlock;               // Copy current block state
    newPos.rotation = (newPos.rotation + 1) % 4;      // Rotate clockwise (0 ? 1 ? 2 ? 3 ? 0)

    // Apply rotation only if it's valid
    if (isValidPosition(newPos)) {
        currentBlock = newPos;
    }
}

// Function to update the on-screen HUD (score, lines, level)
void updateHUD() {
    char buf[20];  // Buffer for converting numbers to strings

    // Update score text
    sprintf(buf, "%d", score);
    TEXT_SetText(hTextScore, buf);

    // Update lines cleared text
    sprintf(buf, "%d", linesCleared);
    TEXT_SetText(hTextLines, buf);

    // Update current level text
    sprintf(buf, "%d", level);
    TEXT_SetText(hTextLevel, buf);
}

// Function to check for and clear any full rows in the game grid
void checkFullRows() {
    int linesRemoved = 0;  // Track how many lines are cleared

    // Scan rows from bottom to top
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        int isFull = 1;  // Assume row is full

        // Check every cell in the row
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (gameGrid[y][x] == 0) {
                isFull = 0;  // Found an empty cell ? not full
                break;
            }
        }

        if (isFull) {
            linesRemoved++;  // Track cleared lines

            // Move all rows above down by one
            for (int moveY = y; moveY > 0; moveY--) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                    gameGrid[moveY][x] = gameGrid[moveY - 1][x];
                }
            }

            // Clear the top row (now copied down)
            for (int x = 0; x < GRID_WIDTH; x++) {
                gameGrid[0][x] = 0;
            }

            // Stay on same y to check the newly moved row
            y++;
        }
    }

    // If any rows were cleared, update game stats and HUD
    if (linesRemoved > 0) {
        score += linesRemoved;           // Increase score
        linesCleared += linesRemoved;    // Update total lines cleared

        level = score / 5 + 1;           // Level increases every 5 points
        if (level > 10) level = 10;      // Cap level at 10

        if (score > highScore) {         // Update high score if beaten
            highScore = score;
        }

        updateHUD();  // Refresh the displayed values
    }
}


// Lock the current block into the game grid (make it permanent)
void lockBlockToGrid() {
    // Loop through the 4x4 matrix of the current block
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            // If this cell is part of the Tetromino shape
            if (tetrominos[currentBlock.type].shape[currentBlock.rotation][y][x]) {
                int gridX = currentBlock.x + x;
                int gridY = currentBlock.y + y;

                // Place the block inside the grid bounds
                if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                    gameGrid[gridY][gridX] = tetrominos[currentBlock.type].color;  // Save block color
                }
            }
        }
    }

    // Check if any rows became full after locking the block
    checkFullRows();

    // Short delay before the next block appears (faster with higher level)
    GUI_X_Delay((int)(500 - (level - 1) * 50));  // Delay decreases as level increases

    // Generate a new falling block
    generateNewBlock();
}

// Try to move the current block down by 1 row
// Returns 1 if moved successfully, 0 if it can't move and must lock
int moveDown() {
    CurrentBlock newPos = currentBlock;
    newPos.y++;  // Move down by 1

    if (isValidPosition(newPos)) {
        currentBlock = newPos;  // Apply move
        return 1;
    } else {
        // If move is invalid, lock the block in place
        lockBlockToGrid();
        return 0;
    }
}

// Instantly drop the block to the bottom of the grid
void hardDrop() {
    while (moveDown());  // Keep moving down until it can't anymore
}
	
			
// Draw a single block on the screen at grid coordinate (x, y) using specified color
void drawBlock(int x, int y, int color) {
    // Convert grid position to screen pixel coordinates
    int screenX = x * GRID_UNIT_SIZE;
    int screenY = y * GRID_UNIT_SIZE;

    // Fill the block area with the corresponding Tetromino color
    GUI_SetColor(blockColors[color]);
    GUI_FillRect(screenX, screenY, screenX + GRID_UNIT_SIZE - 1, screenY + GRID_UNIT_SIZE - 1);

    // Draw a white outline around the block for visual separation
    GUI_SetColor(GUI_WHITE);
    GUI_DrawRect(screenX, screenY, screenX + GRID_UNIT_SIZE - 1, screenY + GRID_UNIT_SIZE - 1);
}

// Draw red grid lines over the game area for visual guidance
void drawGridLines() {
    GUI_SetColor(GUI_RED);  // Set line color to red

    int x0 = 0;
    int y0 = 0;
    int width = GRID_WIDTH * GRID_UNIT_SIZE;
    int height = GRID_HEIGHT * GRID_UNIT_SIZE;

    // Draw vertical grid lines
    for (int i = 1; i < GRID_WIDTH; i++) {
        int x = x0 + i * GRID_UNIT_SIZE;
        GUI_DrawLine(x, y0, x, y0 + height);
    }

    // Draw horizontal grid lines
    for (int i = 1; i < GRID_HEIGHT; i++) {
        int y = y0 + i * GRID_UNIT_SIZE;
        GUI_DrawLine(x0, y, x0 + width, y);
    }
}

// Draw the current state of the game grid
void drawGrid() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (gameGrid[y][x] != 0) {
                // Occupied cell: draw the block with its color
                drawBlock(x, y, gameGrid[y][x]);
            } else {
                // Empty cell: fill with black
                int screenX = x * GRID_UNIT_SIZE;
                int screenY = y * GRID_UNIT_SIZE;
                GUI_SetColor(GUI_BLACK);
                GUI_FillRect(screenX, screenY, screenX + GRID_UNIT_SIZE - 1, screenY + GRID_UNIT_SIZE - 1);
            }
        }
    }
}



// Draw the current falling block on top of the grid
void drawCurrentBlock() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetrominos[currentBlock.type].shape[currentBlock.rotation][y][x]) {
                int gridX = currentBlock.x + x;
                int gridY = currentBlock.y + y;

                // Ensure we only draw within the grid bounds
                if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                    drawBlock(gridX, gridY, tetrominos[currentBlock.type].color);
                }
            }
        }
    }
}



// Draw the next block in the preview window
void drawNextBlock() {
    // Switch drawing context to the "Next block" preview window
    WM_SelectWindow(hNextWin);

    // Clear the preview area
    GUI_SetColor(GUI_BLACK);
    GUI_FillRect(0, 0, 40, 40);  // Clear a 4x4 block space (assuming 10x10 unit size)

    // Draw the next Tetromino shape in its default rotation
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetrominos[nextBlock.type].shape[nextBlock.rotation][y][x]) {
                int screenX = x * GRID_UNIT_SIZE;
                int screenY = y * GRID_UNIT_SIZE;

                // Fill cell with block color
                GUI_SetColor(blockColors[tetrominos[nextBlock.type].color]);
                GUI_FillRect(screenX, screenY, screenX + GRID_UNIT_SIZE - 1, screenY + GRID_UNIT_SIZE - 1);

                // Outline the cell
                GUI_SetColor(GUI_WHITE);
                GUI_DrawRect(screenX, screenY, screenX + GRID_UNIT_SIZE - 1, screenY + GRID_UNIT_SIZE - 1);
            }
        }
    }

    // Return drawing context to main background window
    WM_SelectWindow(WM_HBKWIN);
}


// Called repeatedly to update the game state (timed falling, redrawing)
void updateGame() {
    static int dropCounter = 0;  // Tracks time passed for automatic drop
    static int dropDelay = 500; // Delay (in ms) between auto-drops; reduces as level increases

    if (!clock_running) return;  // If game is paused or over, skip update

    // Adjust delay according to level (faster at higher levels)
    dropDelay = (int)(500 - (level - 1) * 50);
    if (dropDelay < 50) dropDelay = 50;  // Set minimum delay limit

    dropCounter += 10;  // Assume this function is called every 10ms (or tuned to match timer interval)

    // If enough time has passed, drop the block by 1 row
    if (dropCounter >= dropDelay) {
        dropCounter = 0;
        moveDown();  // Try to move block down
    }

    // Request a repaint of the game window (triggers WM_PAINT)
    WM_InvalidateWindow(hGameWin);
}

// Initialize game state, grid, score, level, and first blocks
void initGame() {
    // Clear the game grid (remove all blocks)
    memset(gameGrid, 0, sizeof(gameGrid));

    // Reset game metrics
    score = 0;
    linesCleared = 0;
    level = 1;

    // Initialize the first and next falling blocks
    initBlocks();

    // Set game state to PLAY mode
    currentState = STATE_PLAY;
    clock_running = 1;  // Enable timer and block dropping

    // Reset game timer display
    game_seconds = 0;
    game_minutes = 0;
}


// Window callback to handle drawing the game field
static void _cbGameWin(WM_MESSAGE *pMsg) {
    switch (pMsg->MsgId) {
        case WM_PAINT:
            drawGrid();          // Draw existing placed blocks
            drawCurrentBlock();  // Draw currently falling block
            drawGridLines();     // Overlay grid lines
            drawNextBlock();     // Show preview of the next block
            break;

        default:
            WM_DefaultProc(pMsg);  // Handle other messages with default behavior
            break;
    }
}

// Remove name entry UI elements (e.g., after submitting score or canceling)
void clearNameInputUI() {
    for (int i = 0; i < 5; i++) {
        if (hNameChars[i] != 0) {
            WM_DeleteWindow(hNameChars[i]);  // Delete the character text widget
            hNameChars[i] = 0;               // Clear the handle
        }
    }
}

// Holds the final name submitted by the player (after name input complete)
char finalName[NAME_LENGTH];

int main(void) {
    // === SYSTEM INITIALIZATION ===
    _SYS_Init();                   // Initialize clocks, peripherals
    Init_SW1_Interrupt();          // Setup external interrupt on PA.0 (SW1)
    UART_Open(UART0, 115200);      // Open UART0 for debugging or output
    g_enable_Touch = 0;           // Disable touch until after setup

    // === TIMER0 CONFIGURATION ===
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);  // 1kHz periodic timer
    TIMER_EnableInt(TIMER0);
    NVIC_SetPriority(TMR0_IRQn, 1);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);

    // === GPIO INPUT SETUP ===
    GPIO_SetMode(PA, BIT0 | BIT1, GPIO_MODE_INPUT);       // SW1 and possibly another input
    GPIO_SetMode(PG, BIT2 | BIT3 | BIT4, GPIO_MODE_INPUT); // Directional buttons (UP, DOWN, RIGHT)
    GPIO_SetMode(PC, BIT9 | BIT10, GPIO_MODE_INPUT);       // Directional buttons (LEFT, DOWN)

    // === GUI INIT ===
    GUI_Init();               // emWin GUI library init
    initHighScores();         // Load or prepare high score list
    Init_TouchPanel();        // Initialize touch panel
    g_enable_Touch = 1;       // Enable touch input

    // === APPWIZARD UI SETUP ===
    APPW_X_Setup();                              // AppWizard support setup
    APPW_Init(APPW_PROJECT_PATH);                // Initialize screens
    APPW_CreatePersistentScreens();              // Load reusable screens
    APPW_CreateRoot(&ID_SCREEN_WELCOME_RootInfo, WM_HBKWIN); // Show Welcome screen
    currentState = STATE_WELCOME;

    // === MAIN GAME LOOP ===
    while (1) {
        // Execute GUI task and AppWizard logic
        while (GUI_Exec()) APPW_Exec();
        APPW_Exec();
        GUI_X_Delay(1);

        // === GAME LOOP TIMING (every ~10ms) ===
        static uint32_t lastUpdateTime = 0;
        uint32_t currentTime = OS_TimeMS;
        if (currentState == STATE_PLAY && currentTime - lastUpdateTime >= 10) {
            updateGame(); 
            lastUpdateTime = currentTime;
        }

        // === SW1 BUTTON HANDLER ===
        if (PA0_Released == 1) {
            PA0_Released = 0;

            switch (currentState) {
                case STATE_WELCOME:
                    // Switch to gameplay screen
                    APPW_CreateRoot(&ID_SCREEN_PLAY_RootInfo, WM_HBKWIN);
                    hGameWin = WM_CreateWindowAsChild(GRID_OFFSET_X, GRID_OFFSET_Y,
                        GRID_WIDTH * GRID_UNIT_SIZE,
                        GRID_HEIGHT * GRID_UNIT_SIZE,
                        WM_HBKWIN, WM_CF_SHOW, _cbGameWin, 0);
                    hNextWin = WM_CreateWindowAsChild(180, 20, 40, 40, WM_HBKWIN, WM_CF_SHOW, 0, 0);

                    initTetrominos();    // Initialize Tetromino shapes
                    initGame();          // Initialize game state

                    time_value = 0;
                    game_minutes = 0;
                    game_seconds = 0;
                    clock_running = 1;
                    APPW_SetVarData(ID_VAR_TIME, time_value);

                    // === Create HUD Texts ===
                    hTextClock = TEXT_CreateEx(160, 126, 70, 21, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT1, "00:00");
                    TEXT_SetFont(hTextClock, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextClock, GUI_WHITE);
                    TEXT_SetTextAlign(hTextClock, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    WM_CreateTimer(WM_HBKWIN, CLOCK_TIMER_ID, 1000, 0);
                    WM_SetCallback(WM_HBKWIN, _cbClockTimer);

                    hTextScore = TEXT_CreateEx(160, 199, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT4, "0");
                    TEXT_SetFont(hTextScore, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextScore, GUI_WHITE);
                    TEXT_SetTextAlign(hTextScore, GUI_TA_HCENTER | GUI_TA_VCENTER);

                    hTextLines = TEXT_CreateEx(160, 235, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT5, "0");
                    TEXT_SetFont(hTextLines, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextLines, GUI_WHITE);
                    TEXT_SetTextAlign(hTextLines, GUI_TA_HCENTER | GUI_TA_VCENTER);

                    hTextLevel = TEXT_CreateEx(160, 163, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT6, "1");
                    TEXT_SetFont(hTextLevel, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextLevel, GUI_WHITE);
                    TEXT_SetTextAlign(hTextLevel, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    break;

                case STATE_PLAY:
                    // Toggle pause/resume
                    clock_running = !clock_running;
                    if (!clock_running) {
                        pauseText = TEXT_CreateEx(160, 270, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT2, "PAUSE");
                        TEXT_SetFont(pauseText, GUI_FONT_16B_ASCII);
                        TEXT_SetTextColor(pauseText, GUI_WHITE);
                        TEXT_SetTextAlign(pauseText, GUI_TA_HCENTER | GUI_TA_VCENTER);

                        notificationText1 = TEXT_CreateEx(160, 285, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT3, "press SW1");
                        TEXT_SetFont(notificationText1, GUI_FONT_6X8);
                        TEXT_SetTextColor(notificationText1, GUI_WHITE);
                        TEXT_SetTextAlign(notificationText1, GUI_TA_HCENTER | GUI_TA_VCENTER);

                        notificationText2 = TEXT_CreateEx(160, 295, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT3, "to continue");
                        TEXT_SetFont(notificationText2, GUI_FONT_6X8);
                        TEXT_SetTextColor(notificationText2, GUI_WHITE);
                        TEXT_SetTextAlign(notificationText2, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    } else {
                        WM_DeleteWindow(notificationText1);
                        WM_DeleteWindow(notificationText2);
                        WM_DeleteWindow(pauseText);
                    }
                    break;

                case STATE_GAME_OVER:
                    // Collect player name and store score
                    for (int i = 0; i < 5; i++)
                        finalName[i] = ALPHABET[currentLetter[i]];
                    finalName[5] = '\0';

                    for (int i = 0; i < 5; i++) {
                        if (hNameChars[i] != 0) {
                            WM_DeleteWindow(hNameChars[i]);
                            hNameChars[i] = 0;
                        }
                    }

                    strcpy(playerName, finalName);
                    addHighScore(finalName, score, game_minutes, game_seconds, level);

                    APPW_CreateRoot(&ID_SCREEN_HIGH_SCORE_RootInfo, WM_HBKWIN);
                    clearHighScoreDisplay();
                    displayHighScores();
                    currentState = STATE_HIGH_SCORE;
                    break;

                case STATE_HIGH_SCORE:
                    // Replay game from high score screen
                    clearHighScoreDisplay();
                    APPW_CreateRoot(&ID_SCREEN_PLAY_RootInfo, WM_HBKWIN);

                    hGameWin = WM_CreateWindowAsChild(GRID_OFFSET_X, GRID_OFFSET_Y,
                        GRID_WIDTH * GRID_UNIT_SIZE,
                        GRID_HEIGHT * GRID_UNIT_SIZE,
                        WM_HBKWIN, WM_CF_SHOW, _cbGameWin, 0);

                    hNextWin = WM_CreateWindowAsChild(170, 20, 40, 40, WM_HBKWIN, WM_CF_SHOW, 0, 0);

                    initTetrominos();
                    initGame();
                    game_minutes = 0;
                    game_seconds = 0;
                    time_value = 0;
                    clock_running = 1;

                    hTextClock = TEXT_CreateEx(160, 126, 70, 21, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT1, "00:00");
                    TEXT_SetFont(hTextClock, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextClock, GUI_WHITE);
                    TEXT_SetTextAlign(hTextClock, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    WM_CreateTimer(WM_HBKWIN, CLOCK_TIMER_ID, 1000, 0);
                    WM_SetCallback(WM_HBKWIN, _cbClockTimer);

                    hTextScore = TEXT_CreateEx(160, 199, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT4, "0");
                    TEXT_SetFont(hTextScore, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextScore, GUI_WHITE);
                    TEXT_SetTextAlign(hTextScore, GUI_TA_HCENTER | GUI_TA_VCENTER);

                    hTextLines = TEXT_CreateEx(160, 235, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT5, "0");
                    TEXT_SetFont(hTextLines, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextLines, GUI_WHITE);
                    TEXT_SetTextAlign(hTextLines, GUI_TA_HCENTER | GUI_TA_VCENTER);

                    hTextLevel = TEXT_CreateEx(160, 163, 70, 15, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT6, "1");
                    TEXT_SetFont(hTextLevel, GUI_FONT_6X8);
                    TEXT_SetTextColor(hTextLevel, GUI_WHITE);
                    TEXT_SetTextAlign(hTextLevel, GUI_TA_HCENTER | GUI_TA_VCENTER);

                    currentState = STATE_PLAY;
                    break;

                default:
                    break;
            }
        }

        // === IN-GAME CONTROLS (STATE_PLAY) ===
        if (currentState == STATE_PLAY) {
            if (PG2 == 0 && clock_running) { rotateBlock(); GUI_X_Delay(200); } // UP
            if (PC10 == 0 && clock_running) { hardDrop(); GUI_X_Delay(200); }   // DOWN
            if (PC9 == 0 && clock_running) { moveLeft(); GUI_X_Delay(200); }    // LEFT
            if (PG4 == 0 && clock_running) { moveRight(); GUI_X_Delay(200); }   // RIGHT
            updateGame();
        }

        // === NAME ENTRY CONTROLS (STATE_GAME_OVER) ===
        if (currentState == STATE_GAME_OVER) {
            if (PG4 == 0) { selectedChar = (selectedChar + 1) % 5; updateNameUI(); GUI_X_Delay(200); }  // RIGHT
            if (PC9 == 0) { selectedChar = (selectedChar + 4) % 5; updateNameUI(); GUI_X_Delay(200); }  // LEFT
            if (PG2 == 0) { currentLetter[selectedChar] = (currentLetter[selectedChar] + 1) % 26; updateNameUI(); GUI_X_Delay(200); }  // UP
            if (PC10 == 0) { currentLetter[selectedChar] = (currentLetter[selectedChar] + 25) % 26; updateNameUI(); GUI_X_Delay(200); } // DOWN
        }

        GUI_X_Delay(10);  // General polling delay
    }
}
