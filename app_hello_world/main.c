#include <stddef.h>

#include <nrf.h>

#include <ble.h>
#include <nrf_nvic.h>
#include <nrf_sdm.h>

#include <SEGGER_RTT.h>

#include "P256-cortex-ecdh.h"

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
    uint8_t name[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', ',', ' ', 'h', 'o', 'w', ' ', 'a', 'r', 'e', ' ', 'y', 'o', 'u', ' ', 't', 'o', 'd', 'a', 'y', '?' };
    err_code = sd_ble_gap_device_name_set(&write_perm, name, 31);
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
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&attr_md.read_perm);
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

ble_gap_lesc_p256_pk_t debug_pk = { .pk = {
    0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc, 0xdb, 0xfd, 0xf4, 0xac,
    0x11, 0x91, 0xf4, 0xef, 0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
    0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20, 0x8b, 0xd2, 0x89, 0x15,
    0xd0, 0x8e, 0x1c, 0x74, 0x24, 0x30, 0xed, 0x8f, 0xc2, 0x45, 0x63, 0x76,
    0x5c, 0x15, 0x52, 0x5a, 0xbf, 0x9a, 0x32, 0x63, 0x6d, 0xeb, 0x2a, 0x65,
    0x49, 0x9c, 0x80, 0xdc
}};

uint8_t debug_sk[] = {
    0xbd, 0x1a, 0x3c, 0xcd, 0xa6, 0xb8, 0x99, 0x58, 0x99, 0xb7, 0x40, 0xeb,
    0x7b, 0x60, 0xff, 0x4a, 0x50, 0x3f, 0x10, 0xd2, 0xe3, 0xb3, 0xc9, 0x74,
    0x38, 0x5f, 0xc5, 0xa3, 0xd4, 0xf6, 0x49, 0x3f
};

ble_gap_enc_key_t own_enc_key;
ble_gap_id_key_t own_id_key;

ble_gap_enc_key_t peer_enc_key;
ble_gap_id_key_t peer_id_key;
ble_gap_lesc_p256_pk_t peer_pk;

ble_gap_sec_keyset_t sec_keyset = {
    .keys_own = {
        .p_enc_key = &own_enc_key,
        .p_id_key = &own_id_key,
        .p_pk = &debug_pk
    },
    .keys_peer = {
        .p_enc_key = &peer_enc_key,
        .p_id_key = &peer_id_key,
        .p_pk = &peer_pk
    }
};

void sec_params_reply(uint16_t conn_handle) {
    ble_gap_sec_params_t sec_params = {
        .bond = 0,
        .mitm = 0,
        .lesc = 0,
        .keypress = 0,
        .io_caps = BLE_GAP_IO_CAPS_NONE,
        .oob = 0,
        .min_key_size = 7,
        .max_key_size = 16,
        .kdist_own = { .enc = 1, .id = 1, .sign = 0, .link = 0 },
        .kdist_peer = { .enc = 1, .id = 1, .sign = 0, .link = 0 }
    };
    err_code = sd_ble_gap_sec_params_reply(conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &sec_params, &sec_keyset);
    SEGGER_RTT_printf(0, "sd_ble_gap_sec_params_reply: 0x%x\n", err_code);
}

void sec_info_reply(uint16_t conn_handle) {
    err_code = sd_ble_gap_sec_info_reply(conn_handle, &own_enc_key.enc_info, NULL, NULL);
    SEGGER_RTT_printf(0, "sd_ble_gap_sec_info_reply: 0x%x\n", err_code);
}

