from machine import Pin
from time import sleep_ms, sleep_us, ticks_ms, ticks_diff

RC5_T1 = 889
RC5_FREQUENCY = 36000
MANCHESTER_BIT_PERIOD = 32000//36

IR_DIODE = 13
BUTTON_1 = 16
BUTTON_2 = 17

ir = Pin(IR_DIODE, Pin.OUT)
button1 = Pin(BUTTON_1, Pin.IN, Pin.PULL_UP)
button2 = Pin(BUTTON_2, Pin.IN, Pin.PULL_UP)


def RC5_transmit(command: int):

    if not (command >= 0 and command <= 63):
        print("Out of range. Transmission failed. ")
        return

    start_bits = 0b11
    control_bit = 0b0
    address = 0b00000

    message = (start_bits << 12) | (control_bit << 11) | (
        address << 6) | command

    def send_manchester_bit(bit):
        if bit == 0:
            ir.on()
            sleep_us(MANCHESTER_BIT_PERIOD)
            ir.off()
            sleep_us(MANCHESTER_BIT_PERIOD)
        else:
            # 1 -> LOW then HIGH
            ir.off()
            sleep_us(MANCHESTER_BIT_PERIOD)
            ir.on()
            sleep_us(MANCHESTER_BIT_PERIOD)

    # Transmit the message bit by bit
    start_time = ticks_ms()

    for i in range(13, -1, -1):  # Send 14 bits (start from MSB)
        bit = (message >> i) & 1
        send_manchester_bit(bit)

    # Ensure IR is turned off after transmission
    ir.off()

    end_time = ticks_ms()
    execution_time_ms = ticks_diff(end_time, start_time)
    print(f"Execution time: {execution_time_ms} milliseconds")


while True:
    if not button1.value():
        print("Button 1 pressed. ")
        print("sending value 1. ")
        RC5_transmit(1)
        sleep_ms(200)

    if not button2.value():
        print("button 2 pressed")
        print("sending value 2. ")
        RC5_transmit(2)
        sleep_ms(200)
