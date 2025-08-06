#include <stdio.h>
#include "esp_log.h"
#include "amg8833_simple.h"

#define TAG "AMG8833"

#define TOTAL_PIXELS (64*2)

/**
  AMG8833 Configuration function, should be called before calling other AMG8833 APIs
  @param dev Device handle
  */
esp_err_t amg8833_config(struct amg8833_dev_s *dev) {
	esp_err_t ret = ESP_ERR_INVALID_STATE;
	if(dev->i2c_bus_handle == NULL){
		return ret;
	}

	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = dev->dev_addr,
		.scl_speed_hz = 400000,
	};

	ret = i2c_master_probe(dev->i2c_bus_handle, dev->dev_addr, -1);
	if(ret != ESP_OK){
		return ret;
	}

	ret = i2c_master_bus_add_device(dev->i2c_bus_handle, &dev_cfg, &dev->dev_handle);
	if(ret != ESP_OK){
		return ret;
	}

	//Normal Mode Operation
	uint8_t buffer[2];
	buffer[0] = AMG8833_PCLT;
	buffer[1] = 0x00;
	ret = i2c_master_transmit(dev->dev_handle, buffer, 2, 1000);
	if(ret != ESP_OK){
		return ret;
	}

	//Inital Reset
	buffer[0] = AMG8833_RST;
	buffer[1] = 0x3F;
	ret = i2c_master_transmit(dev->dev_handle, buffer, 2, 1000);
	if(ret != ESP_OK){
		return ret;
	}

	//Disable Interrupts
	buffer[0] = AMG8833_INTC;
	buffer[1] = 0x00;
	ret = i2c_master_transmit(dev->dev_handle, buffer, 2, 1000);
	if(ret != ESP_OK){
		return ret;
	}

	//Set FPS to 10
	buffer[0] = AMG8833_FPSC;
	buffer[1] = 0x00;
	ret = i2c_master_transmit(dev->dev_handle, buffer, 2, 1000);
	if(ret != ESP_OK){
		return ret;
	}

	ret = ESP_OK;
	return ret;
}


/**
  Get the on-board thermistor value
  @param dev Device handle
  @param in_buff Input buffer to store the temperature value in floating value
  */
esp_err_t amg8833_get_temp(struct amg8833_dev_s *dev, float *in_buff){
	esp_err_t ret = ESP_ERR_INVALID_STATE;

	//Set Thermistor Register address
	uint8_t reg_addr = AMG8833_TTHL;
	uint8_t buffer[2];
	ret = i2c_master_transmit_receive(dev->dev_handle, &reg_addr, 1, buffer, 2, -1);
	if(ret != ESP_OK){
		return ret;
	}

	//Move to an integer
	uint16_t val = ((buffer[1] << 8) | buffer[0]);

	//Clear the sign bit and keep absolute value
	val &= ~(1 << 11);

	//Convert to float
	*in_buff = (buffer[1] & 8) ? (float)val * -0.0625 : (float)val * 0.0625;

	ret = ESP_OK;
	return ret;
}


/**
  Get the temperature array values (pixel frame) as float
  @param dev Device handle
  @param in_buff Input buffer to store the pixel values in floating value
  @param in_buff_size The buffer size, should be atleast 64 bytes
  */
esp_err_t amg8833_get_frame(struct amg8833_dev_s *dev, float *in_buff, size_t in_buff_size){
	esp_err_t ret = ESP_ERR_INVALID_STATE;

	if(in_buff_size < TOTAL_PIXELS/2){
		return ESP_ERR_INVALID_SIZE;
	}
	//Set Starting Pixel Register address
	uint8_t reg_addr = AMG8833_PIXEL_OFF;
	uint8_t buffer[TOTAL_PIXELS];
	ret = i2c_master_transmit_receive(dev->dev_handle, &reg_addr, 1, buffer, sizeof(buffer), -1);
	if(ret != ESP_OK){
		return ret;
	}

	for(int i=0; i<TOTAL_PIXELS; i+=2){
		//I saw this shift left and then right operation online, interesting how the data types work
		int16_t tmp = ((buffer[i+1] << 8) | buffer[i]) << 4;
		in_buff[i/2] = (float)(tmp >> 4) * 0.25;
	}

	ret = ESP_OK;
	return ret;
}



/**
  Get the raw temperature array values (pixel frame) as integer.
  Multiply by each pixel value by 0.25 to get the actual temperature
  @param dev Device handle
  @param in_buff Input buffer to store the pixel values in integer value
  @param in_buff_size The buffer size, should be atleast 64 bytes
  */
esp_err_t amg8833_get_rawframe(struct amg8833_dev_s *dev, int16_t *in_buff, size_t in_buff_size){
	esp_err_t ret = ESP_ERR_INVALID_STATE;

	if(in_buff_size < TOTAL_PIXELS/2){
		return ESP_ERR_INVALID_SIZE;
	}
	//Set Starting Pixel Register address
	uint8_t reg_addr = AMG8833_PIXEL_OFF;
	uint8_t buffer[TOTAL_PIXELS];
	ret = i2c_master_transmit_receive(dev->dev_handle, &reg_addr, 1, buffer, sizeof(buffer), -1);
	if(ret != ESP_OK){
		return ret;
	}

	for(int i=0; i<TOTAL_PIXELS; i+=2){
		//I saw this shift left and then right operation online, interesting how the data types work
		int16_t tmp = ((buffer[i+1] << 8) | buffer[i]) << 4;
		in_buff[i/2] = tmp >> 4;
	}

	ret = ESP_OK;
	return ret;
}