void lesc_dhkey_reply(uint16_t conn_handle) {
    ble_gap_lesc_dhkey_t dhkey;
    if (!P256_ecdh_shared_secret(dhkey.key, peer_pk.pk, debug_sk)) {
        SEGGER_RTT_printf(0, "P256_ecdh_shared_secret FAILED\n");
    } else {
        err_code = sd_ble_gap_lesc_dhkey_reply(conn_handle, &dhkey);
        SEGGER_RTT_printf(0, "sd_ble_gap_lesc_dhkey_reply: 0x%x\n", err_code);
    }
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

void print_gap_sec_kdist(ble_gap_sec_kdist_t sec_kdist) {
    SEGGER_RTT_printf(0, "\t\tsec_kdist.enc: %u\n", sec_kdist.enc);
    SEGGER_RTT_printf(0, "\t\tsec_kdist.id: %u\n", sec_kdist.id);
    SEGGER_RTT_printf(0, "\t\tsec_kdist.sign: %u\n", sec_kdist.sign);
    SEGGER_RTT_printf(0, "\t\tsec_kdist.link: %u\n", sec_kdist.link);
}

void print_gap_sec_params(ble_gap_sec_params_t sec_params) {
    SEGGER_RTT_printf(0, "\tsec_params.bond: %u\n", sec_params.bond);
    SEGGER_RTT_printf(0, "\tsec_params.mitm: %u\n", sec_params.mitm);
    SEGGER_RTT_printf(0, "\tsec_params.lesc: %u\n", sec_params.lesc);
    SEGGER_RTT_printf(0, "\tsec_params.keypress: %u\n", sec_params.keypress);
    SEGGER_RTT_printf(0, "\tsec_params.io_caps: %u\n", sec_params.io_caps);
    SEGGER_RTT_printf(0, "\tsec_params.min_key_size: %u\n", sec_params.min_key_size);
    SEGGER_RTT_printf(0, "\tsec_params.max_key_size: %u\n", sec_params.max_key_size);
    SEGGER_RTT_printf(0, "\tsec_params.kdist_own:\n");
    print_gap_sec_kdist(sec_params.kdist_own);
    SEGGER_RTT_printf(0, "\tsec_params.kdist_peer:\n");
    print_gap_sec_kdist(sec_params.kdist_peer);
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

void print_gap_evt_sec_params_request(uint16_t conn_handle, ble_gap_evt_sec_params_request_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_sec_params_request:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    print_gap_sec_params(p.peer_params);
}

void print_gap_evt_sec_info_request(uint16_t conn_handle, ble_gap_evt_sec_info_request_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_sec_info_request:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\tpeer_addr.addr_type: 0x%x\n", p.peer_addr.addr_type);
    SEGGER_RTT_printf(0, "\tpeer_addr.addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
        p.peer_addr.addr[0], p.peer_addr.addr[1], p.peer_addr.addr[2],
        p.peer_addr.addr[3], p.peer_addr.addr[4], p.peer_addr.addr[5]);
    SEGGER_RTT_printf(0, "\tmaster_id: ediv: %u, rand[0]: %u\n", p.master_id.ediv, p.master_id.rand[0]);
    SEGGER_RTT_printf(0, "\tenc_info: %u\n", p.enc_info);
    SEGGER_RTT_printf(0, "\tid_info: %u\n", p.id_info);
    SEGGER_RTT_printf(0, "\tsign_info: %u\n", p.sign_info);
}

void print_gap_evt_lesc_dhkey_request(uint16_t conn_handle, ble_gap_evt_lesc_dhkey_request_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_lesc_dhkey_request:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\toobd_req: %u\n", p.oobd_req);
    SEGGER_RTT_printf(0, "\tp_pk_peer: "); print_data(p.p_pk_peer->pk, 64); SEGGER_RTT_printf(0, "\n");
}

void print_gap_evt_auth_status(uint16_t conn_handle, ble_gap_evt_auth_status_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_auth_status:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\tauth_status: 0x%x\n", p.auth_status);
    SEGGER_RTT_printf(0, "\terror_src: 0x%x\n", p.error_src);
    SEGGER_RTT_printf(0, "\tbonded: %u\n", p.bonded);
    SEGGER_RTT_printf(0, "\tsm1_levels: lv1: %u, lv2: %u, lv3: %u, lv4: %u\n",
        p.sm1_levels.lv1, p.sm1_levels.lv2, p.sm1_levels.lv3, p.sm1_levels.lv4);
    SEGGER_RTT_printf(0, "\tsm2_levels: lv1: %u, lv2: %u, lv3: %u, lv4: %u\n",
        p.sm2_levels.lv1, p.sm2_levels.lv2, p.sm2_levels.lv3, p.sm2_levels.lv4);
    SEGGER_RTT_printf(0, "\tkdist_own:\n");
    print_gap_sec_kdist(p.kdist_own);
    SEGGER_RTT_printf(0, "\tkdist_peer:\n");
    print_gap_sec_kdist(p.kdist_peer);

    SEGGER_RTT_printf(0, "\town_enc_key:\n");
    SEGGER_RTT_printf(0, "\t\tenc_info: lesc: %u, auth: %u, ltk_len: %u\n",
        own_enc_key.enc_info.lesc, own_enc_key.enc_info.auth, own_enc_key.enc_info.ltk_len);
    if (own_enc_key.enc_info.ltk_len > 0) {
        SEGGER_RTT_printf(0, "\t\tltk: "); print_data(own_enc_key.enc_info.ltk, own_enc_key.enc_info.ltk_len); SEGGER_RTT_printf(0, "\n");
    }
    SEGGER_RTT_printf(0, "\t\tmaster_id: ediv: %u, rand[0]: %u\n",
        own_enc_key.master_id.ediv, own_enc_key.master_id.rand[0]);

    SEGGER_RTT_printf(0, "\town_id_key:\n");
    SEGGER_RTT_printf(0, "\t\t"); print_data(own_id_key.id_info.irk, 16); SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "\t\tid_addr_info.addr_type: 0x%x\n", own_id_key.id_addr_info.addr_type);
    SEGGER_RTT_printf(0, "\t\tid_addr_info.addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
        own_id_key.id_addr_info.addr[0], own_id_key.id_addr_info.addr[1], own_id_key.id_addr_info.addr[2],
        own_id_key.id_addr_info.addr[3], own_id_key.id_addr_info.addr[4], own_id_key.id_addr_info.addr[5]);

    SEGGER_RTT_printf(0, "\tpeer_enc_key:\n");
    SEGGER_RTT_printf(0, "\t\tenc_info: lesc: %u, auth: %u, ltk_len: %u\n",
        peer_enc_key.enc_info.lesc, peer_enc_key.enc_info.auth, peer_enc_key.enc_info.ltk_len);
    if (peer_enc_key.enc_info.ltk_len > 0) {
        SEGGER_RTT_printf(0, "\t\tltk: "); print_data(peer_enc_key.enc_info.ltk, peer_enc_key.enc_info.ltk_len); SEGGER_RTT_printf(0, "\n");
    }
    SEGGER_RTT_printf(0, "\t\tmaster_id: ediv: %u, rand[0]: %u\n",
        peer_enc_key.master_id.ediv, peer_enc_key.master_id.rand[0]);

    SEGGER_RTT_printf(0, "\tpeer_id_key:\n");
    SEGGER_RTT_printf(0, "\t\t"); print_data(peer_id_key.id_info.irk, 16); SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "\t\tid_addr_info.addr_type: 0x%x\n", peer_id_key.id_addr_info.addr_type);
    SEGGER_RTT_printf(0, "\t\tid_addr_info.addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
        peer_id_key.id_addr_info.addr[0], peer_id_key.id_addr_info.addr[1], peer_id_key.id_addr_info.addr[2],
        peer_id_key.id_addr_info.addr[3], peer_id_key.id_addr_info.addr[4], peer_id_key.id_addr_info.addr[5]);
}

