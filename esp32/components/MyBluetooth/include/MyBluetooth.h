#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "nvs.h"

namespace MyStuff {
////////////////////////////////////////////////////////////////////////////////
//! Bluetooth helper - separate non-template dependent MyBluetooth functions
class MyBTHelpers {

public: // Public Constants
    static constexpr char kDevName[11] {"SPACES_ESP"};
    static constexpr esp_spp_mode_t kEspSppMode = ESP_SPP_MODE_CB;
    static constexpr esp_spp_sec_t kSecMask = ESP_SPP_SEC_AUTHENTICATE;
    static constexpr esp_spp_role_t kRole = ESP_SPP_ROLE_MASTER;

    // TODO: Add pin to get BT connected - likely need to use GAP
    static constexpr uint8_t kPinLen = 4;
    static constexpr esp_bt_pin_code_t kMyPin {5, 5, 5, 5};

    //! Log tag for GAP events
    static constexpr char GAP_LOG[] {"GAP"};
    //! Log tag for SPP events
    static constexpr char SPP_LOG[] {"SPP"};

public: // Public Interfaces & Functions
    //! Class to mimic ensure an Rx message only gets queued if valid
    class RxMsgIntfc {
        //! Return the size of the configured rx_q
        static uint16_t configdQueueItemSize() { return 0; }

        //! Verify that data casted to this type is valid per implementation
        //!  - Up to the implementor that is passed in as MyBluetooth's RxMsgT
        static bool dataIsValid(void* data, uint16_t data_len) { return false; }
    };

    //! Function to copy the desired pin into a buffer
    static void getPin( uint8_t pin_buff[MyBTHelpers::kPinLen] ) {
        for( uint8_t i=0; i < MyBTHelpers::kPinLen; i++ ) {
           pin_buff[i] = MyBTHelpers::kMyPin[i];
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
//! Class to receive BT Serial Port Profile (SPP) messages into a queue
//! @tparam RxMsgT - Should implement all of MyBTHelpers::RxMsgIntfc functions
//!                   to filter RX'd messages before adding them to the queue
template<typename RxMsgT>
class MySPPBluetooth
{
public:
    //! Ctor @param rx_q Pre-initialized queue to place new, valid messages into
    MySPPBluetooth(QueueHandle_t rx_q);

private:
    //! Callback for SPP BT
    static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

    //! Queue to store received messages
    static QueueHandle_t _rx_q;

    //! Shorten reference to MyBTHelpers
    using BT = MyBTHelpers;
};
template<typename RxMsgT> QueueHandle_t MySPPBluetooth<RxMsgT>::_rx_q;

//! Include template implementation
#include "src_tpp/MyBluetooth.tpp"

} // End namespace MyStuff