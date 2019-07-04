#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

#define GAP_TAG          "GAP"

class MyBluetooth
{
public:
    MyBluetooth();
    ~MyBluetooth();

private:
    void bt_app_gap_start_up(void);
    static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
    static void update_device_info(esp_bt_gap_cb_param_t *param);
    static bool get_name_from_eir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len);
    static char *uuid2str(esp_bt_uuid_t *uuid, char *str, size_t size);
    static char *bda2str(esp_bd_addr_t bda, char *str, size_t size);

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

    static app_gap_cb_t m_dev_info;
};
