# -*- coding: utf-8 -*-

"""
test opendtu-modbus with pysunspec2

Setup of the conda environment is handled by conda wingman extension:
(You need to open the pysunspec2.yaml)
Show and Run Commands > Conda Wingman: Build Conda Environment from YAML file
Show and Run Commands > Conda Wingman: Activate Conda Environment from YAML file

Set the Python Interpreter for VS Code to the environment's one:
Show and Run Commands > Python: Select Interpreter

You could also use the suns.py script directly:
python3 lib/suns.py -a 127 -i 192.168.178.36 -p 501

Ctrl+F5 to run in VSCode.
"""

import sys
import os
import logging
import numbers

logger = logging.getLogger(__name__)

try:
    from sunspec2.modbus import (client, modbus)
except ImportError as e:
    raise ImportError("pysunspec2 is not available!") from e

logger = logging.getLogger(__name__)

# Run
if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    os.system('cls' if os.name=='nt' else 'clear')
    print('SunSpec compatibility test\nusing pySunSpec2\n\n')

    # Settings for the ModbusTCP-Device
    SLAVE_ID = 127
    IPADDR='192.168.178.112'
    IPPORT=502
    TIMEOUT=10

    # have low-level debug output?
    #TRACE_FUNC=logging.debug
    TRACE_FUNC=None

    # Error counter
    ERR_CNT = 0
    MB_EXCPT_CNT = 0

    logging.info('Test SunSpec-compatibility of %d @ %s:%d', SLAVE_ID, IPADDR, IPPORT)
    logging.info('open and scan device...')
    try:
        d = client.SunSpecModbusClientDeviceTCP(slave_id=SLAVE_ID,
                                                    ipaddr=IPADDR,
                                                    ipport=IPPORT,
                                                    trace_func=TRACE_FUNC,
                                                    timeout=TIMEOUT)
        d.scan()
        print('\nGot models:')
        for model in d.models:
            if not isinstance(model, numbers.Number):
                print(f'  {model}')
        print(" ")
    except modbus.ModbusClientTimeout as e:
        logging.error(e)
        ERR_CNT += 1
    except client.SunSpecModbusClientError as e:
        logging.error(e)
        ERR_CNT += 1
    except modbus.ModbusClientException as e:
        logging.error(e)
        MB_EXCPT_CNT += 1

    # print common-model
    try:
        model_common = d.common[0]
        model_common.read()
        print(model_common)
    except Exception as e:
        logging.error(e)

    # print ac_meter-model
    try:
        model_ac_meter = d.ac_meter[0]
        model_ac_meter.read()
        print(model_ac_meter)
    except Exception as e:
        logging.error(e)

    try:
        logging.info('close device...')
        d.close()
    except Exception as e:
        logging.error(e)
    finally:
        logging.info('Done!')
        print('\nDone!\n')
        sys.exit(0)

    logging.info('close device...')
    try:
        d.close()
    except Exception as e:
        logging.error(e)
    finally:
        logging.info('Done!')
        sys.exit(0)
