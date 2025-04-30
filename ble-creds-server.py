#!/usr/bin/env python3
import sys, trio
from dbus_next.aio import MessageBus
from dbus_next.service import ServiceInterface, method, dbus_property, PropertyAccess

BLUEZ = 'org.bluez'
GATT_MGR = 'org.bluez.GattManager1'

class CredsService(ServiceInterface):
    PATH = '/org/bluez/example/service0'
    UUID = '12345678-1234-5678-1234-56789abcdef0'
    def __init__(self, bus):
        super().__init__(BLUEZ + '.GattService1')
        self.path = CredsService.PATH
        self.characteristics = []
    @dbus_property(access=PropertyAccess.READ)
    def UUID(self) -> 's': return CredsService.UUID
    @dbus_property(access=PropertyAccess.READ)
    def Primary(self) -> 'b': return True
    @dbus_property(access=PropertyAccess.READ)
    def Characteristics(self) -> 'ao':
        return [ch.path for ch in self.characteristics]

class WriteOnlyCharacteristic(ServiceInterface):
    def __init__(self, bus, idx, uuid):
        super().__init__(BLUEZ + '.GattCharacteristic1')
        self.path = f"{CredsService.PATH}/char{idx}"
        self.uuid = uuid
        self.value = b''
    @dbus_property(access=PropertyAccess.READ)
    def UUID(self) -> 's': return self.uuid
    @dbus_property(access=PropertyAccess.READ)
    def Service(self) -> 'o': return CredsService.PATH
    @dbus_property(access=PropertyAccess.READ)
    def Flags(self) -> 'as': return ['write']
    @method()
    def WriteValue(self, val: 'ay', opts: 'a{sv}'):
        self.value = bytes(val)

async def main():
    bus = await MessageBus(bus_type=MessageBus.TYPE_SYSTEM).connect()
    gatt_obj = await bus.get_proxy_object(BLUEZ, '/org/bluez/hci0', [GATT_MGR])
    svc = CredsService(bus)
    ch1 = WriteOnlyCharacteristic(bus, 1, '12345678-1234-5678-1234-56789abcdef1')
    ch2 = WriteOnlyCharacteristic(bus, 2, '12345678-1234-5678-1234-56789abcdef2')
    svc.characteristics = [ch1, ch2]
    bus.export(svc.path, svc)
    bus.export(ch1.path, ch1)
    bus.export(ch2.path, ch2)
    await gatt_obj.get_interface(GATT_MGR).RegisterApplication('/', {})

    print("Waiting for SSID & PSK…")
    while not (ch1.value and ch2.value):
        await trio.sleep(1)

    ssid = ch1.value.decode()
    psk  = ch2.value.decode()
    print(f"--> Got SSID: {ssid!r}, PSK: {psk!r}")
    sys.exit(0)

if __name__ == '__main__':
    trio.run(main)
