#!/usr/bin/env python3
import sys
import asyncio
from dbus_next.aio import MessageBus
from dbus_next.constants import BusType
from dbus_next.service import ServiceInterface, method, dbus_property, PropertyAccess

BLUEZ = 'org.bluez'
ADAPTER_PATH = '/org/bluez/hci0'
GATT_MGR_IFACE = 'org.bluez.GattManager1'
LE_ADV_MGR_IFACE = 'org.bluez.LEAdvertisingManager1'
GATT_SVC_IFACE = 'org.bluez.GattService1'
GATT_CHR_IFACE = 'org.bluez.GattCharacteristic1'
LE_ADV_IFACE = 'org.bluez.LEAdvertisement1'

class CredsService(ServiceInterface):
    PATH = '/org/bluez/example/service0'
    UUID = '12345678-1234-5678-1234-56789abcdef0'
    def __init__(self, bus):
        super().__init__(GATT_SVC_IFACE)
        self.bus = bus
        self.path = CredsService.PATH
        self.characteristics = []

    @dbus_property(access=PropertyAccess.READ)
    def UUID(self) -> 's': return CredsService.UUID
    @dbus_property(access=PropertyAccess.READ)
    def Primary(self) -> 'b': return True
    @dbus_property(access=PropertyAccess.READ)
    def Characteristics(self) -> 'ao':
        return [ch.path for ch in self.characteristics]

class WriteOnlyChr(ServiceInterface):
    def __init__(self, bus, idx, uuid):
        super().__init__(GATT_CHR_IFACE)
        self.bus = bus
        self.uuid = uuid
        self.path = f"{CredsService.PATH}/char{idx}"
        self.value = b''

    @dbus_property(access=PropertyAccess.READ)
    def UUID(self) -> 's': return self.uuid
    @dbus_property(access=PropertyAccess.READ)
    def Service(self) -> 'o': return CredsService.PATH
    @dbus_property(access=PropertyAccess.READ)
    def Flags(self) -> 'as': return ['write']

    @method()
    def WriteValue(self, val: 'ay', options: 'a{sv}'):
        self.value = bytes(val)

class CredsAdvertisement(ServiceInterface):
    PATH = '/org/bluez/example/advertisement0'
    def __init__(self, bus):
        super().__init__(LE_ADV_IFACE)
        self.bus = bus
        self.path = CredsAdvertisement.PATH
        self.type = 'peripheral'
        self.service_uuids = [CredsService.UUID]
        self.local_name = 'Pi-Setup'

    @dbus_property(access=PropertyAccess.READ)
    def Type(self) -> 's': return self.type
    @dbus_property(access=PropertyAccess.READ)
    def ServiceUUIDs(self) -> 'as': return self.service_uuids
    @dbus_property(access=PropertyAccess.READ)
    def LocalName(self) -> 's': return self.local_name
    @dbus_property(access=PropertyAccess.READ)
    def Includes(self) -> 'as': return ['tx-power']

    @method()
    def Release(self):
        print("Advertisement released")

async def main():
    bus = await MessageBus(bus_type=BusType.SYSTEM).connect()

    # Introspect adapter for GATT and LE adv managers
    introspection = await bus.introspect(BLUEZ, ADAPTER_PATH)
    gatt_obj = bus.get_proxy_object(BLUEZ, ADAPTER_PATH, introspection)

    svc = CredsService(bus)
    ch1 = WriteOnlyChr(bus, 1, CredsService.UUID[:-1] + '1')
    ch2 = WriteOnlyChr(bus, 2, CredsService.UUID[:-1] + '2')
    svc.characteristics = [ch1, ch2]
    bus.export(svc.path, svc)
    bus.export(ch1.path, ch1)
    bus.export(ch2.path, ch2)
    await gatt_obj.get_interface(GATT_MGR_IFACE).RegisterApplication('/', {})

    # Register LE advertisement
    adv_obj = bus.get_proxy_object(BLUEZ, ADAPTER_PATH, introspection)
    adv_mgr = adv_obj.get_interface(LE_ADV_MGR_IFACE)
    adv = CredsAdvertisement(bus)
    bus.export(adv.path, adv)
    await adv_mgr.RegisterAdvertisement(adv.path, {})

    print("🟢 Advertising as 'Pi-Setup', waiting for SSID & PSK…")
    while not (ch1.value and ch2.value):
        await asyncio.sleep(1)

    ssid = ch1.value.decode()
    psk  = ch2.value.decode()
    print(f"--> Got SSID: {ssid!r}, PSK: {psk!r}")

    await adv_mgr.UnregisterAdvertisement(adv.path)
    sys.exit(0)

if __name__ == '__main__':
    asyncio.run(main())
