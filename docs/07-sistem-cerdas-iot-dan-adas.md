# 🧠 Sistem Cerdas, Keamanan Aktif (ADAS) & Arsitektur Baterai Ganda

Dokumen ini merinci rancang bangun sistem komputasi cerdas terintegrasi pada **Laksanasoft-EV**, mencakup kendali traksi aktif (*Traction Control System*), sensor inersia 6-Axis IMU, sistem akses tanpa kunci (*Smart Keyless NFC & BLE*), manajemen baterai ganda modular (*Dual-Battery Active Balancing*), serta pembaruan firmware nirkabel (*Over-The-Air / OTA*).

---

## 1. Arsitektur Komputasi Cerdas Terpadu

```
                                  +-----------------------------------------------------+
                                  |         LAKSANASOFT SMART VCU & TELEMATICS          |
                                  +-----------------------------------------------------+
                                                             |
            +--------------------+---------------------------+--------------------------+--------------------+
            |                    |                           |                          |                    |
            v                    v                           v                          v                    v
  [ 6-AXIS IMU SENSOR ]   [ SMART KEYLESS ]       [ DUAL-BATTERY BMS ]          [ TRACTION CONTROL ]    [ 7" DIGITAL HMI ]
  - Accelerometer         - NFC / RFID Reader     - Pack A (72V Primary)        - Wheel Slip Detection  - Turn-by-Turn GPS
  - Gyroscope (Lean Angle)- BLE Auto-Proximity    - Pack B (72V Secondary)      - Anti-Wheelie Torque   - Lean Angle Meter
  - Crash / Tilt Detect   - Smartphone Digital Key- Ideal Diode Active Sharing  - Cornering Safe Torque - Real-Time Telemetry
```

---

## 2. Sensor Inersia 6-DOF & Sistem Kendali Traksi (TCS)

VCU mengintegrasikan sensor inersia 6-Axis (*6-Degrees of Freedom*) berkecepatan tinggi ($200\text{ Hz}$) untuk membaca orientasi spasial kendaraan secara *real-time*:

### A. Deteksi Kemiringan (Lean Angle Detection):
- Membaca sudut rebah motor saat menikung ($\theta_{roll}$).
- **Cornering Torque Limiter:** Jika kemiringan motor melebihi $38^\circ$, VCU secara otomatis membatasi akselerasi mendadak (*torque ramp-rate*) untuk mencegah ban selip (*low-side crash*).
- **Tip-Over Crash Cutoff:** Jika motor terjatuh dengan sudut rebah $\ge 60^\circ$ selama $> 500\text{ ms}$, VCU seketika memutus daya kontaktor utama dan mengirimkan sinyal bahaya SOS darurat via 4G LTE beserta koordinat GPS.

### B. Algoritma Kendali Traksi (Traction Control System - TCS):
TCS mencegah ban belakang berputar bebas (*spin*) saat berakselerasi di permukaan jalan licin, basah, atau berpasir:

```
                      +-----------------------------+
                      | Input Gas Pengendara (APPS) |
                      +--------------+--------------+
                                     |
                                     v
                 +----------------------------------------+
                 | Kalkulasi Selisih Kecepatan (Slip Ratio)|
                 |   Delta_RPM = RPM_Motor - RPM_Target    |
                 +-------------------+--------------------+
                                     |
                [ Apakah Delta_RPM > Batas Slip Aman? ]
                               /           \
                             YA            TIDAK
                             /               \
                            v                 v
            +-------------------------+   +-------------------------+
            |  TCS INTERVENSI AKTIF   |   |   NORMAL TORQUE OUTPUT  |
            | Reduksi Arus Torsi 40%  |   | Salurkan Torsi Penuh Sesuai|
            |   dalam Waktu < 15 ms   |   |        Mode Berkendara  |
            +-------------------------+   +-------------------------+
```

---

## 3. Sistem Akses Tanpa Kunci Cerdas (Smart Keyless & Smart Entry)

Kendaraan tidak lagi bergantung pada anak kunci mekanikal konvensional:

1. **BLE Proximity Auto-Unlock (Passive Keyless Entry):**
   - VCU mendeteksi kedekatan smartphone pengendara via Bluetooth Low Energy (BLE) berdasarkan kekuatan sinyal (*RSSI Signal Threshold*).
   - Saat pengendara berada dalam radius $\le 1.5\text{ meter}$, sistem otomatis meng-arm VCU (*Unlock*). Saat pengendara menjauh $> 3\text{ meter}$, sistem otomatis terkunci (*Auto-Lock*).
2. **NFC Smartcard Reader:**
   - Cukup tempelkan kartu pintar NFC / e-KTP / smartwatch pada sensor di stang untuk menyalakan motor dalam waktu $< 100\text{ ms}$.
3. **Emergency PIN Code:**
   - Jika smartphone mati dan kartu NFC tertinggal, pengendara dapat memasukkan 6-digit PIN keamanan langsung pada layar sentuh HMI cluster.
4. **Anti-Theft Motion & Geofencing Alarm:**
   - Saat kondisi terkunci (*ARMED*), jika sensor IMU mendeteksi getaran atau roda digerakkan, VCU akan mengunci motor secara elektrik (*Motor Electronic Resistance Brake*), membunyikan sirine klakson berdenyut, dan mengirimkan notifikasi instan ke aplikasi pengguna.

---

## 4. Arsitektur Baterai Ganda Modular (Dual-Battery Active Balancing)

Platform mendukung penggunaan **2 Unit Baterai 72V Portabel** yang dapat dilepas-pasang (*swappable*) secara fleksibel:

```
   [ BATERAI PACK A (72V 30Ah) ]            [ BATERAI PACK B (72V 30Ah) ]
          (Smart BMS A)                            (Smart BMS B)
                │                                        │
                ▼                                        ▼
   [ Smart Ideal Diode Controller A ]       [ Smart Ideal Diode Controller B ]
                │                                        │
                +───────────────────┬────────────────────+
                                    │ (Total 72V 60Ah / 4.3 kWh)
                                    ▼
                         [ MCB DC / BUS UTAMA 72V ]
                                    │
                                    ▼
                         [ CONTROLLER INVERTER ]
```

### Keunggulan Sistem Baterai Ganda:
- **Hot-Swappable:** Baterai dapat dicopot satu per satu untuk dicas di dalam rumah/kantor tanpa mematikan sistem kelistrikan motor.
- **Active Load Sharing:** Modul *Ideal Diode MOSFET* mencegah arus saling mengisi antar baterai (*cross-charging*) jika terjadi perbedaan voltase antar pack, sehingga aman digunakan meski level baterai Pack A (80%) dan Pack B (40%) berbeda.
- **Single / Dual Pack Operation:** Motor dapat tetap melaju kencang meskipun hanya 1 baterai yang terpasang.

---

## 5. Pembaruan Firmware Nirkabel (Over-The-Air / OTA)

VCU dilengkapi sistem dual-partition flash memory (**OTA A/B Rollback Safety**):
1. VCU mengunduh pembaruan firmware baru dari server cloud di latar belakang tanpa mengganggu operasional kendaraan.
2. Integritas binary diverifikasi menggunakan hashing SHA-256 dan tanda tangan digital kriptografi.
3. Setelah motor dalam posisi berhenti (*STATE_STANDBY*) dan kunci dimatikan, VCU melakukan *boot* ke partisi baru.
4. Jika terjadi kegagalan sistem pada boot pertama, VCU otomatis membatalkan pembaruan (*Automatic Rollback*) ke firmware stabil sebelumnya.
