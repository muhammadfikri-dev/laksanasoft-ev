# 🛒 Bill of Materials (BOM) & Daftar Alat Kerja

Dokumen ini merinci seluruh komponen, perangkat keras, perkakas (*tools*), dan estimasi anggaran biaya untuk membangun sistem **Laksanasoft-EV**.

---

## 1. Daftar Komponen Elektrikal & Powertrain (BOM)

| Kategori | Nama Komponen / Part | Spesifikasi Teknis | Qty | Satuan | Estimasi Harga (IDR) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Powertrain** | QS Motor 3000W 138 70H Mid Drive | 72V 3000W Rated, 6000W Peak, 4800 RPM, Encoder/Hall | 1 | Unit | Rp 6.800.000 |
| **Inverter** | Controller FOC Votol EM150-2 / FarDriver | 72V-90V, 150A Bus Current, 350A Phase Current, CAN | 1 | Unit | Rp 2.750.000 |
| **Baterai** | Cell Lithium NMC / LiFePO4 Prismatic | 3.7V 45Ah Grade A (20 Sel Konfigurasi 20S1P) | 20 | Sel | Rp 7.500.000 |
| **BMS** | Smart BMS 20S 150A Peak Daly/ANT | UART/CAN Communication, Bluetooth App, 4x Temp Probe | 1 | Unit | Rp 1.450.000 |
| **Sistem HV** | Main DC Contactor (Trombetta / Kilovac Clone) | Coil 12V DC, Kontak 100V 200A Continuous | 1 | Unit | Rp 450.000 |
| **Sistem HV** | Pre-Charge Relay & Ceramic Resistor | Relay 12V 10A, Resistor 100 Ohm 50W Aluminium Housed | 1 | Set | Rp 85.000 |
| **Sistem HV** | Fuse Tegangan Tinggi & Fuse Holder | ANL Fuse 150A 125V DC + Holder Isolator Bakelit | 1 | Set | Rp 120.000 |
| **Sistem HV** | Manual Service Disconnect (MSD) Switch | Amphenol Style HV Plug Switch 150A | 1 | Unit | Rp 350.000 |
| **Konversi Daya** | DC-DC Step-Down Converter Terisolasi | Input 48V-90V DC -> Output 12V DC 20A (240W) | 1 | Unit | Rp 250.000 |
| **VCU Core** | Board ESP32-S3-WROOM-1 DevKit | Dual Core 240MHz, 16MB Flash, 8MB PSRAM | 1 | Unit | Rp 130.000 |
| **VCU PCB** | Custom VCU Mainboard PCB (4-Layer) | Automotive-grade isolated inputs, CAN Transceiver, Buck 5V/3.3V | 1 | Board | Rp 300.000 |
| **Transceiver** | CAN Bus Module SN65HVD230 | 3.3V CAN Transceiver dengan 120 Ohm Termination | 1 | Modul | Rp 35.000 |
| **Telemetri** | SIMCOM A7670E 4G LTE Cat-1 Module | Modul Seluler 4G + GNSS/GPS Antenna | 1 | Modul | Rp 280.000 |
| **Input Driver** | Dual-Hall Electronic Throttle Handlebar | Output APPS1 (0.8V-4.2V) & APPS2 (4.2V-0.8V Inverted) | 1 | Set | Rp 220.000 |
| **Sensor Rem** | Hydraulic E-Brake Pressure Switch / Hall | NC/NO switch cutoff 12V | 2 | Unit | Rp 150.000 |
| **HMI Display** | 4.3" Round/Rectangular IPS TFT Display | SPI / RGB Interface dengan ESP32 Touchscreen Support | 1 | Unit | Rp 380.000 |
| **Kabel HV** | Kabel Silikon Fleksibel Oranye Tahan Panas | 25 mm² (4 AWG) Tahan Panas 200°C 600V (Baterai-Inverter) | 4 | Meter | Rp 320.000 |
| **Kabel Fasa** | Kabel Silikon Fasa Motor Kuning/Hijau/Biru | 16 mm² (6 AWG) Tahan Panas 200°C (Inverter-Motor) | 3 | Meter | Rp 240.000 |
| **Kabel LV** | Wiring Harness Otomotif AVSS | 0.5 mm² & 0.85 mm² Multicore Shielded Wire | 1 | Set | Rp 180.000 |
| **Konektor** | Anderson SB120 / Amphenol HVSL630 | Konektor Quick Disconnect Baterai Rating 120A | 2 | Pasang | Rp 220.000 |
| **Enclosure** | Box Baterai Aluminium 3mm Kustom | Finishing Powder Coating + Gasket Karet Silikon IP67 | 1 | Unit | Rp 850.000 |
| **Total Estimasi** | | | | | **± Rp 25.830.000** |

---

## 2. Daftar Alat Kerja & Fasilitas Perakitan (*Tools Required*)

### A. Alat Kelistrikan & Elektronika
1. **Hydraulic Cable Lug Crimper (10 - 120 mm²):**
   Untuk melakukan penekanan (*crimping*) skun kabel tembaga tebal pada kabel baterai dan fasa motor agar resistansi kontak serendah mungkin (< 0.1 mΩ).
2. **Digital Multimeter True RMS (Rating CAT III 1000V):**
   Untuk pengukuran tegangan bus DC, impedansi jalur, kontinuitas ground, dan sinyal analog throttle.
3. **Dual-Channel Digital Storage Oscilloscope (DSO):**
   Untuk analisis sinyal PWM controller, pembacaan sinyal sensor Hall, dan decoding paket CAN Bus.
4. **Soldering Station 80W - 100W dengan Suhu Teratur:**
   Untuk penyolderan komponen PCB, sensor harness, dan penyambungan kabel sinyal dengan timah *lead-free*.
5. **Heat Gun & Heat Shrink Dual-Wall (Adhesive Lined):**
   Untuk isolasi kabel kedap air (*waterproof sealing*).
6. **Spot Welder Baterai / TIG Welder:**
   (Jika menggunakan sel silindris 21700/18650) atau Torsi Kunci Hexagon terkalibrasi untuk terminal sel prisma.

### B. Alat Keselamatan Kerja (Personal Protective Equipment - PPE)
1. **Sarung Tangan Isolasi Listrik Kelas 0 (Rating 1000V AC / 1500V DC):**
   Wajib digunakan saat merangkai sel baterai dan menghubungkan busbar utama.
2. **Kacamata Pelindung Polikarbonat (Safety Goggles / Face Shield).**
3. **Alat Pemadam Api Ringan (APAR) Tipe CO2 atau Lithium Fire Extinguisher (F-500).**
4. **Matras Isolasi Karet (Anti-Static & High Voltage Insulation Mat).**

---

## 3. Komponen Tambahan Mekanikal & Sasis
- Dudukan motor *mid-drive* plat baja/aluminium CNC T6-6061 tebal 8mm.
- Rantai baja tipe 428H / Sabuk timing belt Gates Carbon Drive.
- Sprocket depan 14T baja karbon dan sprocket belakang 58T.
- Baut *grade 8.8* dan *nyloc anti-vibration nuts* untuk seluruh mounting dudukan baterai dan motor.
