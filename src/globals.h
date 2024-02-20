#ifndef globals_h
#define globals_h

#include "file_system.h"
#include <time.h>
/* Defines */

enum DEVICE_NAME
{
    MonBot_LITE = 0,
    MonBot_BOSS,
};
#define DEVICE MonBot_LITE
#define VERSION "1.0.9"

#define ever ;;
/* Macros */
#define taskWait(X) vTaskDelay(X / portTICK_PERIOD_MS)
#define TASK_HIGH_WATERMARK() \
do { \
    uint32_t highWatermark = uxTaskGetStackHighWaterMark(NULL); \
    if (highWatermark < __UINT32_MAX__) { \
        ESP_LOGI("Usage of Memory", "FREE MEMORY : %u", highWatermark); \
    } \
} while (0)

/* WS Terminal */
bool isConnectedToInternet = false;
bool isConnectedToMQTT = false;

const uint16_t MSG_LIST_MAX = 100;
String log_list[MSG_LIST_MAX];
volatile uint16_t log_list_count = 0;
volatile bool messageFlag = true;

void clearMessage()
{
    for (uint8_t i = 0; i < MSG_LIST_MAX; i++)
    {
        log_list[i][0] = '\0';
        log_list_count = 0;
    }
    return;
}
bool checkLoop()
{
    // static time_t last_time = now();
    // if (messageFlag)
    // {
    //     last_time = now();
    // }
    // messageFlag = false;
    // return (difftime(now(), last_time) <= 50);
    return false;
}

void sendToTerminal(const char *tag, const char *format, ...)
{
    char log_text[256];
    if (checkLoop())
    {
        va_list args;
        va_start(args, format);
        vsnprintf(log_text, sizeof(log_text), format, args);
        va_end(args);
        if (log_list_count < MSG_LIST_MAX)
        {
            String log_entry = String("[") + tag + String("] ") + String(log_text) + String("\n");
            log_list[log_list_count] = log_entry;
            log_list_count++;
        }
    }
    else
    {
        clearMessage();
    }
    return;
}

String concatenateMessage()
{
    String log_string = "";
    for (uint16_t i = 0; i < log_list_count; i++)
    {
        log_string += log_list[i];
    }
    return log_string;
}

#endif