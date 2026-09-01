# 🔌 Skematik & Wiring Diagram Lengkap (Laksanasoft-EV)

Dokumen ini adalah **panduan pengkabelan praktis (*practical wiring guide*)** yang dirancang dengan komponen yang **sangat mudah dijangkau di pasaran Indonesia**, lengkap dengan warna kabel, kode terminal, soket pin-to-pin, dan skema koneksi bebas percikan api (*anti-spark*).

---

## 1. Master Wiring Diagram (Skema Lengkap Satu Kendaraan)

```
=======================================================================================================
                                 MASTER WIRING DIAGRAM LAKSANASOFT-EV
=======================================================================================================

      [ BATERAI 72V PACK ]
      (+) Positif 72V ------------------------+
      (-) Negatif (B- BMS)                    |
               |                              |
         [ BMS P- ]                           v
               |                        [ MCB DC 2P 100A ]
               |                              |
               |       +----------------------+--------------------+
               |       | (Jalur Baterai Langsung)                  | (Jalur Kontak)
               |       v                                           v
               |  [ XT90-S Anti-Spark ]                 [ KUNCI KONTAK (Ignition) ]
               |       |                                           | (Output 72V Switched)
               |       |                                           +---------------+
               |       |                                           |               |
               |       v                                           v               v
               |  [ CONTROLLER / INVERTER ]               [ DC-DC STEPDOWN ]   [ CONTROLLER ]
               |  Terminal (B+) Positif 72V               (Input 72V DC)       Kabel Electric Lock
               |                                                   |           (Kabel Oranye / Merah Kecil)
               +-> Terminal (B-) Negatif 72V                       | (Output 12V 10A-20A)
                       |                                           v
                       |                                   [ FUSE BOX 12V ]
                       |                                           |
                       |       +-----------------------------------+--------------------+
                       |       |                        |                               |
                       |       v (10A)                  v (5A)                          v (5A)
                       |  [ RELAY KLAKSON ]       [ FLASHER SEIN ]               [ SWITCH REM ]
                       |  (Pin 30, 87, 86, 85)          | (12V Pulsing)           (Brake Lever SW)
                       |       |                        v                               |
                       |       v               [ SAKLAR SEIN KIRI/KANAN ]               +--------+
                       |  [ KLAKSON 12V ]               |                               |        |
                       |                                v                               v        v
                       |                       [ LAMPU SEIN LED ]              [ LAMPU REM ] [ CONTROLLER ]
                       |                                                          (12V LED)   (High Brake In)
                       |
                       +========================+
                                                |
                 +------------------------------+------------------------------+
                 |                              |                              |
                 v                              v                              v
       [ KABEL FASA MOTOR ]           [ SOKET HALL SENSOR ]          [ SOKET GAS / THROTTLE ]
       - Kuning : Fasa U              - Merah  : +5V Power           - Merah  : +5V Power
       - Hijau  : Fasa V              - Hitam  : GND                 - Hitam  : GND
       - Biru   : Fasa W              - Kuning : Sinyal Hall A       - Hijau  : Sinyal Gas (0.8V - 4.2V)
                                      - Hijau  : Sinyal Hall B       - Abu-abu/Putih: Saklar Speed 1-2-3
                                      - Biru   : Sinyal Hall C       - Coklat : Saklar Mundur (Reverse)
=======================================================================================================
```

---

## 2. Diagram Jalur Daya Utama Tegangan Tinggi (High Voltage 72V)

Jalur ini mengalirkan arus besar ($30\text{A} - 150\text{A}$) dari baterai ke Controller Motor. Gunakan kabel silikon **AWG 8 / AWG 6** atau kabel tembaga **NYAF 10–16 mm²**.

```
[ BATERAI 72V (+) ] ===> [ MCB DC 2P 100A ] ===> [ KONEKTOR ANTI-SPARK XT90-S ] ===> [ Terminal B+ Controller ]
                                                                                   
[ BATERAI 72V (-) ] ===> [ BMS Terminal B- ] ===> [ BMS Terminal P- ] ============> [ Terminal B- Controller ]

[ CONTROLLER ] 
  ├── Terminal Fasa U (Baut Kuning) ==========> Kabel Fasa U Motor (Kuning)
  ├── Terminal Fasa V (Baut Hijau) ===========> Kabel Fasa V Motor (Hijau)
  └── Terminal Fasa W (Baut Biru) ============> Kabel Fasa W Motor (Biru)
```

