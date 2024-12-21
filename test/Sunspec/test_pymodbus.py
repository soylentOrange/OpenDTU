#!/usr/bin/env python3
"""Pymodbus asynchronous client example.

An example of a single threaded synchronous client.

usage: simple_async_client.py

All options must be adapted in the code
The corresponding server must be started before e.g. as:
    python3 server_sync.py
"""
import asyncio

import pymodbus.client as ModbusClient
from pymodbus import (
    ExceptionResponse,
    FramerType,
    ModbusException,
    pymodbus_apply_logging_config,
)

from pymodbus.payload import BinaryPayloadDecoder

from pymodbus.constants import Endian

async def run_async_simple_client(comm, host, port, slave_id, framer=FramerType.SOCKET):
    """Run async client."""
    # activate debugging
    pymodbus_apply_logging_config("WARNING")

    print("get client")
    if comm == "tcp":
        client = ModbusClient.AsyncModbusTcpClient(
            host,
            port=port,
            framer=framer,
        )
    else:
        print(f"Unknown client {comm} selected")
        return

    print("connect to server")
    await client.connect()
    # test client is connected
    assert client.connected

    print("read first block of common model")
    try:
        data = await client.read_holding_registers(40000, 4, slave_id)
        decoder = BinaryPayloadDecoder.fromRegisters(
            data.registers,
            byteorder=Endian.BIG,
            wordorder=Endian.BIG,
        )
    except ModbusException as exc:
        print(f"Received ModbusException({exc}) from library")
        client.close()
        return

    if isinstance(data, ExceptionResponse):
        print(f"Received Modbus library exception ({data})")
        # THIS IS NOT A PYTHON EXCEPTION, but a valid modbus message
        client.close()
        return

    if data.isError():
        print(f"Received Modbus library error({data})")
        client.close()
        return


    print("\n# common model: start #########\n")
    print(f"SunS: {decoder.decode_string(4).decode()}")
    model_id = decoder.decode_16bit_uint()
    print(f"Model ID: {model_id}")
    model_len = decoder.decode_16bit_uint()
    print(f"Length: {model_len}")

    # read next block of model_len words
    try:
        data2 = await client.read_holding_registers(40004, model_len, slave_id)
        decoder2 = BinaryPayloadDecoder.fromRegisters(
            data2.registers,
            byteorder=Endian.BIG,
            wordorder=Endian.BIG,
        )
    except ModbusException as exc:
        print(f"Received ModbusException({exc}) from library")
        client.close()
        return

    if isinstance(data2, ExceptionResponse):
        print(f"Received Modbus library exception ({data2})")
        # THIS IS NOT A PYTHON EXCEPTION, but a valid modbus message
        client.close()
        return

    if data.isError():
        print(f"Received Modbus library error({data2})")
        client.close()
        return

    # 16 words for Manufacturer name
    print(f"Manufacturer: {decoder2.decode_string(32).decode()}")
    # 16 words for model name
    print(f"Model: {decoder2.decode_string(32).decode()}")
    # 8 words for options string
    print(f"Options: {decoder2.decode_string(16).decode()}")
    # 8 words for version string
    print(f"Version: {decoder2.decode_string(16).decode()}")
    # 16 words for serial-number string
    print(f"Serial Number: {decoder2.decode_string(32).decode()}")
    # device address (Modbus ID) - should match the requested one...
    print(f"Device Address: {decoder2.decode_16bit_uint()}")
    # optional pad, when model length == 66
    if model_len == 66:
        print(f"pad: 0x{decoder2.decode_16bit_uint():x}")
    else:
        print("pad: not required")

    print("\n# common model: end ###########\n")
    print("close connection")
    client.close()


if __name__ == "__main__":
    print(" ")
    print("# check slave 127...")
    asyncio.run(
        run_async_simple_client("tcp", "192.168.178.112", 502, 127), debug=True
    )
    print("# done checking slave 127!")
    print(" ")
    print("# check slave 125...")
    asyncio.run(
        run_async_simple_client("tcp", "192.168.178.112", 502, 125), debug=True
    )
    print("# done checking slave 125!")
    print(" ")
