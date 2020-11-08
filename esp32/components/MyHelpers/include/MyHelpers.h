#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

#include <stdio.h>

namespace MyStuff {
namespace MyHelpers {
////////////////////////////////////////////////////////////////////////////////
// Init the ESP non-vol storage and print the chip's specs
bool startAndPrintESPSystem()
{
    // Initialize non-vol storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        return false;
    }
    ESP_ERROR_CHECK( ret );

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("Starting! This ESP32 chip has %d CPU cores, WiFi%s%s, btw ;)\n",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    return true;
}
////////////////////////////////////////////////////////////////////////////////
// Used to not start until the developer is ready - good for checking startup
void WaitForUser()
{
    printf("Starting once 'c' is pressed...\n");
    TickType_t MS_DELAY = 200 / portTICK_PERIOD_MS;
    int i = 0;
    char ch = '\0';

    while( ch != 'c' ) {
        fflush(stdin);
        vTaskDelay( MS_DELAY );
        i++;
        if(i == 20) {
            printf("Waiting for user to enter 'c'\n");
            i = 0;
        }
        ch = getchar();
    }
}
////////////////////////////////////////////////////////////////////////////////
// Used to restart device after 
void WaitAndRestart()
{
    int seconds = 60;
    TickType_t ONE_SEC = 1000 / portTICK_PERIOD_MS;
    printf("Restarting in... ");
    while( seconds >= 0 ) {
        printf("%d ", seconds);
        fflush(stdout);
        seconds--;
        vTaskDelay( ONE_SEC );
    }

    printf("\nRestarting!\n");
    fflush(stdout);
    esp_restart();
}

}} // End namespace MyHelpers & MyStuff