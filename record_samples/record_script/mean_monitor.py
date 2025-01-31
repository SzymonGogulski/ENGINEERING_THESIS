import serial

def calculate_running_mean(serial_port, baud_rate):
    """
    Reads values from the serial port, calculates the running mean, and prints it.

    Parameters:
    - serial_port: The name of the serial port (e.g., 'COM3', '/dev/ttyUSB0').
    - baud_rate: The baud rate for the serial communication.
    """
    try:
        with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
            mean = 0.0
            count = 0

            print("Reading data from serial port... Press Ctrl+C to stop.")
            while True:
                line = ser.readline().decode('utf-8').strip()
                
                try:
                    value = float(line)

                    count += 1
                    mean += (value - mean) / count

                    print(f"Sample: {value:.2f}, Running Mean: {mean:.2f}")
                
                except ValueError:
                    print(f"Invalid input: {line}")
    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("\nStopped by user.")


calculate_running_mean('/dev/ttyACM0', 115200)