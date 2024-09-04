#ifndef _MAX7219TASK_h
#define _MAX7219TASK_h
#include <Task.h>

#include "GlobalSettings.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>

#define MATRIX_COUNT 4
#define HEIGHT 8
#define WIDTH (8 * MATRIX_COUNT)
#define FONT_HEIGHT 8
#define FONT_GAP 1
#define STOPLESS 0x7FFF
#define MAX7219_MAX_CLOCK_SPEED_HZ (10000000) // 10 MHz


enum clockmode_t : uint8_t {NONE_MODE, WATCH_MODE,SCROLLCHAR_MODE, DAY_MODE,INFO_MODE, DISPLAY_MODE};
enum skind_t : uint8_t {SCR_UP, SCR_DOWN,SCR_OPEN,SCR_CLOSE, SCR_LEFT, SCR_RIGHT,SCR_RANDOM};

struct __attribute__((__packed__)) element_t {
    uint8_t x, y;
    char lett;
  };
 struct __attribute__((__packed__)) scrolling_t {
    uint8_t x, y, w, h;
    uint16_t width;
    int16_t pos;
    uint8_t * canvas;
  };

class Max7219Task : public Task{

    public:
    Max7219Task(const char *name, uint32_t stack,EventGroupHandle_t f, QueueHandle_t q, MessageBufferHandle_t m):Task(name, stack){
        flags = f;
        queue=q;
        messages=m;

    }
    
    protected:
    void setup() override;
    void loop() override;
    void cleanup() override;
    char charNormalize(char c);
    void sendCommand(uint8_t cmd, uint8_t value, uint8_t target=0xFF);
    void repaint();
    void setBrightness(uint8_t value);
    uint8_t getBrightness(){return _bright;};
    void beginUpdate();
    void endUpdate();
    void clear();
    bool getPixel(uint8_t x, uint8_t y);
    void setPixel(uint8_t x, uint8_t y, bool color);
    void drawPattern(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t pattern);
    void drawPattern(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *pattern);
    void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,bool what);
    void printChar(uint8_t x, uint8_t y, char c);
    void printChar(element_t el);
    void printStr(uint8_t x, uint8_t y, const char *str);
    void printStrCenter(const char *str);
    void printClock(uint8_t hour, uint8_t min);
    void scrollDigits(char * newtime, uint32_t tempo, skind_t kind);
    void scroll2center(const char *str, uint32_t tempo);
    void scroll(const char *str, uint32_t tempo);
    void noScroll();
    void prepareDisplay();
    void save_screen();
    void restore_screen();
    inline void noAnimate() {noScroll();};
    void animate(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t frames, const uint8_t *patterns, uint32_t tempo);
    uint8_t  charWidth(char c);
    uint16_t strWidth(const char *str);
    const uint8_t * charPattern(char c);
    void onMyTick();
    static void onTick(TimerHandle_t tm);

    spi_device_interface_config_t spi_cfg;
    spi_device_handle_t spi_dev;
  
    static const uint8_t FONT[] PROGMEM;
    static const uint8_t CHAR_WIDTH[] PROGMEM;
    static const char WEEKDAYS[7][3] PROGMEM;
    static const char MONTH[12][9] PROGMEM;
    uint8_t _bits[WIDTH];
    
    element_t watch[5];

    scrolling_t _scrolling;
    uint8_t _bright : 4;
    uint8_t _updating : 3;
    bool _anim : 1;
        
    clockmode_t cmode;
    

    skind_t skind;
    uint8_t second;
    int8_t temp;
    uint8_t hum;
    uint16_t press;
    uint8_t month;
    uint8_t day;
    uint8_t year;
    uint8_t wday;

    EventGroupHandle_t flags;
    QueueHandle_t queue;
    TimerHandle_t _timer;
    MessageBufferHandle_t messages;

    uint8_t display_period;
    TickType_t saved_period;
    clockmode_t saved_mode;
    uint8_t saved_bits[WIDTH];
    bool wait_display_end;
    char disp[6];
};
#endif