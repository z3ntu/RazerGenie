# SPDX-License-Identifier: GPL-3.0-or-later
"""Private-session test service; never run against the user's daemon."""
import sys
import xml.etree.ElementTree as ET
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()
if bus.name_has_owner('org.razer'):
    sys.exit('Refusing to replace an existing daemon')
name = dbus.service.BusName('org.razer', bus)
class Mouse(dbus.service.Object):
    mode, smart, acceleration, writes = 1, False, True, 0
    fail_read, fail_write = False, False
    reads = 0
    def read(self, value):
        self.reads += 1
        if self.fail_read:
            raise dbus.exceptions.DBusException('Mouse disconnected')
        return value
    def write(self):
        self.writes += 1
        if self.fail_write:
            raise dbus.exceptions.DBusException('Device rejected setting')
    @dbus.service.method('org.freedesktop.DBus.Introspectable', out_signature='s')
    def Introspect(self):
        xml = dbus.service.Object.Introspect(self, "/org/razer/device/test", bus)
        root = ET.fromstring(xml)
        for interface in root.findall('interface'):
            if interface.get('name') == 'razer.device.power' and 'charging' not in sys.argv:
                for method in list(interface):
                    if method.get('name') == 'isCharging': interface.remove(method)
        if len(sys.argv) > 1 and sys.argv[1] in ('mode', 'none'):
            for interface in root.findall('interface'):
                if interface.get('name') == 'razer.device.scroll':
                    for method in list(interface):
                        if sys.argv[1] == 'none' or method.get('name') not in ('getScrollMode', 'setScrollMode'):
                            interface.remove(method)
        return ET.tostring(root, encoding='unicode')
    @dbus.service.method('razer.device.scroll', out_signature='y')
    def getScrollMode(self): return self.read(self.mode)
    @dbus.service.method('razer.device.scroll', in_signature='y')
    def setScrollMode(self, value): self.write(); self.mode = int(value)
    @dbus.service.method('razer.device.scroll', out_signature='b')
    def getScrollSmartReel(self): return self.read(self.smart)
    @dbus.service.method('razer.device.scroll', in_signature='b')
    def setScrollSmartReel(self, value): self.write(); self.smart = bool(value)
    @dbus.service.method('razer.device.scroll', out_signature='b')
    def getScrollAcceleration(self): return self.read(self.acceleration)
    @dbus.service.method('razer.device.scroll', in_signature='b')
    def setScrollAcceleration(self, value): self.write(); self.acceleration = bool(value)
    @dbus.service.method('razer.device.power', out_signature='d')
    def getBattery(self): return self.read(75.0)
    @dbus.service.method('razer.device.power', out_signature='b')
    def isCharging(self): return self.read(True)
    @dbus.service.method('org.razer.Test', in_signature='ybb')
    def HardwareSettings(self, mode, smart, acceleration):
        self.mode, self.smart, self.acceleration = int(mode), bool(smart), bool(acceleration)
    @dbus.service.method('org.razer.Test', in_signature='y')
    def HardwareMode(self, value): self.mode = int(value)
    @dbus.service.method('org.razer.Test', out_signature='i')
    def Reads(self): return self.reads
    @dbus.service.method('org.razer.Test', out_signature='i')
    def Writes(self): return self.writes
    @dbus.service.method('org.razer.Test', in_signature='bb')
    def Fail(self, read, write): self.fail_read, self.fail_write = read, write
mouse = Mouse(bus, '/org/razer/device/test')
print('READY', flush=True)
GLib.MainLoop().run()
