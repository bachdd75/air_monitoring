# PM2.5 Air Quality Monitor

A web application for monitoring PM2.5 air quality data from a Plantower sensor using a Raspberry Pi.

## Hardware Requirements

- Raspberry Pi (any model with USB ports)
- Plantower PM2.5 sensor (PMS5003, PMS7003, or compatible)
- USB to TTL converter (if sensor doesn't have USB interface)

## Wiring Instructions

Connect the Plantower sensor to the Raspberry Pi:

1. If using USB to TTL converter:
   - VCC → 5V
   - GND → GND
   - TXD → RXD
   - RXD → TXD

2. If using USB interface, simply plug it into any USB port on the Raspberry Pi

## Software Setup

1. Install Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```

2. Update the serial port in `app.py` if necessary:
   - Default is set to `/dev/ttyUSB0`
   - You can find the correct port using:
     ```bash
     ls /dev/tty*
     ```

3. Run the application:
   ```bash
   python app.py
   ```

4. Access the web interface:
   - Open a browser on any device in your network
   - Navigate to `http://<raspberry-pi-ip>:5000`
   - Replace `<raspberry-pi-ip>` with your Raspberry Pi's IP address

## Features

- Real-time PM2.5 readings
- Automatic data updates every 5 seconds
- Color-coded air quality status
- Responsive design for mobile and desktop
- Error handling and status display

## Air Quality Index Reference

- Good: 0-12 µg/m³
- Moderate: 12.1-35.4 µg/m³
- Unhealthy: >35.4 µg/m³

## Troubleshooting

1. If you can't access the sensor:
   - Check the USB connection
   - Verify the serial port name
   - Ensure you have permission to access the serial port:
     ```bash
     sudo usermod -a -G dialout $USER
     ```
   - Restart the Raspberry Pi

2. If the web interface shows connection errors:
   - Check if the Flask application is running
   - Verify your network connection
   - Ensure the port 5000 is not blocked by firewall 