- Using HW PWM TIM1
- Hold left button (PE0) to change color
- Press right button (PE1) to change intensity
- Hold black user button (PA0) on the main board before pressing buttons to reverse direction of changing
- Two LEDs toggles every second on the main board via TIM2 IRQ
- Using static library libled.a for RGB LED control
- Make creates libled.a from source code