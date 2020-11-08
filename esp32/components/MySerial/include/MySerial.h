#include <stdint.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <atomic>

namespace MyStuff {

template<typename RxMsgT, typename TxMsgT>
class MySerial {
public:
    //! @param uart_num ESP define like UART_NUM_0
    //! @param config Desired port configuration structure
    //! @param tx_q Queue to pull data from to transmit - should already be xQueueCreated
    //! @param rx_q Queue to push data into once received - will be xQueueCreated
    MySerial(uart_port_t uart_num, const uart_config_t& config,
             QueueHandle_t tx_q, QueueHandle_t rx_q, uint16_t rx_q_items) :
        _uart_num(uart_num),
        _tx_q(tx_q),
        _rx_q(rx_q)
    {
        uart_param_config(uart_num, &config);
        uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                      UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        int tx_buffer_size = 0; // Block until data is sent
        int internal_rx_buff_size = 129; // Must be > than UART_FIFO_LEN (128)
        int no_irq_flags = 0;
        esp_err_t ret = uart_driver_install(uart_num, internal_rx_buff_size,
                            tx_buffer_size, rx_q_items, &_rx_q, no_irq_flags);
        if( ret != ESP_OK ) {
            ESP_LOGE("MySerial", "Init failed!: %s\n", esp_err_to_name(ret));
            return;
        }
        _config_okay = true;

        //! Start TX task...
        uint32_t tx_task_stack = 2096; // 512 was too small... 1048 overflows
                                        // when to BT connects...
        uint32_t tx_task_priority = 8; // TODO: check freeRtos priority scaling..
        xTaskCreate(_startTxTask, "SerialTxTask", tx_task_stack, (void*)this, 
                    tx_task_priority, &_tx_task_handle);
        if( _tx_task_handle == NULL ) {
            ESP_LOGE("MySerial", "Failed to start TX task\n");
            return;
        }

        //! TODO: Receive task not implemented since its not needed yet
    }
    //! @return True if serial was configured correctly
    bool isConfigd() { return _config_okay; }

    //! @return True serial tasks have been started - may have to sleep before
    //!   checking as threads take time to spool up
    bool tasksRunning() { return _tasks_running; }
    
    //! @param ending_char If called, TX's after every message pulled from Q
    void setEndingChar(char ending_char ) {
        _ending_char = ending_char; 
        _send_ending_char = true;
    }

private:
    static void _startTxTask(void* this_obj_arg) {
        MySerial<RxMsgT, TxMsgT>* started_obj =
            static_cast<MySerial<RxMsgT, TxMsgT>*>(this_obj_arg);
        if( started_obj->isConfigd() )
            started_obj->_txTask();
    }

    void _txTask() {
        _tasks_running = true;
        constexpr TickType_t MS_TIMEOUT = 1000 / portTICK_PERIOD_MS;
        while( true ) {
            if( xQueueReceive(_tx_q, (void*)&_tx_rcv_buff, MS_TIMEOUT) ) {
                ESP_LOGI("MySerial", "Forwarding rcv'd msg to serial \n");
                uart_write_bytes(_uart_num, (const char*)&_tx_rcv_buff,
                                    sizeof(_tx_rcv_buff));
                if( _send_ending_char )
                    uart_write_bytes(_uart_num, &_ending_char, sizeof(_ending_char));
            }
        }
    }

    uart_port_t _uart_num;
    //! Flag to track ctor status
    bool _config_okay {false};
    //! Flag to track starting tasks status
    std::atomic<bool> _tasks_running {false};

    //! Queue to store messages to transmit
    QueueHandle_t _tx_q;
    //! Queue to store received messages 
    QueueHandle_t _rx_q;
    //! Tx item buff to transfer from _tx_q
    TxMsgT _tx_rcv_buff;
    TaskHandle_t _tx_task_handle {nullptr};
    //! Send extra char sent over serial after every message
    bool _send_ending_char {false};
    //! Extra char sent over serial after every message
    char _ending_char {'\0'};
};

} // End namespace MyStuff