### 💡 Tips Anti-Spark Sederhana (Mencegah Percikan Api Saat Colok Baterai):
1. **Opsi 1 (Paling Mudah):** Gunakan konektor **XT90-S (Tipe Anti-Spark Hijau/Kuning)**. Konektor ini memiliki resistor bawaan pada ujung pinnya yang meredam lonjakan arus pengisian kapasitor saat dicolokkan.
2. **Opsi 2 (Menggunakan MCB DC):** Pasang MCB DC pada posisi **OFF** sebelum mencolokkan kabel baterai. Setelah semua soket terpasang kencang, baru naikkan tuas MCB DC ke posisi **ON**.

---

## 3. Diagram Jalur Kelistrikan Tegangan Rendah (Low Voltage 12V)

DC-DC Converter menurunkan tegangan baterai ($72\text{V}$) menjadi tegangan aki standar ($12\text{V}$) untuk menyalakan lampu, klakson, speedometer, dan saklar kontrol.

```
                  +--------------------------------------------------------+
                  |            DC-DC STEPDOWN CONVERTER (72V -> 12V)       |
                  |                                                        |
[ Output Kontak ] |  [ MERAH ]   : Input Positif Tegangan Tinggi (72V DC)  |
[ Negatif Bersama]|  [ HITAM ]   : Input Negatif / Ground Bersama (GND)    |
[ Jalur 12V Pos ] |  [ KUNING ]  : Output Positif 12V DC Stabil (10A-20A)  |
                  +----------------------------+---------------------------+
                                               |
                                        [ FUSE BOX 12V ]
                                               |
         +-------------------------------------+-------------------------------------+
         | (Fuse 10A)                          | (Fuse 5A)                           | (Fuse 5A)
         v                                     v                                     v
   [ PIN 30 RELAY KLAKSON ]            [ FLASHER SEIN 12V ]                  [ SAKLAR LAMPU UTAMA ]
         |                                     | (Kaki L)                            |
         | [ PIN 87 ]                          v                                     v
         +--------> [ KLAKSON (+) ]    [ SAKLAR SEIN STANG ]                [ LAMPU DEPAN LED (+) ]
         |                                     |                                     |
   [ PIN 86 ]                                  +---> [ SEIN KIRI (+) ]               v
         |                                     +---> [ SEIN KANAN (+) ]            [ GND ]
         v                                     |
   [ TOMBOL KLAKSON STANG ]                    v
         |                                   [ GND ]
         v
       [ GND ]
```

---

## 4. Pinout & Panduan Kabel Controller Populer (Votol EM-50s / EM-100 / EM-150)

Controller Votol menggunakan soket multi-pin dengan kode warna standar pabrik sebagai berikut:

```
                  +------------------------------------------------------+
                  |        SOKET KENDALI UTAMA CONTROLLER VOTOL          |
                  +------------------------------------------------------+
```

| No | Warna Kabel Standar | Nama Sinyal | Terhubung Ke | Keterangan & Cara Kerja |
| :-: | :--- | :--- | :--- | :--- |
| **1** | **Oranye / Merah Kecil** | **Electric Door Lock** | Output Kunci Kontak (+72V) | Memberikan trigger tegangan baterai untuk menyalakan CPU controller. |
| **2** | **Merah (Soket Gas)** | **Throttle +5V** | Kabel Merah Handle Gas | Suplai tegangan 5 Volt untuk sensor Hall gas. |
| **3** | **Hitam (Soket Gas)** | **Throttle GND** | Kabel Hitam Handle Gas | Ground sensor gas. |
| **4** | **Hijau (Soket Gas)** | **Throttle Signal** | Kabel Hijau Handle Gas | Input tegangan analog ($0.8\text{V} - 4.2\text{V}$) pengatur kecepatan. |
| **5** | **Ungu (Satu Kabel)** | **High Brake (Rem 12V)**| Switch Tuas Rem (+12V saat ditekan) | Memutus daya motor & mengaktifkan lampu rem saat tuas ditarik. |
| **6** | **Kuning/Putih** | **Low Brake (Rem GND)** | Switch Rem (GND saat ditekan) | Opsi rem alternatif: memutus motor jika dihubungkan ke Ground. |
| **7** | **Biru Muda / Pink** | **Speed 3 (High Speed)**| Saklar Speed di Stang (ke GND) | Kecepatan maksimal / Sport Mode jika dihubungkan ke GND. |
| **8** | **Biru Tua / Abu-abu** | **Speed 1 (Low Speed)** | Saklar Speed di Stang (ke GND) | Mode hemat energi (Eco) jika dihubungkan ke GND. |
| **9** | **Coklat** | **Reverse (Mundur)** | Tombol Saklar Mundur (ke GND) | Motor berputar mundur (maks 5 km/jam) saat tombol ditekan. |
| **10**| **Biru / Kuning-Hijau**| **One-Line Display** | Speedometer Digital | Sinyal data serial digital ke layar speedometer LCD. |
| **11**| **Soket 5-Pin Hall** | **Hall Sensor Motor** | Soket 5-Pin dari Hub Motor | Merah (+5V), Hitam (GND), Kuning (A), Hijau (B), Biru (C). |

