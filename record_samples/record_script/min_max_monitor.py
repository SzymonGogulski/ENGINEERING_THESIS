import serial

def monitor_serial_port(port, baudrate):
    """
    Monitor a serial port, updating and printing the maximum and minimum values
    with each incoming sample.
    
    Args:
        port (str): Serial port name (e.g., 'COM3', '/dev/ttyUSB0').
        baudrate (int): Baud rate for the serial communication.
    """
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            max_val = 2050
            min_val = 2050
            
            print(f"Monitoring serial port {port} at baudrate {baudrate}...")
            
            while True:
                # Read a line from the serial port
                line = ser.readline().decode('utf-8').strip()
                
                if line:
                    try:
                        sample = float(line)

                        if sample > max_val:
                            max_val = sample
                            print(f"New max value: {max_val} , {min_val}")
                        
                        if sample < min_val:
                            min_val = sample
                            print(f"New min value: {max_val} , {min_val}")
                    
                    except ValueError:
                        print(f"Received non-numeric data: {line}")
                        
    except serial.SerialException as e:
        print(f"Serial port error: {e}")

monitor_serial_port('/dev/ttyACM0', 115200)