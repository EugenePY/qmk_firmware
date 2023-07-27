# Model OLED Image Specification

1. Only support signale image or Animation rendering.
2. OLED.IMG file specification

# OLED.IMG

1. the image is a fixed size file which containing two sector
- file header
- pixel content 

Piexel content is RGB565(16bit RGB https://rgbcolorpicker.com/565) 
2. this file is fix 129,024 (512x252)byte which just fit into the image buffer and loacated device's flash memory.  

the fix size design is to prevent user to put multiple files into the device.
which currently not support by the device, which due to the production cost.

 the header sector is documented, 

```c
struct {
 uint16_t n_frame; // number of the frame of the image,
 bool image_delay; // in ms, if n_frame is 1 this sector will be ignored. 
}
```