---

## 5. Skema Rangkaian Relay Otomotif Standar (Klakson & Lampu Daya Tinggi)

Relay 12V 4-Pin / 5-Pin otomotif (Merk Bosch, Denso, atau Hella) menghubungkan beban tanpa merusak saklar stang:

```
              [ +12V DC (Dari Fuse Box) ]
                           |
                           +-------------------+
                           |                   |
                           v                   v
                    [ PIN 30 (Daya In) ]  [ PIN 86 (Coil In) ]
                           |                   |
                     (Kontak Saklar)           |
                           |                   v
                           v          [ SAKLAR / TOMBOL STANG ]
                    [ PIN 87 (Daya Out) ]      |
                           |                   v
                           v                [ GND ]
                   [ BEBAN (KLAKSON /
                    LAMPU TEMBAK LED) ]
                           |
                           v
                        [ GND ]
```

- **Pin 30:** Input sumber daya positif (+12V) dari aki / DC-DC Converter melalui sekring.
- **Pin 87:** Output daya (+12V) menuju beban (Klakson / Lampu Sorot).
- **Pin 86:** Input positif kumparan (*coil*) relay (bisa digabung ke Pin 30 atau output kunci kontak).
- **Pin 85:** Negatif kumparan (*coil*) relay yang menuju tombol saklar stang (terhubung ke GND saat ditekan).

---

## 6. Panduan Perakitan Kabel Langkah-demi-Langkah (Anti Korsleting)

Ikuti urutan pengerjaan berikut untuk menjamin keamanan dan mencegah kerusakan komponen elektronik:

```
[LANGKAH 1] Pasang Rangka & Komponen Fisik (Motor Hub, Controller, MCB DC, DC-DC Converter)
     │
[LANGKAH 2] Pasang Kabel Tegangan Rendah 12V (Kunci Kontak, DC-DC, Lampu, Sein, Klakson, Rem)
     │      --> Uji fungsi kelistrikan 12V menggunakan aki 12V eksternal atau adaptor 12V.
     │
[LANGKAH 3] Hubungkan Soket Kontrol (Handle Gas, Hall Sensor 5-Pin, Switch Speed, One-Line Speedo)
     │      --> Pastikan warna pin kabel Hall motor dan controller saling cocok.
     │
[LANGKAH 4] Hubungkan 3 Kabel Fasa Motor (Kuning, Hijau, Biru) ke Terminal Baut Controller
     │      --> Kencangkan mur baut fasa menggunakan kunci pas agar tidak timbul panas.
     │
[LANGKAH 5] Pasang Baterai 72V dengan MCB DC dalam Posisi OFF (Tuas Turun)
     │      --> Pasang kabel B- (Negatif) terlebih dahulu, kemudian kabel B+ (Positif).
     │
[LANGKAH 6] Naikkan Tuas MCB DC ke Posisi ON & Putar Kunci Kontak
     │      --> Periksa layar speedometer menyala, putar gas perlahan untuk uji putaran motor.
```

---

## 7. Troubleshooting Cepat Masalah Kelistrikan Umum

| Gejala Masalah | Kemungkinan Penyebab | Solusi Cepat |
| :--- | :--- | :--- |
| **Kunci kontak ON tetapi layar speedometer mati total** | Sekring 12V putus / Kabel oranye electric lock belum tersambung / DC-DC rusak | Periksa sekring 12V dengan multitester, cek tegangan output DC-DC stepdown (harus 12V). |
| **Layar nyala, tetapi digas motor tidak berputar** | Tuas rem tertarik (sinyal High Brake aktif) / Standar samping turun / Kabel gas hijau lepas | Cek apakah lampu rem menyala terus (switch rem macet). Ukur kabel hijau gas (harus 0.8V–4.2V saat diputar). |
| **Motor berputar tersendat-sendat atau berdengung keras** | Urutan kabel fasa (U, V, W) atau urutan kabel Hall Sensor tidak cocok | Tukar 2 posisi kabel fasa (misal: tukar Kuning dan Biru) atau lakukan *Auto-Learn / Hall Identification* via software Votol. |
| **Motor berputar terbalik (arah mundur saat digas)** | Konfigurasi arah fasa motor terbalik | Hubungkan kabel Reverse sebentar atau ubah setting *Motor Direction (CW/CCW)* di software controller. |
