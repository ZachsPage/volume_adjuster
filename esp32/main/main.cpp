#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "MyBluetooth.h"

extern "C" void app_main()
{
    printf("Starting once 'c' is pressed...\n");
    TickType_t xDelay = 200 / portTICK_PERIOD_MS;
    int i = 0;
    char ch = '\0';

    while( ch != 'c' )
    {
        fflush(stdin);
        vTaskDelay( xDelay );
        i++;
        if(i == 20)
        {
            printf("Waiting for user to enter 'c'\n");
            i = 0;
        }
        ch = getchar();
    }

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("Starting! This ESP32 chip has %d CPU cores, WiFi%s%s, btw ;)\n",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    MyBluetooth bluetooth;

    i = 40;
    xDelay = 1000 / portTICK_PERIOD_MS;
    printf("Restarting in... ");
    while( i >= 0 )
    {
        printf("%d ", i);
        fflush(stdout);
        i--;
        vTaskDelay( xDelay );
    }

    printf("\nRestarting!\n");
    fflush(stdout);
    esp_restart();
}