void print_gap_evt_conn_sec_update(uint16_t conn_handle, ble_gap_evt_conn_sec_update_t p) {
    SEGGER_RTT_printf(0, "ble_gap_evt_conn_sec_update:\n");
    SEGGER_RTT_printf(0, "\tconn_handle: 0x%x\n", conn_handle);
    SEGGER_RTT_printf(0, "\tconn_sec.sec_mode: sm: %u, lv: %u\n", p.conn_sec.sec_mode.sm, p.conn_sec.sec_mode.lv);
    SEGGER_RTT_printf(0, "\tconn_sec.encr_key_size: %u\n", p.conn_sec.encr_key_size);
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
                    // conn_param_update(e.evt.gap_evt.conn_handle);
                    break;

                    case BLE_GAP_EVT_DISCONNECTED:
                    print_gap_evt_disconnected(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.disconnected);
                    ble_adv_start();
                    break;

                    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
                    print_gap_evt_conn_param_update(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.conn_param_update);
                    break;

                    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
                    print_gap_evt_sec_params_request(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.sec_params_request);
                    sec_params_reply(e.evt.gap_evt.conn_handle);
                    break;

                    case BLE_GAP_EVT_SEC_INFO_REQUEST:
                    print_gap_evt_sec_info_request(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.sec_info_request);
                    sec_info_reply(e.evt.gap_evt.conn_handle);
                    break;

                    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
                    print_gap_evt_lesc_dhkey_request(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.lesc_dhkey_request);
                    lesc_dhkey_reply(e.evt.gap_evt.conn_handle);
                    break;

                    case BLE_GAP_EVT_AUTH_STATUS:
                    print_gap_evt_auth_status(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.auth_status);
                    break;

                    case BLE_GAP_EVT_CONN_SEC_UPDATE:
                    print_gap_evt_conn_sec_update(e.evt.gap_evt.conn_handle, e.evt.gap_evt.params.conn_sec_update);
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
