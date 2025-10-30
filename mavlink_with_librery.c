#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "c_library_v1/common/mavlink.h"  // must point to the mavlink v1 include path


int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_binary_file> <output_csv_file>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *fin = fopen(input_file, "rb");
    if (!fin) {
        perror("Error opening input file");
        return 1;
    }

    FILE *fout = fopen(output_file, "w");
    if (!fout) {
        perror("Error opening output file");
        fclose(fin);
        return 1;
    }

    // CSV header
    fprintf(fout, "lat_deg,lon_deg,alt_m\n");

    mavlink_message_t msg;
    mavlink_status_t status;
    uint8_t byte;

    while (fread(&byte, 1, 1, fin) == 1) {
        // Parse one byte at a time
        if (mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
            // We got a full MAVLink message
            if (msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
                mavlink_gps_raw_int_t gps;
                mavlink_msg_gps_raw_int_decode(&msg, &gps);

                // Convert from scaled integers to real units
                double lat_deg = gps.lat / 1e7;
                double lon_deg = gps.lon / 1e7;
                double alt_m   = gps.alt / 1000.0; // millimeters to meters

                fprintf(fout, "%.7f,%.7f,%.3f\n", lat_deg, lon_deg, alt_m);
                printf("%.7f,%.7f,%.3f\n", lat_deg, lon_deg, alt_m);
            }
        }
    }

    fclose(fin);
    fclose(fout);
    printf("Done. Parsed GPS_RAW_INT messages written to %s\n", output_file);
    return 0;
}
