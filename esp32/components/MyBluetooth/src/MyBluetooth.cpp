#include "MyBluetooth.h"

using namespace MyStuff;

////////////////////////////////////////////////////////////////////////////////
//! Static Inits
constexpr esp_bt_pin_code_t MyBTHelpers::kMyPin;
constexpr char MyBTHelpers::kDevName[11];
constexpr esp_spp_mode_t MyBTHelpers::kEspSppMode;
constexpr esp_spp_sec_t MyBTHelpers::kSecMask;
constexpr esp_spp_role_t MyBTHelpers::kRole;
constexpr char MyBTHelpers::GAP_LOG[];
constexpr char MyBTHelpers::SPP_LOG[];