#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "amg8833_simple.h"

#define TAG "IRCAM"

void app_main(void)
{
	ESP_LOGI(TAG, "Main Function");

	i2c_master_bus_handle_t bus_handle = NULL;

	i2c_master_bus_config_t i2c_mst_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = I2C_NUM_0,
		.scl_io_num = 9,
		.sda_io_num = 10,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = true,
	};

	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

	struct amg8833_dev_s amg8833_dev = {
		.dev_addr = 0x69,
		.i2c_bus_handle = bus_handle
	};

	vTaskDelay(pdMS_TO_TICKS(150));

	//Call AMG8833 Config
	ESP_ERROR_CHECK(amg8833_config(&amg8833_dev));


	float val = 0;
	float buf[64];
	int16_t rawbuf[64];
	while(1){
		//Get On-board thermistor Value
		ESP_ERROR_CHECK(amg8833_get_temp(&amg8833_dev, &val));

		ESP_LOGI(TAG, "On-board Temperature : %f", val);

		//Get pixel values
		ESP_ERROR_CHECK(amg8833_get_frame(&amg8833_dev, buf, 64));

		ESP_LOGI(TAG, "Frame :");
		for(int i=0; i<64; i+=8){
			ESP_LOGI(TAG, "%f, %f, %f, %f, %f, %f, %f, %f", buf[i],buf[i+1], buf[i+2],buf[i+3],buf[i+4],buf[i+5],buf[i+6],buf[i+7]);
		}
		vTaskDelay(pdMS_TO_TICKS(150));

		//Get RAW pixel values
		ESP_ERROR_CHECK(amg8833_get_rawframe(&amg8833_dev, rawbuf, 64));

		ESP_LOGI(TAG, "Raw Frame :");
		for(int i=0; i<64; i+=8){
			ESP_LOGI(TAG, "%d, %d, %d, %d, %d, %d, %d, %d", rawbuf[i],rawbuf[i+1],rawbuf[i+2],rawbuf[i+3],rawbuf[i+4],rawbuf[i+5],rawbuf[i+6],rawbuf[i+7]);
		}
		vTaskDelay(pdMS_TO_TICKS(150));
	}
}
