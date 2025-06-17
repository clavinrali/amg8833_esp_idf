# AMG8833 Infrared Array Sensor Grid-EYE component

This is a simple and minimal driver for AMG8833 Low resolution IR camera sensor.
This driver can read the onboad thermistor and frame data. It uses the new I2C master APIs

## API
```C
esp_err_t amg8833_config(struct amg8833_dev_s *dev);										//Configuration function, should be called first
esp_err_t amg8833_get_temp(struct amg8833_dev_s *dev, float *buff);							//Get on-board thermistor value
esp_err_t amg8833_get_frame(struct amg8833_dev_s *dev, float *buff, size_t in_buff_size);	//Get the pixel values, size should be 64
esp_err_t amg8833_get_rawframe(struct amg8833_dev_s *dev, int16_t *in_buff, size_t in_buff_size);	//Get raw pixel values, size should be 64
```


## Example

```C
void app_main(void)
{
    ESP_LOGI(TAG, "Main Function");

    i2c_master_bus_handle_t bus_handle;

	//Set I2C bus
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = 9,
        .sda_io_num = 10,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,                                                                                                                   };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    struct amg8833_dev_s amg8833_dev = {
        .dev_addr = 0x69,                                                                                                                                           .i2c_bus_handle = bus_handle
    };
                                                                                                                                                                //Call AMG8833 Config
    ESP_ERROR_CHECK(amg8833_config(&amg8833_dev));

    float val = 0;
    float buf[64];                                                                                                                                              while(1){
        //Get On-board thermistor Value
        ESP_ERROR_CHECK(amg8833_get_temp(&amg8833_dev, &val));

        ESP_LOGI(TAG, "On-board Temperature : %f", val);

		//Get pixel values
		ESP_ERROR_CHECK(amg8833_get_frame(&amg8833_dev, buf, 64));
		for(int i=0; i<64; i+=8){
			ESP_LOGI(TAG, "%f, %f, %f, %f, %f, %f, %f, %f", buf[i],buf[i+1], buf[i+2],buf[i+3],buf[i+4],buf[i+5],buf[i+6],buf[i+7]);
		}
		vTaskDelay(pdMS_TO_TICKS(150));
	}
}
```
