#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_system.h"
//#include "esp_spi_flash.h"

#include "MyBluetooth.h"
#include "MySerial.h"

//! Implement the required MyBluetooth RxMsgIntfc interface, but do not inherit
//!   as to leave the class size matching the actual data size - 3 bytes
//! TODO: Would be nice to define this in a shared header file for C++/Java/Python...
struct CommMsg {
    uint8_t hdr[2] {0x00, 0x00};
    uint8_t cmd {0x00};
    bool hdrValid() {
        bool hdr_valid = (hdr[0] == 0x55 && hdr[1] == 0x54);
        return hdr_valid;
    }
};

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
    using namespace MyStuff;
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

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = false // Deprecated but avoid comp. warning
    };

    QueueHandle_t bt_to_uart_q = nullptr;
    bt_to_uart_q = xQueueCreate( 10, sizeof(CommMsg) );
    QueueHandle_t uart_rx_q = nullptr;

    MySerial<CommMsg, CommMsg> serial_port(UART_NUM_0, uart_config,
                               bt_to_uart_q, uart_rx_q, 1);
    serial_port.setEndingChar('\n');
    MyBluetooth<CommMsg> bluetooth(bt_to_uart_q);

    TickType_t MS_DELAY = 100 / portTICK_PERIOD_MS;
    vTaskDelay( MS_DELAY );

    if( not serial_port.isConfigd() )
        printf("Failed to configure serial port! \n");

    if( not serial_port.tasksRunning() )
        printf("Failed to actually start the serial tasks! \n");

    // TODO - Should be a suspend function somewhere...
    printf("Suspending to allows tasks to run...\n");
    MS_DELAY = 100000 / portTICK_PERIOD_MS;
    while(true) {
        vTaskDelay( MS_DELAY );
    }
}
