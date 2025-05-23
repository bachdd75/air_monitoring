from flask import Flask, jsonify
from flask_cors import CORS
import serial
import time
from datetime import datetime

app = Flask(__name__)
CORS(app)

# Configure serial port for Plantower sensor
# Note: Change the port according to your Raspberry Pi setup
SERIAL_PORT = '/dev/ttyUSB0'  # Common USB port on Raspberry Pi
BAUD_RATE = 9600

def read_pm25_data():
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2) as ser:
            # Clear buffer
            ser.reset_input_buffer()
            
            while True:
                # Plantower data frame is 32 bytes
                data = ser.read(32)
                
                if data[0] == 0x42 and data[1] == 0x4d:
                    # Extract PM2.5 value (bytes 6 and 7)
                    pm25 = (data[6] << 8) | data[7]
                    return {
                        'pm25': pm25,
                        'timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                    }
    except Exception as e:
        return {'error': str(e)}

@app.route('/api/pm25')
def get_pm25():
    data = read_pm25_data()
    return jsonify(data)

@app.route('/')
def index():
    return app.send_static_file('index.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000) 