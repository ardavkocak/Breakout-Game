# Breakout Game (Arduino)

**Arduino Uno** üzerinde, **128×64 OLED (SSD1306)** ekrana çizilen klasik Breakout (tuğla kırma) oyunu. Servo, LED'ler ve 7-segment display ile fiziksel donanım üzerinden interaktif bir oyun deneyimi sunar.

## Donanım

| Bileşen | Detay |
|---------|-------|
| Arduino Uno | Ana denetleyici |
| OLED Ekran | 128×64, SSD1306, I²C |
| Butonlar | Sol (0), Sağ (9), Seç (10) |
| 7-Segment Display | Pinler 2–8 |
| LED'ler (×3) | Pin 11, 12, 13 (can göstergesi) |
| Servo | (oyun olayları için, ör. ödül/animasyon) |

## Oyun Mekaniği

- **3 durum**: `MENU`, `GAMEPLAY`, `GAMEOVER`
- **3 can**: kayıp her topta LED ile gösterilir
- **Skor sayacı**: 7-segment ekranda
- **Top hızı**: her bölüm sonunda `%20` artar (`BALL_SPEED_INCREMENT`)
- **Paddle**: butonlarla sola/sağa hareket
- **Tuğlalar**: 12×4 piksel boyutunda dizilim
- **"Can" düşmesi**: tuğla kırıldığında düşen bonus mekanikleri

## Kütüphaneler

```cpp
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
```

Arduino IDE üzerinden **Adafruit GFX** ve **Adafruit SSD1306** kütüphanelerini yükleyin.

## Yükleme

1. Arduino IDE'yi açın
2. `sketch_apr21b/sketch_apr21b.ino` dosyasını açın
3. Yukarıdaki kütüphaneleri yükleyin
4. Kart: **Arduino Uno**, port seçin → **Upload**

Alternatif olarak kaynak kodun düz metin sürümü `breakout.txt` içinde yer alır.

## Dosyalar

```
Breakout-Game/
├── sketch_apr21b/
│   ├── sketch_apr21b.ino       # Ana oyun kodu
│   └── build/                  # Derlenmiş .hex, .elf çıktıları
└── breakout.txt                # Kaynak kodun düz metin kopyası
```

## Bağlam

Elektronik / gömülü sistemler dersi kapsamında prototip oyun çalışması.
