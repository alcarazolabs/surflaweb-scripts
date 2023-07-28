#if 1
//By @surflaweb | Freddy Alcarazo | creatorpart@gmail.com

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=7,XM=A1,YP=A2,YM=6; //240x320 ID=0x9341
const int TS_LEFT=870,TS_RT=138,TS_TOP=941,TS_BOT=161;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Crear variables de los botones:
Adafruit_GFX_Button on_btn_led1, off_btn_led1, btn_text_led1;
Adafruit_GFX_Button on_btn_led2, off_btn_led2, btn_text_led2;
Adafruit_GFX_Button on_btn_led3, off_btn_led3, btn_text_led3;


int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


const int led1 = 22;
const int led2 = 24;
const int led3 = 26;

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(0);            //PORTRAIT
    tft.fillScreen(BLACK);
    // Pintar botones LED1
    btn_text_led1.initButton(&tft,  120, 30, 200, 30, WHITE, WHITE, BLACK, "LED 1", 2);
    on_btn_led1.initButton(&tft,  60, 70, 100, 40, WHITE, CYAN, BLACK, "ON", 2); // (&tft, margen izquierdo/derecho (menor valor se va a la izquierda y mayor a la derecha), coordenada y (sube/baja), ancho del boton, altura del boton)
    off_btn_led1.initButton(&tft, 180, 70, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    btn_text_led1.drawButton(true);
    on_btn_led1.drawButton(false);
    off_btn_led1.drawButton(false);

    // Pintar botones LED2
    btn_text_led2.initButton(&tft,  120, 110, 200, 30, WHITE, WHITE, BLACK, "LED 2", 2);
    btn_text_led2.drawButton(true);
    on_btn_led2.initButton(&tft,  60, 150, 100, 40, WHITE, CYAN, BLACK, "ON", 2); // (&tft, margen izquierdo/derecho (menor valor se va a la izquierda y mayor a la derecha), coordenada y (sube/baja), ancho del boton, altura del boton)
    off_btn_led2.initButton(&tft, 180, 150, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    on_btn_led2.drawButton(false);
    off_btn_led2.drawButton(false);

     // Pintar botones LED3
    btn_text_led3.initButton(&tft,  120, 190, 200, 30, WHITE, WHITE, BLACK, "LED 3", 2);
    btn_text_led3.drawButton(true);
    on_btn_led3.initButton(&tft,  60, 230, 100, 40, WHITE, CYAN, BLACK, "ON", 2); // (&tft, margen izquierdo/derecho (menor valor se va a la izquierda y mayor a la derecha), coordenada y (sube/baja), ancho del boton, altura del boton)
    off_btn_led3.initButton(&tft, 180, 230, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    on_btn_led3.drawButton(false);
    off_btn_led3.drawButton(false);

    pinMode(led1, OUTPUT);

    
}

/* two buttons are quite simple
 */
void loop(void)
{
    bool down = Touch_getXY();
    
    on_btn_led1.press(down && on_btn_led1.contains(pixel_x, pixel_y));
    off_btn_led1.press(down && off_btn_led1.contains(pixel_x, pixel_y));

    // LED 1
    
    if (on_btn_led1.justReleased())
        on_btn_led1.drawButton();
        
    if (off_btn_led1.justReleased())
        off_btn_led1.drawButton();
        
    if (on_btn_led1.justPressed()) {
        on_btn_led1.drawButton(true);

        //Prender LED1
        digitalWrite(led1, HIGH);
    }
    if (off_btn_led1.justPressed()) {
        off_btn_led1.drawButton(true);
        //Apagar LED1
         digitalWrite(led1, LOW);
    }

    //LED 2
      on_btn_led2.press(down && on_btn_led2.contains(pixel_x, pixel_y));
      off_btn_led2.press(down && off_btn_led2.contains(pixel_x, pixel_y));
      
    if (on_btn_led2.justReleased())
        on_btn_led2.drawButton();
        
    if (off_btn_led2.justReleased())
        off_btn_led2.drawButton();
        
    if (on_btn_led2.justPressed()) {
        on_btn_led2.drawButton(true);

        //Prender LED2
        digitalWrite(led2, HIGH);
    }
    if (off_btn_led2.justPressed()) {
        off_btn_led2.drawButton(true);
        //Apagar LED2
         digitalWrite(led2, LOW);
    }

    //LED 3
      on_btn_led3.press(down && on_btn_led3.contains(pixel_x, pixel_y));
      off_btn_led3.press(down && off_btn_led3.contains(pixel_x, pixel_y));
      
    if (on_btn_led3.justReleased())
        on_btn_led3.drawButton();
        
    if (off_btn_led3.justReleased())
        off_btn_led3.drawButton();
        
    if (on_btn_led3.justPressed()) {
        on_btn_led3.drawButton(true);

        //Prender LED3
        digitalWrite(led3, HIGH);
    }
    if (off_btn_led3.justPressed()) {
        off_btn_led3.drawButton(true);
        //Apagar LED3
         digitalWrite(led3, LOW);
    }
}
#endif
