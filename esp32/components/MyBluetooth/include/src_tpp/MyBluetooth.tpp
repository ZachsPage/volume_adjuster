
////////////////////////////////////////////////////////////////////////////////
template<typename RxMsgT>
MySPPBluetooth<RxMsgT>::MySPPBluetooth(QueueHandle_t rx_q)
{
    //! Set RX queue for receving SPP messages
    MySPPBluetooth<RxMsgT>::_rx_q = rx_q;

    esp_err_t ret = ESP_FAIL;
    ESP_ERROR_CHECK( esp_bt_controller_mem_release(ESP_BT_MODE_BLE) );
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if( (ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK ) {
        ESP_LOGE(BT::GAP_LOG, "Init controller failed!: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK ) {
        ESP_LOGE(BT::GAP_LOG, "Enable controller failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_bluedroid_init()) != ESP_OK ) {
        ESP_LOGE(BT::GAP_LOG, "Init bluedroid failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_bluedroid_enable()) != ESP_OK ) {
        ESP_LOGE(BT::GAP_LOG, "Enable bluedroid failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if( (ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK ) {
        ESP_LOGE(BT::SPP_LOG, "Spp register failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(BT::kEspSppMode)) != ESP_OK) {
        ESP_LOGE(BT::SPP_LOG, "Spp init failed: %s\n", esp_err_to_name(ret));
        return;
    }
}
////////////////////////////////////////////////////////////////////////////////
template<typename RxMsgT>
void MySPPBluetooth<RxMsgT>::esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t* param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(BT::SPP_LOG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(BT::kDevName);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(BT::kSecMask, BT::kRole, 0, BT::kDevName);
        break;
    case ESP_SPP_DATA_IND_EVT:
        if( param->data_ind.len != RxMsgT::configdQueueItemSize() ) {
            ESP_LOGI(BT::SPP_LOG, "Message different size (%d) than queue item (%d)\n",
                        param->data_ind.len, RxMsgT::configdQueueItemSize());
            break;
        }

        if( RxMsgT::dataIsValid(param->data_ind.data, param->data_ind.len) ) {
            ESP_LOGI(BT::SPP_LOG, "Adding valid data");
            xQueueSendToFront( _rx_q, param->data_ind.data, 0 );
        } else {
            ESP_LOGI(BT::SPP_LOG, "Invalid data message received... ignoring \n");
        }
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(BT::SPP_LOG, "New Client - Handle %d", param->srv_open.handle);
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(BT::SPP_LOG, "ESP_SPP_WRITE_EVT, status %d, handle %d, len %d",
                 param->write.status, param->write.handle, param->write.len);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(BT::SPP_LOG, "Spp Discovery started");
        break;
    case ESP_SPP_OPEN_EVT: ESP_LOGI(BT::SPP_LOG, "Client lost"); break;
    case ESP_SPP_CLOSE_EVT: ESP_LOGI(BT::SPP_LOG, "ESP_SPP_CLOSE_EVT"); break;
    case ESP_SPP_START_EVT: ESP_LOGI(BT::SPP_LOG, "ESP_SPP_START_EVT"); break;
    case ESP_SPP_CL_INIT_EVT: ESP_LOGI(BT::SPP_LOG, "ESP_SPP_CL_INIT_EVT"); break;
    case ESP_SPP_CONG_EVT: ESP_LOGI(BT::SPP_LOG, "ESP_SPP_CONG_EVT"); break;
    default: break;
    }
}