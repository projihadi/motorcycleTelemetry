#pragma once
#include <LovyanGFX.hpp>
#include <lvgl.h>

#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK 14
#define PIN_NUM_CS 10
#define PIN_NUM_DC 12
#define PIN_NUM_RST 11
#define PIN_NUM_BCKL -1

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define FULL_SCREEN_BUF_SIZE                                                   \
  (SCREEN_WIDTH * SCREEN_HEIGHT * (LV_COLOR_DEPTH / 8))

class LGFX_Custom : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 panel;
  lgfx::Bus_SPI bus;

public:
  LGFX_Custom(void) {
    auto config = bus.config();
    config.spi_host = SPI2_HOST;
    config.spi_mode = 0;
    config.freq_write = 40000000;
    // config.freq_read = 16000000;
    config.pin_sclk = PIN_NUM_CLK;
    config.pin_mosi = PIN_NUM_MOSI;
    config.pin_miso = PIN_NUM_MISO;
    config.pin_dc = PIN_NUM_DC;
    bus.config(config);
    panel.setBus(&bus);

    auto panelConfig = panel.config();
    panelConfig.pin_cs = PIN_NUM_CS;
    panelConfig.pin_rst = PIN_NUM_RST;
    // panelConfig.pin_bl = PIN_NUM_BCKL;
    panelConfig.panel_width = 240;
    panelConfig.panel_height = 320;
    panelConfig.offset_x = 0;
    panelConfig.offset_y = 0;
    panelConfig.dummy_read_pixel = 8;
    panelConfig.dummy_read_bits = 1;
    panelConfig.readable = false;
    panelConfig.invert = false;
    panelConfig.rgb_order = false;
    panelConfig.dlen_16bit = false;
    panelConfig.bus_shared = true;
    panel.config(panelConfig);
    setPanel(&panel);
  }
};
