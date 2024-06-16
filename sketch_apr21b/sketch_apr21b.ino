#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128 // OLED ekran genişliği
#define SCREEN_HEIGHT 64 // OLED ekran yüksekliği
#define OLED_RESET    -1
#define CAN_SIZE 2 // Canın boyutu
#define CAN_SPEED 1 // Canın düşüş hızı

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int SevenSegDispPins[] = {2,3,4,5,6,7,8};

#define radius 3 // Topun yarıçapı
#define LED_COUNT 3 // LED sayısı
const int LED_PINS[] = {11, 12, 13}; // Örnek pin numaraları, gerektiğinde ayarlayın
#define BALL_SPEED_INCREMENT 20 // Her bölüm geçildiğinde top hızının artış yüzdesi
#define brickWidth 12
#define brickHeight 4

// Buton pinleri
#define LEFT_BUTTON 0
#define RIGHT_BUTTON 9
#define SELECT_BUTTON 10

// Oyun durumları
enum GameState {
  MENU,
  GAMEPLAY,
  GAMEOVER
};

int arda=0;
int score = 0; // Oyuncunun skoru
int lives = 3; // Oyuncunun can sayısı
int ballSpeed =1; // Topun başlangıç hızı
int paddlePosition_x = 0;
int paddlePosition_y = 61; // Paletin başlangıç pozisyonu
bool gameOver = false; // Oyunun bitip bitmediği kontrolü
int ali =0;
int araba=1;
int sayi_y=4;
int sayi_x=1;
// Topun pozisyon ve hızı
float ballX = SCREEN_WIDTH / 2;
float ballY = SCREEN_HEIGHT / 2;
float ballDX = 1; //Topun x ekseninde hangi yöne doğru gideceğini tayin eder.
float ballDY = 1; //Topun y ekseninde hangi yöne doğru gideceğini tayin eder.
int kontrol=0;
// Tuğlaların düzeni
bool bricks[2][8];
int tugla_sayisi;
int bonus_control=1;
int bonus_x=64;
int bonus_y=32;
int tekrar_bonus=1;
int menuSelection = 0; // Menü seçeneğini takip etmek için değişken
// Oyun durumunu takip etmek için değişken


GameState gameState = MENU;
void handleMenu() {
  // Başlat ve Çıkış seçeneklerini gösterme
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  if (menuSelection == 0) {
    display.println("> Baslat");
    display.setCursor(20, 40);
    display.println("  Cikis");
  } else if (menuSelection == 1) {
    display.println("  Baslat");
    display.setCursor(20, 40);
    display.println("> Cikis");
  }
  
  // Ekranı güncelleme
  display.display();
  // Yukarı ve aşağı butonlarını kontrol etme
  if (digitalRead(LEFT_BUTTON) == LOW) {
    menuSelection = 0; // Baslat seçeneğini seç
  } else if (digitalRead(RIGHT_BUTTON) == LOW) {
    menuSelection = 1; // Cikis seçeneğini seç
  }

  // Başlat veya Çıkış butonuna basılırsa oyunu başlat veya çıkış yap
  if (digitalRead(SELECT_BUTTON) == LOW) {
    if (menuSelection == 0) {
      gameState = GAMEPLAY; // Oyunu başlat
    } else {
      display.clearDisplay(); // Çıkış seçeneği seçildiği zaman ekran temizlenir.
    }
  }
}

void setup() {

   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 başlatılamadı. Ekran bağlantısını kontrol edin."));
    while (true);
  }
  // OLED ekranı temizleme
  display.clearDisplay();
  
  // LED pinlerini çıkış olarak ayarlama
  for (int i = 0; i < LED_COUNT; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  
  // 7 segment display pinlerini çıkış olarak ayarlama
  for (int i = 0; i < 7; i++) {
    pinMode(SevenSegDispPins[i], OUTPUT);
  }
  
  // Buton pinlerini giriş olarak ayarlama ve dahili pull-up dirençlerini etkinleştirme
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);
  
  handleMenu();
}

