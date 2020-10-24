#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "driver/uart.h"

#include "MyBluetooth.h"

// Used to not start until the developer is ready
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

extern "C" void app_main()
{
    //WaitForUser();

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("Starting! This ESP32 chip has %d CPU cores, WiFi%s%s, btw ;)\n",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    MyBluetooth bluetooth;
    
    // TODO Have log starting byte so that python just prints the message
        uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = false // Deprecated but avoid comp. warning
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    QueueHandle_t uart_rx_q = nullptr;
    uart_driver_install(UART_NUM_0, 4096, 8192, 10, &uart_rx_q, 0);
    //xTaskCreate(uart_task, "uTask", 2048, (void*)UART_NUM_0, 8, NULL)

    //WaitAndRestart();

    // TODO - Should be a suspend function somewhere...
    //uint32_t usDelay = 10000000;
    TickType_t MS_DELAY = 100 / portTICK_PERIOD_MS;
    while(true){
        uint8_t data[] = {0x55, 0x55, '\n'};
        uart_write_bytes(UART_NUM_0, (const char*)&data, sizeof(data));
        vTaskDelay( MS_DELAY );
    }
}
