#pragma once
#include "lgfx/v1/panel/Panel_ILI9341.hpp"
#include <LovyanGFX.hpp>
#include <lvgl.h>

#define PIN_NUM_CS 14
#define PIN_NUM_RST 13
#define PIN_NUM_DC 12
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK 10
#define PIN_NUM_MISO 46
#define PIN_NUM_BCKL -1

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define FULL_SCREEN_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * (LV_COLOR_DEPTH / 8))

class LGFX_Custom : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 panel;
  lgfx::Bus_SPI bus;

public:
  LGFX_Custom(void) {
    auto config = bus.config();
    config.spi_host = SPI2_HOST;
    config.spi_mode = 0;
    config.freq_write = 40000000; // Stable sweet spot (27MHz). Drop to 16000000 if wires are long.
    config.freq_read = 16000000;
    config.spi_3wire = false;
    config.use_lock = true; // CRITICAL for thread/LVGL safety
    config.dma_channel = SPI_DMA_CH_AUTO;

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
    panelConfig.panel_width = SCREEN_WIDTH;
    panelConfig.panel_height = SCREEN_HEIGHT;
    panelConfig.offset_x = -80;
    panelConfig.offset_y = 0;
    panelConfig.dummy_read_pixel = 8;
    panelConfig.dummy_read_bits = 1;
    panelConfig.readable = false;
    panelConfig.invert = false;
    panelConfig.rgb_order = true;
    panelConfig.offset_rotation = 6;
    panelConfig.dlen_16bit = false;
    panelConfig.bus_shared = false; // Set to true ONLY if sharing SPI pins with an SD card
    panel.config(panelConfig);

    setPanel(&panel);
  }
};