void moveBall() {


 
  // Topun hareketi
  ballX += ballDX * ballSpeed;
  ballY += ballDY * ballSpeed;
  
  // Ekran kenarlarına çarpma kontrolü
  if (ballX <= 0 || ballX >= SCREEN_WIDTH - radius) {
    ballDX = -ballDX;
  }

  if (ballY <= 0) {
    ballDY = -ballDY;
  }
  
  // Palet ile çarpışma kontrolü
  if((ballX+radius >= paddlePosition_x && ballX+radius <=paddlePosition_x+30))
  {
  if ((ballY+radius>=paddlePosition_y)) {
    ballDY = -ballDY;
  }
  }
   // Top ekranın altına düşerse
  if (ballY >= SCREEN_HEIGHT-radius-2 ) {
    // Topu başlangıç konumuna getir
    ballX = SCREEN_WIDTH / 2;
    ballY = SCREEN_HEIGHT / 2;

    // Oyuncunun canını azalt
    lives--;

    // Canları kontrol et
    if (lives <= 0) {
      // Oyunu bitir
      gameState = GAMEOVER;
      gameOver = true;
    }
  }

  // Tuğlalarla çarpışma kontrolü
  for (int x = 0; x < sayi_x; x++) {
    for (int y = 0; y < sayi_y; y++) {
      if (bricks[x][y]) {
        // Tuğlanın koordinatlarını hesapla
        int brickX = y * 16;
        int brickY = x * 8;
        
        // Topun çarpışma kutusunun koordinatlarını hesapla
        int ballLeft = ballX - radius;
        int ballRight = ballX + radius;
        int ballTop = ballY - radius;
        int ballBottom = ballY + radius;
        
        // Tuğla ile topun çarpışıp çarpışmadığını kontrol et
        if (ballRight >= brickX && ballLeft <= brickX + brickWidth &&
            ballBottom >= brickY && ballTop <= brickY + brickHeight) {
          // Tuğlayı yok et
          bricks[x][y] = false;
         if(tekrar_bonus==1)
         {
          bonus_x=64;
          bonus_y=32;
         }
          
          
          tugla_sayisi--;
          score++; 
          // Topun yönünü değiştir
          ballDY = -ballDY;
          // Topun pozisyonunu ayarla (çarpışma sonrası içeri doğru kayma)
          ballY += ballDY * ballSpeed;
          
          
          }
        }
      
      }
    }
  
  

}

// OLED ekranı güncelleyen fonksiyon
void updateDisplay() {
  display.clearDisplay();
  
  // Oyun durumuna göre farklı ekranlar gösterme

  if (gameState == GAMEPLAY) {
    // Oyun alanını ve canları gösterme
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.setCursor(0, 10);
    //Tuğlaları çizme
    for (int x = 0; x < sayi_x; x++) {
      for (int y = 0; y < sayi_y; y++) {

        if (bricks[x][y]==true) {
          display.fillRect(y * 16, x * 8, brickWidth, brickHeight, SSD1306_WHITE);
        }
      }
    }
   
    // Paleti çizme
    display.fillRect(paddlePosition_x,paddlePosition_y,30,3, SSD1306_WHITE);  //Paletin yüksekliği 3 birim , genişliği 20 birimdir.
    // Topu çizme
    display.fillCircle(ballX, ballY, radius, SSD1306_WHITE);
    // Bonus objeyi çizme
      if(1)
  {
    tekrar_bonus=0;
    display.fillCircle(bonus_x, bonus_y, radius/2, SSD1306_WHITE);
          bonus_y++;

          if((bonus_x=1>= paddlePosition_x && bonus_x+1 <=paddlePosition_x+30))
          {
         if ((bonus_y+1==paddlePosition_y)) {
          tekrar_bonus=1;
             lives++;
          }
         }
          if(bonus_y>SCREEN_HEIGHT )
          {
            tekrar_bonus=1;
          }
  }
    
  
  } else if (gameState == GAMEOVER) {
    // Oyun bittiğinde skoru gösterme
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 20);
    display.println("Oyun bitti!");
    display.setCursor(20, 40);
    display.println("Skor: " + String(score));
  }
  
  // Can göstergesi
  for (int i = 0; i < LED_COUNT; i++) {
    if (i < lives) {
      digitalWrite(LED_PINS[i], HIGH); // Can varsa LED'i yak
    } else {
      digitalWrite(LED_PINS[i], LOW); // Can yoksa LED'i söndür
    }
  }
  
  display.display();
  delay(10);
}

