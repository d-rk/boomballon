# Bill of Materials

This table is the authoritative BOM for the electronics described in the
[Build Guide overview](../build-guide/overview-bom.md). Part numbers and
suppliers below were reconciled against the original order sheet
([`hardware/bom/20170825_Bestellliste.xlsx`](https://github.com/boomballoon/boomballoon.github.io/blob/main/hardware/bom/20170825_Bestellliste.xlsx),
supplier mostly Reichelt). Datasheets for the numbered parts live under
[`hardware/datasheets/`](https://github.com/boomballoon/boomballoon.github.io/tree/main/hardware/datasheets).

| Component | Role | Qty | Part / supplier |
|---|---|---|---|
| Arduino Nano (ATmega328P) | Controller | 1 | Keywish Nano (original prototype used an Arduino Micro) |
| 12 V / 3 A PSU | Power | 1 | generic (Amazon) |
| DC socket (switched) | Power input | 1 | BKL 072342 |
| Toggle switch | On/off | 1 | MS-165 series |
| Membrane/vacuum pump | Inflate | 1 | AIRPO D2028B (12 V) |
| Solenoid valve | Deflate | 1 | CEME 5000EN1,5P |
| TIP120 Darlington | Pump/valve switching | 2 | STM (Reichelt) |
| 1N4004 diode | Flyback across each coil | 2 | — |
| 2.2 kΩ resistor | Transistor base | 2 | metal film (METALL 2,20K) |
| 74HC595 | Display shift register | 1 | SMD HC 595 |
| 7-seg 20 mm (common cathode) | Display digit | 1 | SC08-11 |
| Piezo buzzer | Audio | 1 | EKULIT AL-60P12 |
| Photoresistor | Card sensing | 5 | Reichelt A 905014 |
| 330 Ω resistor | Photocell divider (Fotomodul) | 5 | SMD 1206 |
| 3 mm LED | Card illumination | 5 | Reichelt LED EL 3-11250KW |
| 330 Ω resistor | LED limiting (Lichtmodul) | 5 | SMD 1206 |
| 330 Ω resistor | Segment limiting (Display) | 8 | SMD 1206 |
| Headers / stacking strips | Inter-module connectors | — | MPE-Garry / STAPELLEISTE (Reichelt) |
| Polycarbonate window | Display pane | 1 | Reely (Conrad 229802) |
| 3D-printed housing | Enclosure | 1 set | see [Enclosure](../build-guide/enclosure/index.md) |
