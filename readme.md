#  iotsaDoorbellRinger

![build-platformio](https://github.com/cwi-dis/iotsaDoorbellRinger/workflows/build-platformio/badge.svg)
![build-arduino](https://github.com/cwi-dis/iotsaDoorbellRinger/workflows/build-arduino/badge.svg)

A buzzer server: you can tell it to make a buzzer sound for some time, and a LED light up. Companion to _iotsaDoorbellButton_.

Primarily used to experiment with tokens and capabilities for access control.

## Provisioning

Use the shared `iotsa` CLI (from the iotsa repo, `extras/python/`; see `iotsa-group/CLAUDE.md`). After a factory-fresh flash, three things need setting:

- **WiFi** — `iotsa networks` finds the `config-iotsa<suffix>` AP; then `iotsa --ssid config-iotsa<suffix> wifiConfig ssid=… ssidPassword=…`, then reboot.
- **Hostname** — `iotsa -t iotsa<suffix>.local --credentials owner:… configWait config hostName=…` (put the device in configuration mode when `configWait` asks; don't reboot it yourself to force the mode).
- **Capability issuer** — `iotsa -t <name>.local --credentials owner:… configWait xConfig capabilities trustedIssuer=<url> issuerKey=<shared-secret>`. This is what makes the token/capability access control work: `<url>` is the issuer that mints tokens for this device.