void loop() {

  if (gameState == MENU) {
    handleMenu(); // Menü işlemlerini yönet
  } else if (gameState == GAMEPLAY) {

    playGame1(); // Oyunu oyna

    if (gameOver) {
    } 
    
    else {
      
    }
  }

  if (gameState != MENU) {
    // Skoru 7 segment ekranda göster
    displayScoreOn7Seg(score);
  }

}

void playGame1() {
  if(araba==1)
  {
    
    sayi_y=4;
  enableBricks1();
  araba=2;
  }
 if(araba==3)
  {
    ballX = SCREEN_WIDTH / 2;
ballY = SCREEN_HEIGHT / 2;
delay(1000);
    sayi_y=8;
  enableBricks2();
  araba=4;
  }
  if(araba==5)
  {
    ballX = SCREEN_WIDTH / 2;
ballY = SCREEN_HEIGHT / 2;

    delay(1000);
    sayi_x=2;
    sayi_y=8;
  enableBricks3();
  araba=6;
  }
if(araba==7)
  {
     gameState=GAMEOVER;
    
  }

tuglalar_bitti_mi();
  movePaddle();
  
  moveBall();

  updateDisplay();

  // Skoru 7 segment displayde gösterme
  displayScoreOn7Seg(score);
 }

void displayScoreOn7Seg(int score) {
  switch(score%10) {
    case 0:
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      break;
    case 1:
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      break;
    case 2:
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      break;
    case 3:
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      break;
    case 4:
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      break;
    case 5:
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      break;
    case 6:
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      break;
    case 7:
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      break;
    case 8:
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      break;
    case 9:
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      break;
    default:
      break;
  }
}
// Paletin hareketini kontrol eden fonksiyon
void movePaddle() {
  // Sol butona basıldığında paleti sola hareket ettirme
  if (digitalRead(LEFT_BUTTON) == LOW) {
    paddlePosition_x -= 3; // Örnek olarak 3 birim sola hareket
  }
  // Sağ butona basıldığında paleti sağa hareket ettirme
  if (digitalRead(RIGHT_BUTTON) == LOW) {
    paddlePosition_x += 3; // Örnek olarak 3 birim sağa hareket
  }
  // Paletin ekran sınırlarını kontrol etme
  paddlePosition_x = constrain(paddlePosition_x, 0, SCREEN_WIDTH - 30); // Paletin genişliğini göz önünde bulundur
}
void enableBricks1() {
  for (int x = 0; x < 1; x++) {
    for (int y = 0; y < 4; y++) {
      bricks[x][y] = true; 
    }
  }

 tugla_sayisi=4;
}

void enableBricks2() {
  for (int x = 0; x < 1; x++) {
    for (int y = 0; y < 8; y++) {
      bricks[x][y] = true; 
    }
  }
 tugla_sayisi=8;
}

void enableBricks3() {
  for (int x = 0; x < 2; x++) {
    for (int y = 0; y < 8; y++) {
      bricks[x][y] = true;
    }
  }
 tugla_sayisi=16;
}

void tuglalar_bitti_mi()
{
  if(tugla_sayisi==0)
  {
    araba++;
  }
}



