#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "c_library_v1/common/mavlink.h"

// Configure and open the serial port
int open_serial_port(const char *device, int baudrate)
{
    int fd = open(device, O_RDONLY | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Error opening serial port");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("Error from tcgetattr");
        close(fd);
        return -1;
    }

    // Baud rate
    speed_t speed;
    switch (baudrate) {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        case 230400: speed = B230400; break;
        case 460800: speed = B460800; break;
        case 921600: speed = B921600; break;
        default:
            fprintf(stderr, "Unsupported baudrate %d\n", baudrate);
            close(fd);
            return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // Configure 8N1, no flow control, raw mode
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;    // disable break processing
    tty.c_lflag = 0;           // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;           // no remapping, no delays
    tty.c_cc[VMIN]  = 1;       // read blocks until 1 byte arrives
    tty.c_cc[VTIME] = 0;       // no read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // disable xon/xoff
    tty.c_cflag |= (CLOCAL | CREAD);        // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // no parity
    tty.c_cflag &= ~CSTOPB;                 // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                // no RTS/CTS

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <serial_device> <baudrate> <output_csv>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/ttyUSB0 57600 gps_data.csv\n", argv[0]);
        return 1;
    }

    const char *serial_device = argv[1];
    int baudrate = atoi(argv[2]);
    const char *output_file = argv[3];

    int fd = open_serial_port(serial_device, baudrate);
    if (fd < 0)
        return 1;

    FILE *fout = fopen(output_file, "w");
    if (!fout) {
        perror("Error opening output file");
        close(fd);
        return 1;
    }

    fprintf(fout, "lat_deg,lon_deg,alt_m\n");

    mavlink_message_t msg;
    mavlink_status_t status;
    uint8_t byte;

    printf("Reading MAVLink data from %s at %d baud...\n", serial_device, baudrate);

    while (1) {
        int n = read(fd, &byte, 1);
        if (n > 0) {
            if (mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
                if (msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
                    mavlink_gps_raw_int_t gps;
                    mavlink_msg_gps_raw_int_decode(&msg, &gps);

                    double lat_deg = gps.lat / 1e7;
                    double lon_deg = gps.lon / 1e7;
                    double alt_m   = gps.alt / 1000.0;

                    fprintf(fout, "%.7f,%.7f,%.3f\n", lat_deg, lon_deg, alt_m);
                    fflush(fout);

                    printf("GPS: lat=%.7f lon=%.7f alt=%.2f\n", lat_deg, lon_deg, alt_m);
                }
            }
        } else if (n < 0) {
            perror("Error reading serial port");
            break;
        }
    }

    fclose(fout);
    close(fd);
    return 0;
}
