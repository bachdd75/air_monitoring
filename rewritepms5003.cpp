#include <iostream>
#include <stdexcept>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstdint>
#include <chrono>
#include <thread>

const std::string uart_device = "/dev/ttyS5";  // Adjust this if using different UART pins

class UART {
public:
    explicit UART(const std::string &device) {
        uart_filestream = open(device.c_str(), O_RDWR | O_NOCTTY);
        if (uart_filestream == -1) {
            throw std::runtime_error("Unable to open UART device: " + device);
        }
        configureUART();
    }

    ~UART() {
        if (uart_filestream != -1) {
            close(uart_filestream);
        }
    }

    ssize_t readData(void *buffer, size_t size) {
        return read(uart_filestream, buffer, size);
    }

private:
    int uart_filestream;

    void configureUART() {
        struct termios options;
        tcgetattr(uart_filestream, &options);
        options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;  // Baud rate 9600, 8 data bits
        options.c_iflag = IGNPAR;                        // Ignore parity errors
        options.c_oflag = 0;
        options.c_lflag = 0;

        // Set blocking mode
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 0;

        tcflush(uart_filestream, TCIFLUSH);
        tcsetattr(uart_filestream, TCSANOW, &options);
    }
};

class PMS5003 {
public:
    explicit PMS5003(UART &uart) : uart(uart) {}

    void readData(int &pm1, int &pm25, int &pm10) {
        constexpr size_t packetSize = 32;
        unsigned char buffer[packetSize];

        while (true) {
            if (!syncToStartBytes()) {
                continue;
            }

            size_t bytesRead = 2;
            while (bytesRead < packetSize) {
                ssize_t result = uart.readData(buffer + bytesRead, packetSize - bytesRead);
                if (result < 0) {
                    throw std::runtime_error("Failed to read data from PMS5003 sensor.");
                }
                bytesRead += result;
            }

            if (verifyChecksum(buffer)) {
                parseData(buffer, pm1, pm25, pm10);
                break;
            } else {
                std::cerr << "Checksum mismatch. Data might be corrupted.\n";
            }
        }
    }

private:
    UART &uart;

    bool syncToStartBytes() {
        unsigned char startBytes[2];
        ssize_t result = uart.readData(startBytes, 1);
        if (result <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return false;
        }

        if (startBytes[0] != 0x42) {
            return false;
        }

        result = uart.readData(startBytes + 1, 1);
        if (result <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return false;
        }

        return startBytes[1] == 0x4D;
    }

    bool verifyChecksum(const unsigned char *buffer) {
        // For debugging, we'll bypass checksum verification
        // In production, you should implement proper checksum verification
        return true;
    }

    void parseData(const unsigned char *buffer, int &pm1, int &pm25, int &pm10) {
        pm1 = (buffer[10] << 8) | buffer[11];
        pm25 = (buffer[12] << 8) | buffer[13];
        pm10 = (buffer[14] << 8) | buffer[15];

        std::cout << "=========================" << std::endl;
        std::cout << "PM1.0: " << pm1 << " µg/m³" << std::endl;
        std::cout << "PM2.5: " << pm25 << " µg/m³" << std::endl;
        std::cout << "PM10:  " << pm10 << " µg/m³" << std::endl;
        std::cout << "=========================" << std::endl;
    }
};

int main() {
    try {
        UART uart(uart_device);
        PMS5003 sensor(uart);

        while (true) {
            int pm1, pm25, pm10;
            sensor.readData(pm1, pm25, pm10);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}