var group__sdk__nrf__dfu__req__handler =
[
    [ "nrf_dfu_req_t", "structnrf__dfu__req__t.html", [
      [ "obj_type", "structnrf__dfu__req__t.html#aa39536e3382ec447f89b1f78085da8c0", null ],
      [ "object_size", "structnrf__dfu__req__t.html#a7884fde25086b0ae77ab61c1aa09dd8e", null ],
      [ "p_req", "structnrf__dfu__req__t.html#a1f0d63db77bdf18d18a125211b5a8d43", null ],
      [ "req_len", "structnrf__dfu__req__t.html#a7bf8b265597d608c2d14c24f3465f0fc", null ],
      [ "req_type", "structnrf__dfu__req__t.html#ac97ba2d265a6fbaa3431d21abb23822d", null ]
    ] ],
    [ "nrf_dfu_res_t", "structnrf__dfu__res__t.html", [
      [ "crc", "structnrf__dfu__res__t.html#a2b2c77b14de5fa6fecb904d9a6c72abb", null ],
      [ "max_size", "structnrf__dfu__res__t.html#a511b5d7c777182073e6f472a9801a96c", null ],
      [ "offset", "structnrf__dfu__res__t.html#aef0594664ac548e29d931e3124244099", null ],
      [ "p_res", "structnrf__dfu__res__t.html#a823365f82b349da67e468bba01f63f98", null ],
      [ "res_len", "structnrf__dfu__res__t.html#a1ec0dba90fc226a3086d39e5954bf9f5", null ]
    ] ],
    [ "nrf_dfu_obj_type_t", "group__sdk__nrf__dfu__req__handler.html#ga13e0184fedc2068008f85dfd997860d7", [
      [ "NRF_DFU_OBJ_TYPE_INVALID", "group__sdk__nrf__dfu__req__handler.html#gga13e0184fedc2068008f85dfd997860d7a7aa618f0ed350537b234d98a431826a5", null ],
      [ "NRF_DFU_OBJ_TYPE_COMMAND", "group__sdk__nrf__dfu__req__handler.html#gga13e0184fedc2068008f85dfd997860d7a107fd13ec2f04a7b524983ace3480e07", null ],
      [ "NRF_DFU_OBJ_TYPE_DATA", "group__sdk__nrf__dfu__req__handler.html#gga13e0184fedc2068008f85dfd997860d7adbc41d4f18e2395c89586155487c192f", null ]
    ] ],
    [ "nrf_dfu_req_op_t", "group__sdk__nrf__dfu__req__handler.html#gac718d0204baed25a1bef3d01484c59f6", [
      [ "NRF_DFU_OBJECT_OP_NONE", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6a41f28e345425abe35be569605127c036", null ],
      [ "NRF_DFU_OBJECT_OP_CREATE", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6a16f83f7891e8e828e0b00cfd7aa587ce", null ],
      [ "NRF_DFU_OBJECT_OP_WRITE", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6af59b92b16d58fc96513349b45b890ffe", null ],
      [ "NRF_DFU_OBJECT_OP_EXECUTE", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6ae3b628136bc65910bfe74650843a7a2e", null ],
      [ "NRF_DFU_OBJECT_OP_CRC", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6a1acff0e79d0d905301593134fdaba129", null ],
      [ "NRF_DFU_OBJECT_OP_SELECT", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6a63b33d606af8acb19521de83bc991065", null ],
      [ "NRF_DFU_OBJECT_OP_OTHER", "group__sdk__nrf__dfu__req__handler.html#ggac718d0204baed25a1bef3d01484c59f6a26a6e6769f47c371e23b92bb0d81c954", null ]
    ] ],
    [ "nrf_dfu_res_code_t", "group__sdk__nrf__dfu__req__handler.html#ga31217e9d52d12cb3d3dc3c2c5cba4211", [
      [ "NRF_DFU_RES_CODE_INVALID", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211a120c75995f7e2d57a0fa9f123ab38145", null ],
      [ "NRF_DFU_RES_CODE_SUCCESS", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211a8934202d8cf397e2fcc76dcef027e602", null ],
      [ "NRF_DFU_RES_CODE_OP_CODE_NOT_SUPPORTED", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211a24c219738e572a3044f8f4a5d6f6df7e", null ],
      [ "NRF_DFU_RES_CODE_INVALID_PARAMETER", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211acbe549f2e19090aeeba66bfad2fe766e", null ],
      [ "NRF_DFU_RES_CODE_INSUFFICIENT_RESOURCES", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211af54a2806ebcbd3d5b5675fe3d9a7d5c5", null ],
      [ "NRF_DFU_RES_CODE_INVALID_OBJECT", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211a09a912a599bf9e93c6ae5d4c16463d05", null ],
      [ "NRF_DFU_RES_CODE_UNSUPPORTED_TYPE", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211a597a549dba6ef7f94c1c492b63ee5d3e", null ],
      [ "NRF_DFU_RES_CODE_OPERATION_NOT_PERMITTED", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211a9fd490a7a3ca55ff97b709433d590a14", null ],
      [ "NRF_DFU_RES_CODE_OPERATION_FAILED", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211afaee5b61948735cefa97fb5762bb90a6", null ],
      [ "NRF_DFU_RES_CODE_EXT_ERROR", "group__sdk__nrf__dfu__req__handler.html#gga31217e9d52d12cb3d3dc3c2c5cba4211aecaaa8d2fb5da91322326ff54041afad", null ]
    ] ],
    [ "nrf_dfu_req_handler_init", "group__sdk__nrf__dfu__req__handler.html#gaf509d7dd7d172a391e757c831fac6407", null ],
    [ "nrf_dfu_req_handler_on_req", "group__sdk__nrf__dfu__req__handler.html#gaf013a692d389cc1f43a0cffbfde5e0a9", null ]
];