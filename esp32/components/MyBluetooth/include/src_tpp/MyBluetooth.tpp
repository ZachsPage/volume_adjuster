////////////////////////////////////////////////////////////////////////////////
template<typename RxMsgT>
MyBluetooth<RxMsgT>::MyBluetooth(QueueHandle_t rx_q)
{
    MyBluetooth<RxMsgT>::_rx_q = rx_q;
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_err_t ret = ESP_FAIL;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if( (ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK ) {
        ESP_LOGE(GAP_TAG, "Init controller failed!: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK ) {
        ESP_LOGE(GAP_TAG, "Enable controller failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_bluedroid_init()) != ESP_OK ) {
        ESP_LOGE(GAP_TAG, "Init bluedroid failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_bluedroid_enable()) != ESP_OK ) {
        ESP_LOGE(GAP_TAG, "Enable bluedroid failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK ) {
        ESP_LOGE(SPP_TAG, "Spp register failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(MyBTHelpers::kEspSppMode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Spp init failed: %s\n", esp_err_to_name(ret));
        return;
    }

    bt_app_gap_start_up();
}
////////////////////////////////////////////////////////////////////////////////
template<typename RxMsgT>
void MyBluetooth<RxMsgT>::bt_app_gap_start_up(void)
{
    esp_bt_dev_set_device_name((char*)MyBTHelpers::kDevName);

    /* // TODO - Set up PIN for connection 
    uint8_t pin_buff[kPinLen];
    getPin(pin_buff);
    esp_err_t ret = esp_bt_gap_set_pin(ESP_BT_PIN_TYPE_FIXED, kPinLen, pin_buff);
    if( ret != ESP_OK ) {
        printf("Failed setting PIN, error %d", ret);
    } else {
        ESP_LOGI(GAP_TAG, "Set pin code: %d %d %d %d",
           pin_buff[0], pin_buff[1], pin_buff[2], pin_buff[3]);
    } */

    // Set discoverable and connectable mode, wait to be connected
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    // Register GAP callback function
    esp_bt_gap_register_callback(bt_app_gap_cb);

    // Start Bluetooth device discovery - if we get this far, clear state
    memset(&_m_dev_info, 0, sizeof(_m_dev_info));
    _m_dev_info.state = MyBTHelpers::APP_GAP_STATE_DEVICE_DISCOVERING;
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
}
////////////////////////////////////////////////////////////////////////////////
template<typename RxMsgT>
void MyBluetooth<RxMsgT>::esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t* param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name( MyBTHelpers::kDevName );
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(MyBTHelpers::kSecMask, MyBTHelpers::kRole, 0,
                          MyBTHelpers::kDevName);
        break;
    case ESP_SPP_DATA_IND_EVT:
        if( param->data_ind.len == sizeof(RxMsgT) ) {
            RxMsgT* rxd_data = (RxMsgT*)param->data_ind.data;
            if( rxd_data->hdrValid() ) {
                ESP_LOGI(SPP_TAG, "Adding valid data");
                xQueueSendToFront( _rx_q, param->data_ind.data, 0 );
            } else {
                ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT - invalid header");
            }
        } else {
            ESP_LOGI(SPP_TAG, "Invalid size message received - %d, expected %d",
                      param->data_ind.len, sizeof(RxMsgT) );
        }
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT, status %d, handle %d, len %d",
                 param->write.status, param->write.handle, param->write.len);
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT, handle %d, listen handle %d",
                    param->srv_open.handle, param->srv_open.new_listen_handle);
        break;
    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////
template<typename RxMsgT>
void MyBluetooth<RxMsgT>::bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param)
{
    using BTState = MyBTHelpers::app_gap_state_t;
    MyBTHelpers::app_gap_cb_t *p_dev = &_m_dev_info;
    char bda_str[18];
    char uuid_str[37];

    switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT: 
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_RES_EVT");
        esp_log_buffer_hex(SPP_TAG, param->disc_res.bda, ESP_BD_ADDR_LEN);
        esp_bd_addr_t peer_bd_addr;
        for (int i = 0; i < param->disc_res.num_prop; i++){
            if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR)
            {
                ESP_LOGI(SPP_TAG, "Starting SPP discovery on peer");
                memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
                esp_spp_start_discovery(peer_bd_addr);
                esp_bt_gap_cancel_discovery();
            }
        }
        break;
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
            ESP_LOGI(GAP_TAG, "Device discovery stopped.");
            if ( (p_dev->state == BTState::APP_GAP_STATE_DEVICE_DISCOVER_COMPLETE ||
                    p_dev->state == BTState::APP_GAP_STATE_DEVICE_DISCOVERING)
                    && p_dev->dev_found) {
                p_dev->state = BTState::APP_GAP_STATE_SERVICE_DISCOVERING;
                ESP_LOGI(GAP_TAG, "Discover services ...");
                esp_bt_gap_get_remote_services(p_dev->bda);
            }
        } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
            ESP_LOGI(GAP_TAG, "Discovery started.");
        }
        break;
    case ESP_BT_GAP_RMT_SRVCS_EVT: 
        if (memcmp(param->rmt_srvcs.bda, p_dev->bda, ESP_BD_ADDR_LEN) == 0 &&
                p_dev->state == BTState::APP_GAP_STATE_SERVICE_DISCOVERING) {
            p_dev->state = BTState::APP_GAP_STATE_SERVICE_DISCOVER_COMPLETE;
            if (param->rmt_srvcs.stat == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(GAP_TAG, "Services for device %s found", 
                            MyBTHelpers::bda2str(p_dev->bda, bda_str, 18));
                for (int i = 0; i < param->rmt_srvcs.num_uuids; i++) {
                    esp_bt_uuid_t *u = param->rmt_srvcs.uuid_list + i;
                    ESP_LOGI(GAP_TAG, "--%s", MyBTHelpers::uuid2str(u, uuid_str, 37));
                    // ESP_LOGI(GAP_TAG, "--%d", u->len);
                }
            } else {
                ESP_LOGI(GAP_TAG, "Services for device %s not found", 
                            MyBTHelpers::bda2str(p_dev->bda, bda_str, 18));
            }
        }
        break;
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        printf("\nAuth Complete Status %d From Device %s\n", 
                    param->auth_cmpl.stat, 
                    param->auth_cmpl.device_name);
        break;
    case ESP_BT_GAP_PIN_REQ_EVT:
        ESP_LOGI(GAP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(GAP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            uint8_t pin_buff[MyBTHelpers::kPinLen];
            getPin(pin_buff);
            ESP_LOGI(GAP_TAG, "Input pin code: %c %c %c %c",
                    pin_buff[0], pin_buff[1], pin_buff[2], pin_buff[3]);
            esp_bt_gap_pin_reply(param->pin_req.bda, true,
                                    sizeof(pin_buff), pin_buff);
        }
        break;
    default: 
        ESP_LOGI(GAP_TAG, "event: %d", event);
        break;
    }
    return;
}