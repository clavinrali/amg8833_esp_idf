# IR CAM Application

This is a test application for the AMG8833 Grid-EYE.
It displays the pixel values onto the serial console

## To Run
```
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```
