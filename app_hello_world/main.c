#include <stddef.h>

#include <nrf.h>

#include <ble.h>
#include <nrf_sdm.h>

#include <SEGGER_RTT.h>

#define LED_1 21
#define LED_2 22
#define LED_3 23
#define LED_4 24

int main(void) {
    NRF_GPIO->PIN_CNF[LED_1] = 0x03;
    NRF_GPIO->OUTCLR = 1 << LED_1;

    uint32_t err_code;

    nrf_clock_lf_cfg_t nrf_clock_lf_cfg = {
        .source = NRF_CLOCK_LF_SRC_XTAL,
        .rc_ctiv = 0,
        .rc_temp_ctiv = 0,
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM
    };
    err_code = sd_softdevice_enable(&nrf_clock_lf_cfg, NULL);
    SEGGER_RTT_printf(0, "sd_softdevice_enable: err_code = %u\n", err_code);

    ble_enable_params_t ble_enable_params = {
        .common_enable_params = {
            .vs_uuid_count = 1,
            .p_conn_bw_counts = NULL
        },
        .gap_enable_params = {
            .periph_conn_count = 1,
            .central_conn_count = 1,
            .central_sec_count = 1
        },
        .gatts_enable_params = {
            .attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT,
            .service_changed = 0
        }
    };
    uint32_t app_ram_base = 0x200025e0;
    err_code = sd_ble_enable(&ble_enable_params, &app_ram_base);
    SEGGER_RTT_printf(0, "sd_ble_enable: err_code = %u, app_ram_base = %x\n", err_code, app_ram_base);
}
