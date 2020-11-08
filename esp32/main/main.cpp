
#include "MyHelpers.h"
#include "MyBluetooth.h"
#include "MySerial.h"

//! Implement the required MyBluetooth RxMsgIntfc interface for CRTP
struct CommMsg {
    struct ExpectedData {
        uint8_t hdr[2] {0x00, 0x00};
        uint8_t cmd {0x00};
    } data;

    static uint16_t configdQueueItemSize() { return sizeof(ExpectedData); };

    static bool dataIsValid(void* data, uint16_t data_len) {
        if( !data || data_len != sizeof(ExpectedData) )
            return false;
        
        ExpectedData* new_data = (ExpectedData*)data;
        bool hdr_valid = (new_data->hdr[0] == 0x55 && new_data->hdr[1] == 0x54);
        return hdr_valid;
    }
};

extern "C" void app_main()
{
    using namespace MyStuff;
    //MyHelpers::WaitForUser(); //!< Uncomment to see start up info 
    MyHelpers::startAndPrintESPSystem();

    // Create queues for bridging the serial / bluetooth services
    QueueHandle_t bt_to_uart_q = nullptr;
    bt_to_uart_q = xQueueCreate( 10, CommMsg::configdQueueItemSize() );
    QueueHandle_t uart_rx_q = nullptr;

    // Config / start serial service
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = false // Deprecated but avoid comp. warning
    };

    MySerial<CommMsg, CommMsg> serial_port(UART_NUM_0, uart_config,
                               bt_to_uart_q, uart_rx_q, 1);
    serial_port.setEndingChar('\n');
    if( not serial_port.isConfigd() )
        printf("Failed to configure serial port! \n");

    // Give some time for the task to initialize before checking...
    TickType_t MS_DELAY = 100 / portTICK_PERIOD_MS;
    vTaskDelay( MS_DELAY );
    if( not serial_port.tasksRunning() )
        printf("Failed to start the serial tasks! \n");

    // Start Bluetooth service
    MySPPBluetooth<CommMsg> bluetooth(bt_to_uart_q);

    printf("Suspending app_main to allows tasks to run... \n");
    vTaskSuspend(nullptr);
}
