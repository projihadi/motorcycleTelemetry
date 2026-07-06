#include "imu.hpp"
#include "driver/i2c_types_legacy.h"
#include "esp_err.h"
#include "hal/i2c_types.h"
#include "soc/gpio_num.h"

static esp_err_t initI2C(void) {
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_0,

  }
}
