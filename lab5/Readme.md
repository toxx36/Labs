# Lab 5

- Using static library `libled.a` for RGB LED control
- Make creates `libled.a` from source code, which stored in [`lib_src`](./lib_src/) directory. Lib will have built in the `lib` directory
- Program shows HEX-colors at RGB LED from array as demo

At this moment the library consist next feautures:
- Init GPIO and timer in PWM mode for LED control
- Set color as RGB via array of 3 values (Red, Green and Blue) from 0 to 255
- Set color by HEX color code, for example 0xFACE8D
- Set color by raw value of timer PWM period 
- Color value converts from linear value to parabolic for almost linear eye perception of brightness change