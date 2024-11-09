# OpenDTU

## Additional Modbus Interface

OpenDTU is extended by a Modbus server as proposed in [PR#1893](https://github.com/tbnobody/OpenDTU/pull/1893) - see the original [fork of b0661](https://github.com/b0661/OpenDTU/tree/pr_modbus).

The Modbus server serves TCP at port 502.

* At Modbus ID 1 the server mimicks the Hoymiles Modbus TCP Interface registers in the original DTUPro.
* At Modbus ID 125 the server serves a SunSpec compatible "total inverter" that
provides the OpenDTU aggregated data from all registered inverters.
* At Modbus ID 243 the server serves a SunSpec power meter that provides AC power and AC yield as if measuring all registered inverters.

The webapp is extended by Modbus configuration and info views.

The Modbus library used for Modbus communication is [eModbus](https://github.com/eModbus/eModbus).
Documentation for the library is [here](https://emodbus.github.io/).

### Homeassistant

Having the HACS addon [Hoymiles Plant DTU-Pro Sensor](https://github.com/ArekKubacki/Hoymiles-Plant-DTU-Pro) installed in Home Assistant, add to Home Assistant's configuration.yaml:

```yaml
sensor:
# change ip-address and panels to match your setup
  - platform: hoymiles_dtu
    host: 192.168.178.xxx
    name: Hoymiles PV
    dtu_type: 0
    monitored_conditions:
      - "pv_power"
      - "today_production"
      - "total_production"
      - "alarm_flag"
    monitored_conditions_pv:
      - "pv_power"
      - "today_production"
      - "total_production"
      - "pv_voltage"
      - "pv_current"
      - "grid_voltage"
      - "temperature"
      - "operating_status"
      - "alarm_code"
      - "alarm_count"
      - "link_status"
    panels: 3
```

### Minimal SunSpec Meter

A minimal SunSpec compliant meter is available at Modbus ID 243. Only provides AC power and AC yield as if measuring the output of all registered inverters.
To be used e.g. as a "Fronius Smart Meter TCP" as orginally by [AloisKlingler](https://github.com/AloisKlingler/OpenDTU-FroniusSM-MB).

### Full SunSpec Meter

A SunSpec compliant meter is avaialble at Modbus ID 125, which provides the OpenDTU aggregated data from all registered inverters.
To be used e.g. in [evcc](https://evcc.io/).

### Modbus Interface Details

The Modbus TCP server is available by default at port 502 and supports the modbus FC03: READ_HOLD_REGISTER.

#### Hoymiles Modbus TCP Interface registers

The Modbus id of the Hoymiles Modbus TCP Interface is 1 by default.

* Registers 0 - 3: Serial number
* Register 4: PV Voltage (V) - decimal fixed point, precision 1
* Register 5: PV Current (A) - decimal fixed point, precision 2
* Register 6: Grid Voltage (V) - decimal fixed point, precision 1
* Register 7: Grid Frequency (Hz) - decimal fixed point, precision 2
* Register 8: PV Power (W) - decimal fixed point, precision 1
* Register 9: Today Production (Wh) - uint16
* Registers 10 - 11: Total Production (Wh) - uint32
* Register 12: Temperature (°C) - decimal fixed point, precision 1
* Register 13: Operating Status - TODO (returns fixed statuscode 0x03)
* Register 14: Alarm code
* Register 15: Alarm count
* Register 16: Link status - TODO (return fixed statuscode 0x07)

See also: [src/ModbusDtuPro.cpp](https://github.com/soylentOrange/OpenDTU/blob/add-modbus/src/ModbusDtuPro.cpp) for the registers.

#### Minimal SunSpec Meter Interface registers

The Modbus id of the minimal SunSpec meter is 253 by default.
See: [src/ModbusDtuMeter.cpp](https://github.com/soylentOrange/OpenDTU/blob/add-modbus/src/ModbusDtuMeter.cpp) for the registers.

#### Full SunSpec Meter Interface registers

The Modbus id of the total SunSpec meter is 125 by default.
See: [src/ModbusDtuTotal.cpp](https://github.com/soylentOrange/OpenDTU/blob/add-modbus/src/ModbusDtuTotal.cpp) for the registers.

## Build the firmware image

Clone this repository, switch to branch "add-modbus" and compile following the guide of [OpenDTU](https://www.opendtu.solar/firmware/compile_vscode/).

You (probably) do not have to compile the webapp (see [OpenDTU guide](https://www.opendtu.solar/firmware/compile_webapp/)) in advance as an updated webapp is alread part of this repository. Use the included OpenDTU_yarn.yaml with the VSCode Extension Conda Wingman to create an envorinment with the required dependencies for builing the webapp.

## !! IMPORTANT UPGRADE NOTES !!

If you are upgrading from a version before 15.03.2023 you have to upgrade the partition table of the ESP32. Please follow the [this](docs/UpgradePartition.md) documentation!

## Background

This project was started from [this](https://www.mikrocontroller.net/topic/525778) discussion (Mikrocontroller.net).
It was the goal to replace the original Hoymiles DTU (Telemetry Gateway) with their cloud access. With a lot of reverse engineering the Hoymiles protocol was decrypted and analyzed.

## Documentation

The documentation can be found [here](https://tbnobody.github.io/OpenDTU-docs/).
Please feel free to support and create a PR in [this](https://github.com/tbnobody/OpenDTU-docs) repository to make the documentation even better.

## Breaking changes

Generated using: `git log --date=short --pretty=format:"* %h%x09%ad%x09%s" | grep BREAKING`

```code
* 1b637f08      2024-01-30      BREAKING CHANGE: Web API Endpoint /api/livedata/status and /api/prometheus/metrics
* e1564780      2024-01-30      BREAKING CHANGE: Web API Endpoint /api/livedata/status and /api/prometheus/metrics
* f0b5542c      2024-01-30      BREAKING CHANGE: Web API Endpoint /api/livedata/status and /api/prometheus/metrics
* c27ecc36      2024-01-29      BREAKING CHANGE: Web API Endpoint /api/livedata/status
* 71d1b3b       2023-11-07      BREAKING CHANGE: Home Assistant Auto Discovery to new naming scheme
* 04f62e0       2023-04-20      BREAKING CHANGE: Web API Endpoint /api/eventlog/status no nested serial object
* 59f43a8       2023-04-17      BREAKING CHANGE: Web API Endpoint /api/devinfo/status requires GET parameter inv=
* 318136d       2023-03-15      BREAKING CHANGE: Updated partition table: Make sure you have a configuration backup and completly reflash the device!
* 3b7aef6       2023-02-13      BREAKING CHANGE: Web API!
* d4c838a       2023-02-06      BREAKING CHANGE: Prometheus API!
* daf847e       2022-11-14      BREAKING CHANGE: Removed deprecated config parsing method
* 69b675b       2022-11-01      BREAKING CHANGE: Structure WebAPI /api/livedata/status changed
* 27ed4e3       2022-10-31      BREAKING: Change power factor from percent value to value between 0 and 1
```

## Currently supported Inverters

A list of all currently supported inverters can be found [here](https://www.opendtu.solar/hardware/inverter_overview/)

## Acknowledgements

This fork is based on the great work of
