from machine import Pin
from time import sleep_ms, sleep_us, ticks_ms, ticks_us, ticks_diff

HALF_BIT_PERIOD = 889  # microseconds

IR_DIODE = 0
BUTTON_1 = 14
BUTTON_2 = 15

ir = Pin(IR_DIODE, Pin.OUT)
button1 = Pin(BUTTON_1, Pin.IN, Pin.PULL_UP)
button2 = Pin(BUTTON_2, Pin.IN, Pin.PULL_UP)

# Constants for RC5 protocol
RC5_START = 0b11
RC5_TOGGLE = 0b1
RC5_ADDR = 0b0


def send_bit(bit: int):
    """Send a single RC5 bit - Manchester (as per G. E. Thomas)."""
    if bit == 1:
        # Send '1' HIGH to LOW
        ir.on()
        sleep_us(HALF_BIT_PERIOD)
        ir.off()
        sleep_us(HALF_BIT_PERIOD)
    else:
        # Send '0' LOW to HIGH
        ir.off()
        sleep_us(HALF_BIT_PERIOD)
        ir.on()
        sleep_us(HALF_BIT_PERIOD)


def RC5_transmit(toggle: int, addr: int, command: int):
    """
    Accepts values:
        toggle: 0 or 1 (single bit)
        addr: from 0 to 31 (5 bits)
        command: from 0 to 63 (6 bits)

    RC5 frame consists of 14 bits in total:
    START(2 bits) TOGGLE(1 bit) ADDRESS (5 bits) COMMAND (6 bits).

    start is constant 0b11.

    bits are encoded with the Manchester encoding.
    This encoding divides bits to half bits with HALF_BIT_PERIOD = 889us.
    Depending on the implementation:

    Manchester (as per G. E. Thomas):
    bit 1 -> halfbits: 10 (HIGH to LOW)
    bit 0 -> halfbits: 01 (LOW to HIGH)

    Manchester (as per IEEE 802.3):
    bit 1 -> halfbits: 01 (LOW to HIGH)
    bit 0 -> halfbits: 10 (HIGH to LOW)

    total message durration 14(bits) x 2(halfbits) x 889us = 24892us ~ 24.9ms
    """
    start = 0b11

    rc5_frame = start << 12 | toggle << 11 | (
        addr & 0x1F) << 6 | (command & 0x3F)

    start_time = ticks_us()

    # Transmit each bit in the RC5 frame
    for i in range(14):
        # Extract each bit from MSB to LSB
        bit = (rc5_frame >> (13 - i)) & 0x01
        send_bit(bit)

    end_time = ticks_us()
    execution_time_us = ticks_diff(end_time, start_time)
    print(f"Execution time: {execution_time_us} microseconds")

    ir.off()


ir.off()

while True:
    if not button1.value():
        # print("Button 1 pressed. ")
        # print("sending value 1. ")
        # RC5_transmit(0, 0, 1)
        ir.toggle()
        print(">toggle led\r\n")
        sleep_ms(200)
