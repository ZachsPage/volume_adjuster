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

#define GAP_TAG          "GAP"
#define SPP_TAG          "SPP"

namespace MyStuff {
////////////////////////////////////////////////////////////////////////////////
//! Bluetooth helper - separate non-template dependent MyBluetooth functions
class MyBTHelpers {
public:
    static char *uuid2str(esp_bt_uuid_t *uuid, char *str, size_t size);
    static char *bda2str(esp_bd_addr_t bda, char *str, size_t size);

public:
    static constexpr char kDevName[11] {"SPACES_ESP"};
    static constexpr uint8_t kPinLen = 4;
    static constexpr esp_bt_pin_code_t kMyPin {5, 5, 5, 5};
    static constexpr esp_spp_mode_t kEspSppMode = ESP_SPP_MODE_CB;
    static constexpr esp_spp_sec_t kSecMask = ESP_SPP_SEC_AUTHENTICATE;
    //! Not ESP_SPP_ROLE_SLAVE 
    static constexpr esp_spp_role_t kRole = ESP_SPP_ROLE_MASTER;

public:
    //! Struct to inherit from to ensure an Rx message only gets queued if valid
    //! - User's data class should not actually inherit from this class such that
    //!   the user's class sizeof matches the size of the incoming data
    struct RxMsgIntfc {
        //! Verify that data casted to this type is valid per implementation
        virtual bool hdrValid() = 0;
    };

    typedef enum {
        APP_GAP_STATE_IDLE = 0,
        APP_GAP_STATE_DEVICE_DISCOVERING,
        APP_GAP_STATE_DEVICE_DISCOVER_COMPLETE,
        APP_GAP_STATE_SERVICE_DISCOVERING,
        APP_GAP_STATE_SERVICE_DISCOVER_COMPLETE,
    } app_gap_state_t;

    typedef struct {
        bool dev_found;
        uint8_t bdname_len;
        uint8_t eir_len;
        uint8_t rssi;
        uint32_t cod;
        uint8_t eir[ESP_BT_GAP_EIR_DATA_LEN];
        uint8_t bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
        esp_bd_addr_t bda;
        app_gap_state_t state;
    } app_gap_cb_t;
};

////////////////////////////////////////////////////////////////////////////////
//! Class to receive messages into a FreeRTOS queue - if valid through 
template<typename RxMsgT>
class MyBluetooth
{
public:
    MyBluetooth(QueueHandle_t rx_q);

    static void getPin( uint8_t pin_buff[MyBTHelpers::kPinLen] ) {
        for( uint8_t i=0; i < MyBTHelpers::kPinLen; i++ ) {
           pin_buff[i] = MyBTHelpers::kMyPin[i];
        }
    }

private:
    //! Callback for SPP BT
    static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    //! Setup for BT configuration
    void bt_app_gap_start_up(void);
    //! Callback for generic BT 
    static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

    //! Current BT object status - must have static to be update via CB...
    //!  - Can only have a single BT object per system anyway, but this template
    //!    class provides 
    static MyBTHelpers::app_gap_cb_t _m_dev_info;

    //! Queue to store received messages
    static QueueHandle_t _rx_q;
};

template<typename RxMsgT>
MyBTHelpers::app_gap_cb_t MyBluetooth<RxMsgT>::_m_dev_info;
template<typename RxMsgT> QueueHandle_t MyBluetooth<RxMsgT>::_rx_q;

//! Include template implementation
#include "src_tpp/MyBluetooth.tpp"

} // End namespace MyStuff