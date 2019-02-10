#include <stddef.h>

#include <nrf.h>

#include <ble.h>
#include <nrf_nvic.h>
#include <nrf_sdm.h>

#include <SEGGER_RTT.h>

nrf_nvic_state_t nrf_nvic_state;

uint32_t err_code;

void SD_EVT_IRQHandler(void) {
    SEGGER_RTT_printf(0, "SD_EVT_IRQHandler\n");
}

void softdevice_enable(void) {
    nrf_clock_lf_cfg_t nrf_clock_lf_cfg = {
        .source = NRF_CLOCK_LF_SRC_XTAL,
        .rc_ctiv = 0,
        .rc_temp_ctiv = 0,
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM
    };
    err_code = sd_softdevice_enable(&nrf_clock_lf_cfg, NULL);
    SEGGER_RTT_printf(0, "sd_softdevice_enable: 0x%x\n", err_code);

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
            .service_changed = 1
        }
    };
    uint32_t app_ram_base = 0x200025e0;
    err_code = sd_ble_enable(&ble_enable_params, &app_ram_base);
    SEGGER_RTT_printf(0, "sd_ble_enable: 0x%x, app_ram_base = 0x%x\n", err_code, app_ram_base);
}

void ble_curiosity_check(void) {
    // default device name (spoiler: it's "nRF5x")
    uint8_t dev_name[BLE_GAP_DEVNAME_MAX_LEN + 1];
    uint16_t dev_name_len = BLE_GAP_DEVNAME_MAX_LEN;
    err_code = sd_ble_gap_device_name_get(dev_name, &dev_name_len);
    SEGGER_RTT_printf(0, "sd_ble_gap_device_name_get: 0x%x, len = %u\n", err_code, dev_name_len);
    if (err_code == NRF_SUCCESS) {
        dev_name[dev_name_len] = 0;
        SEGGER_RTT_printf(0, "\tdev_name: %s\n", dev_name);
    }

    // BLE stack version (spoiler, it's 4.2)
    ble_version_t ble_version;
    err_code = sd_ble_version_get(&ble_version);
    SEGGER_RTT_printf(0, "sd_ble_version_get: 0x%x\n", err_code);
    if (err_code == NRF_SUCCESS) {
        SEGGER_RTT_printf(0, "\tversion_number: %u\n", ble_version.version_number);
        SEGGER_RTT_printf(0, "\tcompany_id: %u\n", ble_version.company_id);
        SEGGER_RTT_printf(0, "\tsubversion_number: %u\n", ble_version.subversion_number);
    }
}

void ble_set_name(void) {
    ble_gap_conn_sec_mode_t write_perm = { .sm = 0, .lv = 0 };
    uint8_t name[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' };
    err_code = sd_ble_gap_device_name_set(&write_perm, name, 11);
    SEGGER_RTT_printf(0, "sd_ble_gap_device_name_set: 0x%x\n", err_code);
}

void ble_set_random_addr(void) {
    ble_gap_addr_t addr = {
        .addr_type = BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE
    };
    err_code = sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_AUTO, &addr);
    SEGGER_RTT_printf(0, "sd_ble_gap_address_set: %u\n", err_code);
}

void ble_gatts_setup(void) {
    ble_uuid_t ble_uuid = { .type = BLE_UUID_TYPE_BLE };
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t attr_md = { .vloc = BLE_GATTS_VLOC_STACK };
    ble_gatts_attr_t attr_char_value;

    ble_uuid.uuid = 0x180F;
    uint16_t svc_handle_battery;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &svc_handle_battery);
    SEGGER_RTT_printf(0, "sd_ble_gatts_service_add: 0x%x\n", err_code);

    ble_uuid.uuid = 0x2A19;
    char_md = (ble_gatts_char_md_t) {
        .char_props = {
            .read = true
        }
    };
    attr_md.read_perm = (ble_gap_conn_sec_mode_t) { .sm = 1, .lv = 1 };
    uint8_t battery_level = 0x10;
    attr_char_value = (ble_gatts_attr_t) {
        .p_uuid = &ble_uuid,
        .p_attr_md = &attr_md,
        .init_len = 1,
        .max_len = 1,
        .p_value = &battery_level
    };
    ble_gatts_char_handles_t char_handles_battery_level;
    err_code = sd_ble_gatts_characteristic_add(svc_handle_battery, &char_md, &attr_char_value, &char_handles_battery_level);
    SEGGER_RTT_printf(0, "sd_ble_gatts_characteristic_add: 0x%x\n", err_code);
}

void ble_adv_setup(void) {
    uint8_t data[] = {
        0x02, BLE_GAP_AD_TYPE_FLAGS, BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,
        0x06, BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, 'H', 'e', 'l', 'l', 'o'
    };
    err_code = sd_ble_gap_adv_data_set(data, sizeof(data), NULL, 0);
    SEGGER_RTT_printf(0, "sd_ble_gap_adv_data_set: 0x%x\n", err_code);
}

void ble_adv_start(void) {
    ble_gap_adv_params_t adv_params = {
        .type = BLE_GAP_ADV_TYPE_ADV_IND,
        .interval = 100 / 0.625
    };
    err_code = sd_ble_gap_adv_start(&adv_params);
    SEGGER_RTT_printf(0, "sd_ble_gap_adv_start: 0x%x\n", err_code);
}

