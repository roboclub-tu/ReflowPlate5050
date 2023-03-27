// Adafruit_GFX.cpp and Adafruit_GFX.h are modified!

#include <Arduino.h>
#include <vector>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

// #include <TouchScreen.h> //original version (doesn't work with this tft pinout!)

#include "TouchScreen_kbv.h" //edited version
#define TouchScreen TouchScreen_kbv
#define TSPoint TSPoint_kbv

#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 27, XM = 15, YP = 4, YM = 14; // 240x320 ID=0x0154

// const int TS_LEFT = 964, TS_RT = 179, TS_TOP = 215, TS_BOT = 940;
const int TS_LEFT = 940, TS_RT = 215, TS_TOP = 964, TS_BOT = 179; // original

const int screenWidth = 320, screenHeight = 240;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button sect1_btn, sect2_btn, sect3_btn, sect4_btn, center_btn, onof_btn;
bool sect1_state = false, sect2_state = false, sect3_state = false, sect4_state = false, onof_state = false;

int pixel_x, pixel_y; // Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT); // restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH); // because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed)
    {
        if (tft.getRotation() == 1 || tft.getRotation() == 3)
        {
            // Swap x and y coordinates for landscape orientation
            pixel_x = map(p.y, TS_TOP, TS_BOT, 0, 320);
            pixel_y = map(p.x, TS_RT, TS_LEFT, 0, 240);
            // pixel_x = map(p.y, TS_LEFT, TS_RT, 0, 320);
            // pixel_y = map(p.x, TS_TOP, TS_BOT, 0, 240);
        }
        else
        {
            // Use normal x and y coordinates for portrait orientation
            pixel_x = map(p.x, TS_LEFT, TS_RT, 0, 240);
            pixel_y = map(p.y, TS_TOP, TS_BOT, 0, 320);
        }
    }
    return pressed;
}

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

void drawCenterPointRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                              int16_t r, uint16_t color)
{
    tft.drawRoundRect(x - (w / 2), y - (h / 2), w, h, r, color);
}

int lastXValue = 0;

void writeHeader(String text)
{
    // tft.fillRect(0, 0, 320, 35, BLACK); //delete old text

    int fontSize = 2;
    int textLen = text.length();
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(fontSize);

    tft.setCursor(lastXValue, 15);
    tft.print(String("                    "));

    lastXValue = (screenWidth / 2) - (textLen * 3 * fontSize);
    tft.setCursor(lastXValue, 15);
    tft.print(String(text));
}

void writeTemp(int section, int temp, int goalTemp = 0)
{
    int fontSize = 2;
    String text = "3/90"; //(String)(temp + "°/" + goalTemp + (String)"°");
    int textLen = text.length();
    int rectSize = 67;

    // 126.5, 86.5,
    // 193.5, 86.5,
    // 126.5, 153.5
    // 193.5, 153.5

    // Calculate x for text to be centered
    tft.setCursor(93 + (rectSize / 2) - (textLen * 3 * fontSize),
                  53 + (rectSize / 2) - (4 * fontSize));
    tft.setTextColor(BLACK);
    tft.setTextSize(fontSize);
    tft.print(text);
}

