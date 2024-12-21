// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 Bobby Noelte
 */
#include <cstring>
#include <string>

// OpenDTU
#include "Hoymiles.h"
#include "Configuration.h"
#include "MessageOutput.h"
#include "Datastore.h"
#include "ModbusDtu.h"
#include "ModbusSettings.h"
#include "NetworkSettings.h"
#include "__compiled_constants.h"

// eModbus
#include "Logging.h"

// OpenDTU Total inverter
// - FC 0x03 requests (read holding registers)
ModbusMessage OpenDTUTotal(ModbusMessage request) {
    uint16_t addr = 0;          // Start address
    uint16_t words = 0;         // # of words requested

    const CONFIG_T& config = Configuration.get();

    // read address from request
    request.get(2, addr);
    // read # of words from request
    request.get(4, words);

    uint16_t response_size = words * 2 + 6;
    ModbusDTUMessage response(response_size);     // The Modbus message we are going to give back

    // get the version string from compiled constant
    std::string strVersion = static_cast<std::string>(__COMPILED_GIT_HASH__);
    size_t hyphen_pos = strVersion.find_first_of('-');
    if (hyphen_pos != std::string::npos) {
        strVersion = static_cast<std::string>(__COMPILED_GIT_HASH__).substr(0, hyphen_pos);
    }

    LOG_D("Request FC03 0x%04x:%d - response with size %d\n", (int)addr, (int)words, (int)response_size);

    if (addr >= 40000) {
        // SunSpec - OpenDTU Total inverter

        // Set up response
        response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(words * 2));

        // Complete response
        for (uint16_t reg = addr; reg < (addr + words); reg++) {
            if (reg < 40070) {
                // Model 1 - SunSpec Common Registers
                uint8_t reg_idx = reg - 40000;

                switch (reg_idx) {
                    case 0 ... 1:
                        // SunS
                        response.addString("SunS", reg_idx - 0);
                        break;
                    case 2:
                        // Model ID
                        response.addUInt16(1);
                        break;
                    case 3:
                        // SunSpec model register count (length without header (4))
                        response.addUInt16(66);
                        break;
                    case 4 ... 19:
                        // Manufacturer - string
                        response.addString("OpenDTU", reg_idx - 4);
                        break;
                    case 20 ... 35:
                        // Model - string
                        response.addString("OpenDTU Total", reg_idx - 20);
                        break;
                    case 36 ... 43:
                        // Options - string
                        response.addString(config.Dev_PinMapping, reg_idx - 36);
                        break;
                    case 44 ... 51:
                        // Version - string
                        response.addString(strVersion.c_str(), strVersion.length(), reg_idx - 44);
                        break;
                    case 52 ... 67:
                        // Serial Number - string
                        response.addUInt64AsHexString(config.Dtu.Serial, reg_idx - 52);
                        break;
                    case 68:
                        // Device Address - uint16
                        response.addUInt8(request.getServerID());
                        break;
                    default:
                        // Points set to NaN or padding
                        response.addUInt16(0x8000);
                        break;
                }
            } else if (reg < 40170) { // >= 40070
                // Model 12 - IPv4 Model
                uint8_t reg_idx = reg - 40070;

                switch (reg_idx) {
                    case 0:
                        // Model ID
                        response.addUInt16(12);
                        break;
                    case 1:
                        // SunSpec model register count (length without model header (2))
                        response.addUInt16(98);
                        break;
                    case 2 ... 5:
                        // Interface name
                        response.addString(NetworkSettings.NetworkMode() == network_mode::WiFi ? "wifi" : "eth", reg_idx - 2);
                        break;
                    case 6:
                        // Config Status: VALID_SETTING: 1 (enum16)
                        response.addUInt16(1);
                        break;
                    case 7:
                        // Change Status: (bitfield16)
                        response.addUInt16(0);
                        break;
                    case 8:
                        // Config Capability: CFG_SETTABLE: bit 4 (bitfield16)
                        response.addUInt16(16);
                        break;
                    case 9:
                        // IPv4 Config: DHCP: 1 - Static: 0 (enum16)
                        response.addUInt16(Configuration.get().WiFi.Dhcp ? 1 : 0);
                        break;
                    case 10:
                        // Control:
                        response.addUInt16(0);
                        break;
                    case 11 ... 18:
                        // IP - string
                        response.addIPAddressAsString(NetworkSettings.localIP(), reg_idx - 11);
                        break;
                    case 19 ... 26:
                        // Netmask - string
                        response.addIPAddressAsString(NetworkSettings.subnetMask(), reg_idx - 19);
                        break;
                    case 27 ... 34:
                        // Gateway - string
                        response.addIPAddressAsString(NetworkSettings.gatewayIP(), reg_idx - 27);
                        break;
                    case 35 ... 42:
                        // DNS1 - string
                        response.addIPAddressAsString(NetworkSettings.dnsIP(0), reg_idx - 35);
                        break;
                    case 43 ... 50:
                        // DNS2 - string
                        response.addIPAddressAsString(NetworkSettings.dnsIP(1), reg_idx - 43);
                        break;
                    case 51 ... 62:
                        // NTP1 - string
                        response.addUInt16(0);
                        break;
                    case 63 ... 74:
                        // NTP2 - string
                        response.addUInt16(0);
                        break;
                    case 75 ... 86:
                        // Domain name - string
                        response.addUInt16(0);
                        break;
                    case 87 ... 98:
                        // Host name - string
                        response.addString(NetworkSettings.getHostname(), reg_idx - 87);
                        break;
                    default:
                        // Points set to NaN or padding
                        response.addUInt16(0x8000);
                        break;
                }
            } else if (reg < 40232) { // >= 40170
                // Model 111 - Inverter (Single Phase) FLOAT Model
                // The Inverter Manager acts as a virtual inverter that combines the individual
                // measured values of the inverters, if useful.
                uint8_t reg_idx = reg - 40170;

                switch (reg_idx) {
                    case 0:
                        // Model ID
                        response.addUInt16(111);
                        break;
                    case 1:
                        // SunSpec model register count (length without model header (2))
                        response.addUInt16(60);
                        break;
                    case 22 ... 23:
                        // AC Power (W) - required for EVCC, usage pv
                        response.addFloat32(Datastore.getTotalAcPowerEnabled(), reg_idx - 22);
                        break;
                    case 32 ... 33:
                         // AC Energy (Wh) - required for EVCC, usage pv
                        response.addFloat32(Datastore.getTotalAcYieldTotalEnabled() * 1000, reg_idx - 32);
                        break;
                    case 38 ... 39:
                        // DC Power (W)
                        response.addFloat32(Datastore.getTotalDcPowerEnabled(), reg_idx - 38);
                        break;
                    case 48 ... 49:
                        // enum16
                        response.addUInt16(0);
                        break;
                    case 50 ... 61:
                        // bitfield32
                        response.addUInt16(0);
                        break;
                    default:
                        // float32 - Not a Number
                        response.addFloat32(NAN, reg_idx & 0x01);
                        break;
                }
            } else if (reg < 40234) { // >= 40232
                uint8_t reg_idx = reg - 40232;
                switch (reg_idx) {
                    case 0:
                        // Mark empty model
                        response.addUInt16(0xFFFF);
                        break;
                    case 1:
                        // empty model with a length of 0
                        response.addUInt16(0);
                        break;
                }
            } else {
                goto address_error;
            }
        }
    } else {
address_error:
        // No valid regs - send respective error response
        LOG_W("Illegal data address 0x%04x:%d\n", (int)addr, (int)words);
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    }


    HEXDUMP_D("Response FC03", response.data(), response.size());

    // Send response back
    return response;
}