void conn_param_update(uint16_t conn_handle) {
    // iOS puts caps on:
    // - max effective conn interval: <= 2 s
    // - slave latency: 30
    // - max sup timeout: 6 s

    // - max w/ slave latency
    // ble_gap_conn_params_t conn_params = {
    //     .min_conn_interval = 36,
    //     .max_conn_interval = 51,
    //     .slave_latency = 30,
    //     .conn_sup_timeout = 600
    // };

    // - max w/o slave latency
    ble_gap_conn_params_t conn_params = {
        .min_conn_interval = 1584,
        .max_conn_interval = 1599,
        .slave_latency = 0,
        .conn_sup_timeout = 600
    };

    err_code = sd_ble_gap_conn_param_update(conn_handle, &conn_params);
    SEGGER_RTT_printf(0, "sd_ble_gap_conn_param_update: 0x%x\n", err_code);
}

void print_data(uint8_t *data, uint16_t len) {
    for (int i = 0; i < len; i++) {
        SEGGER_RTT_printf(0, "%02x", data[i]);
    }
}

void print_gap_conn_params(ble_gap_conn_params_t conn_params) {
    SEGGER_RTT_printf(0, "\tconn_params.min_conn_interval: %u units\n", conn_params.min_conn_interval);
    SEGGER_RTT_printf(0, "\tconn_params.max_conn_interval: %u units\n", conn_params.max_conn_interval);
    SEGGER_RTT_printf(0, "\tconn_params.slave_latency: %u\n", conn_params.slave_latency);
    SEGGER_RTT_printf(0, "\tconn_params.conn_sup_timeout: %u ms\n", conn_params.conn_sup_timeout * 10);
}

void print_gap_evt_connected(uint16_t conn_handle, ble_gap_evt_connected_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_connected:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\tpeer_addr.addr_type: 0x%x\n", p.peer_addr.addr_type);
    SEGGER_RTT_printf(0, "\tpeer_addr.addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
        p.peer_addr.addr[0], p.peer_addr.addr[1], p.peer_addr.addr[2],
        p.peer_addr.addr[3], p.peer_addr.addr[4], p.peer_addr.addr[5]);
    SEGGER_RTT_printf(0, "\trole: 0x%x\n", p.role);
    SEGGER_RTT_printf(0, "\tirk_match: %u\n", p.irk_match);
    SEGGER_RTT_printf(0, "\tirk_match_idx: %u\n", p.irk_match_idx);
    print_gap_conn_params(p.conn_params);
}

void print_gap_evt_disconnected(uint16_t conn_handle, ble_gap_evt_disconnected_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_disconnected:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\treason: 0x%x\n", p.reason);
}

void print_gap_evt_conn_param_update(uint16_t conn_handle, ble_gap_evt_conn_param_update_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_conn_param_update:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    print_gap_conn_params(p.conn_params);
}

void print_gatts_evt_write(uint16_t conn_handle, ble_gatts_evt_write_t p) {
    SEGGER_RTT_printf(0, "ble_gatts_evt_write:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\thandle: 0x%x\n", p.handle);
    SEGGER_RTT_printf(0, "\tuuid.uuid: 0x%x\n", p.uuid.uuid);
    SEGGER_RTT_printf(0, "\tuuid.type: 0x%x\n", p.uuid.type);
    SEGGER_RTT_printf(0, "\top: 0x%x\n", p.op);
    SEGGER_RTT_printf(0, "\tauth_required: 0x%x\n", p.auth_required);
    SEGGER_RTT_printf(0, "\toffset: %u\n", p.offset);
    SEGGER_RTT_printf(0, "\tlen: %u\n", p.len);

    SEGGER_RTT_printf(0, "\tdata: ");
    print_data(p.data, p.len);
    SEGGER_RTT_printf(0, "\n");
}

int main(void) {
    softdevice_enable();
    // ble_curiosity_check();
    ble_set_name();
    // ble_set_random_addr();
    ble_gatts_setup();
    ble_adv_setup();

    ble_adv_start();

    err_code = sd_nvic_EnableIRQ(SD_EVT_IRQn);
    SEGGER_RTT_printf(0, "sd_nvic_EnableIRQ: 0x%x\n", err_code);

    union {
        uint8_t buf[sizeof(ble_evt_t) + GATT_MTU_SIZE_DEFAULT];
        ble_evt_t evt;
    } __attribute__ ((aligned (BLE_EVTS_PTR_ALIGNMENT))) ble_evt;
    uint16_t ble_evt_buf_size = sizeof(ble_evt);

    while (true) {
        err_code = sd_app_evt_wait();
        SEGGER_RTT_printf(0, "sd_app_evt_wait: 0x%x\n", err_code);

        bool has_events = true;
        do {
            uint16_t ble_evt_len = ble_evt_buf_size;
            err_code = sd_ble_evt_get(ble_evt.buf, &ble_evt_len);
            SEGGER_RTT_printf(0, "sd_ble_evt_get: 0x%x, ble_evt_len = %u\n", err_code, ble_evt_len);

            if (err_code == NRF_SUCCESS) {
                ble_evt_hdr_t h = ble_evt.evt.header;
                ble_evt_t e = ble_evt.evt;

                SEGGER_RTT_printf(0, "ble_evt.header: evt_id = 0x%x, evt_len = %u\n", h.evt_id, h.evt_len);

                switch (h.evt_id) {
                    case BLE_GAP_EVT_CONNECTED:
                    print_gap_evt_connected(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.connected);
                    conn_param_update(e.evt.gap_evt.conn_handle);
                    break;

                    case BLE_GAP_EVT_DISCONNECTED:
                    print_gap_evt_disconnected(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.disconnected);
                    ble_adv_start();
                    break;

                    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
                    print_gap_evt_conn_param_update(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.conn_param_update);
                    break;

                    case BLE_GATTS_EVT_WRITE:
                    print_gatts_evt_write(e.evt.gatts_evt.conn_handle, e.evt.gatts_evt.params.write);
                    break;
                }
            }

            if (err_code == NRF_ERROR_NOT_FOUND) {
                has_events = false;
            }
        } while (has_events);
    }
}