std::vector<Adafruit_GFX_Button> sections;
bool sectionsState[4] = {false, false, false, false};
bool onof_bt_state = false;

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3)
        ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1); // Landcape
    tft.fillScreen(BLACK);

    sect1_btn.initButton(&tft, 126.5 - 15, 86.5, 67 + 30, 67, WHITE, BLACK, WHITE, const_cast<char *>(""), 2, 1, 0, 0, 0);
    sect2_btn.initButton(&tft, 193.5 + 15, 86.5, 67 + 30, 67, WHITE, BLACK, WHITE, const_cast<char *>(""), 2, 0, 1, 0, 0);
    sect3_btn.initButton(&tft, 126.5 - 15, 153.5, 67 + 30, 67, WHITE, BLACK, WHITE, const_cast<char *>(""), 2, 0, 0, 0, 1);
    sect4_btn.initButton(&tft, 193.5 + 15, 153.5, 67 + 30, 67, WHITE, BLACK, WHITE, const_cast<char *>(""), 2, 0, 0, 1, 0);
    // center_btn.initButton(&tft, 160, 120, 30, 30, 30, WHITE, BLACK, WHITE, const_cast<char *>(""), 2);
    onof_btn.initButton(&tft, 160, 215, 90, 35, WHITE, BLACK, WHITE, const_cast<char *>("START"), 2);

    sections.push_back(sect1_btn);
    sections.push_back(sect2_btn);
    sections.push_back(sect3_btn);
    sections.push_back(sect4_btn);

    sect1_btn.initButton(&tft, 126.5 - 15, 86.5, 67 + 30, 67, RED, BLACK, WHITE, const_cast<char *>(""), 2, 1, 0, 0, 0);
    sect2_btn.initButton(&tft, 193.5 + 15, 86.5, 67 + 30, 67, RED, BLACK, WHITE, const_cast<char *>(""), 2, 0, 1, 0, 0);
    sect3_btn.initButton(&tft, 126.5 - 15, 153.5, 67 + 30, 67, RED, BLACK, WHITE, const_cast<char *>(""), 2, 0, 0, 0, 1);
    sect4_btn.initButton(&tft, 193.5 + 15, 153.5, 67 + 30, 67, RED, BLACK, WHITE, const_cast<char *>(""), 2, 0, 0, 1, 0);

    sections.push_back(sect1_btn);
    sections.push_back(sect2_btn);
    sections.push_back(sect3_btn);
    sections.push_back(sect4_btn);

    for (size_t i = 0; i < 4; i++)
    {
        sections[i].drawButton(false);
    }

    // sect1_btn.drawButton(false);
    // sect2_btn.drawButton(false);
    // sect3_btn.drawButton(false);
    // sect4_btn.drawButton(false);

    // center_btn.drawButton(false);

    onof_btn.drawButton(false);

    writeHeader("Select section");
}

void handleSectionPress(int i, int pixel_x, int pixel_y)
{
    bool down = Touch_getXY();

    sections[i].press(down && sections[i].contains(pixel_x, pixel_y));
    if (sections[i].justReleased())
    {
        // sections[i].drawButton();
    }
    if (sections[i].justPressed())
    {
        if (sectionsState[i]) // on
        {
            sections[i].drawButton();
            sectionsState[i] = false;
        }
        else // of
        {
            sections[i + 4].drawButton();
            sectionsState[i] = true;
        }

        // Serial.print("Changed section ");
        // Serial.println(i);
        // Serial.println(sectionsState[i]);
        // Serial.println();

        // action
    }
}

void handleOnOf(bool state)
{
    if (state)
    { // on
      // for (size_t i = 0; i < 4; i++)
      // {
      //     if (sectionsState[i] == true)
      //     {
      //         sections[i + 4].drawButton(RED, RED, WHITE, true);
      //     }
      // }
    }
    else
    { // of
        for (size_t i = 0; i < 4; i++)
        {
            if (sectionsState[i] == true)
            {
                sections[i + 4].drawButton(false);
            }
        }
    }
}

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis4 = 0;
unsigned long interval = 300;

void startHeating()
{
    writeHeader("Heating");

    // TODO: organize the code in functions
    // TODO: make ite possible to turn of the system during heating

    // simulation of heating sections
    int currentTemp = 101;
    int goalTemp = 240;

    if (sectionsState[0] || sectionsState[1] || sectionsState[2] || sectionsState[3]){
        while (currentTemp <= goalTemp)
        {
            if (sectionsState[0] == true)
            {
                // TODO: get real temperature
                unsigned long currentMillis = millis();

                if (currentMillis - previousMillis1 >= interval)
                {
                    previousMillis1 = currentMillis;

                    sections[0 + 4].drawButton(RED, RED, WHITE, String(currentTemp) + "/" + goalTemp, true);
                    ++currentTemp;
                }
            }

            if (sectionsState[1] == true)
            {
                // TODO: get real temperature
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis2 >= interval)
                {
                    previousMillis2 = currentMillis;

                    sections[1 + 4].drawButton(RED, RED, WHITE, String(currentTemp) + "/" + goalTemp, true);
                    ++currentTemp;
                }
            }
            if (sectionsState[2] == true)
            {
                // TODO: get real temperature
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis3 >= interval)
                {
                    previousMillis3 = currentMillis;

                    sections[2 + 4].drawButton(RED, RED, WHITE, String(currentTemp) + "/" + goalTemp, true);
                    ++currentTemp;
                }
            }
            if (sectionsState[3] == true)
            {
                // TODO: get real temperature
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis4 >= interval)
                {
                    previousMillis4 = currentMillis;

                    sections[3 + 4].drawButton(RED, RED, WHITE, String(currentTemp) + "/" + goalTemp, true);
                    ++currentTemp;
                }
            }
        }
    }
}

void loop(void)
{
    int x, y;
    if (Touch_getXY())
    {
        TSPoint p = ts.getPoint();
        x = map(p.y, TS_TOP, TS_BOT, 0, 320);
        y = map(p.x, TS_RT, TS_LEFT, 0, 240);
        handleSectionPress(0, x, y);
        handleSectionPress(1, x, y);
        handleSectionPress(2, x, y);
        handleSectionPress(3, x, y);
    }

    bool down = Touch_getXY();

    onof_btn.press(down && onof_btn.contains(pixel_x, pixel_y));
    if (onof_btn.justReleased())
    {
        onof_btn.drawButton(false);
    }
    if (onof_btn.justPressed())
    {
        if (onof_bt_state) // on
        {
            onof_btn.initButton(&tft, 160, 215, 90, 35, WHITE, GREEN, BLACK, const_cast<char *>("START"), 2);
            onof_bt_state = false;
            handleOnOf(onof_bt_state);
            writeHeader("Select section");
        }
        else // of
        {
            onof_btn.initButton(&tft, 160, 215, 90, 35, WHITE, RED, WHITE, const_cast<char *>("STOP"), 2);
            onof_bt_state = true;
            handleOnOf(onof_bt_state);
            startHeating();
        }
        onof_btn.drawButton(true);
    }
}

// sections[1].press(down && sections[1].contains(pixel_x, pixel_y));
// touchSection(1);

// sect1_btn.press(down && sect1_btn.contains(pixel_x, pixel_y));
// if (sect1_btn.justReleased())
// {
//     sect1_btn.drawButton(false);
//     // center_btn.drawButton(false);
// }
// if (sect1_btn.justPressed())
// {
//     sect1_btn.drawButton(true);
//     // action
// }

// sect2_btn.press(down && sect2_btn.contains(pixel_x, pixel_y));
// if (sect2_btn.justReleased())
// {
//     sect2_btn.drawButton(false);
//     // center_btn.drawButton(false);
// }
// if (sect2_btn.justPressed())
// {
//     sect2_btn.drawButton(true);
//     // action
// }

// sect3_btn.press(down && sect3_btn.contains(pixel_x, pixel_y));
// if (sect3_btn.justReleased())
// {
//     sect3_btn.drawButton(false);
//     // center_btn.drawButton(false);
// }
// if (sect3_btn.justPressed())
// {
//     sect3_btn.drawButton(true);
//     // action
// }

// sect4_btn.press(down && sect4_btn.contains(pixel_x, pixel_y));
// if (sect4_btn.justReleased())
// {
//     // sect4_btn.drawButton(false);
//     // center_btn.drawButton(false);
// }
// if (sect4_btn.justPressed())
// {
//     if (sect4_state) // on
//     {
//         sect4_btn.initButton(&tft, 193.5, 153.5, 67, 67, WHITE, BLACK, WHITE, const_cast<char *>(""), 2, 0, 0, 1, 0);
//         sect4_state = false;
//         sect4_btn.drawButton();
//     }
//     else // of
//     {
//         // sect4_btn.initButton(&tft, 193.5, 153.5, 67, 67, RED, BLACK, WHITE, const_cast<char *>(""), 2, 0, 0, 1, 0);
//         sections[4].drawButton();
//         sect4_state = true;
//     }

//     // sect4_btn.drawButton();

//     // action
// }

// center_btn.press(down && center_btn.contains(pixel_x, pixel_y));
// if (center_btn.justReleased())
// {
//     center_btn.drawButton(false);
// }
// if (center_btn.justPressed())
// {
//     center_btn.drawButton(true);
//     // action
// }