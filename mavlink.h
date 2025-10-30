#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_PAYLOAD_LENGTH 256

#define HEARTBEAT_ID 0
#define SYSTEM_TIME_ID 2
#define GPS_RAW_INT_ID 24
#define ALTITUDE_ID 30
#define GLOBAL_POSITION_INT_ID 33

#define SYNC_BYTE 0xfe

typedef struct mavlink_header
{
    uint8_t payload_length;
    uint8_t packet_sequence;
    uint8_t system_id;
    uint8_t component_id;
    uint8_t messege_id;
} __attribute__((packed)) HEADER;

typedef struct mavlink_message
{
    HEADER header;
    uint8_t payload[MAX_PAYLOAD_LENGTH];
    uint16_t checksum;
    // TODO: add message time - time of messege arrival!
} Message;


// MESSAGE ID 0: HEARTBEAT (9 bytes)
typedef struct heartbeat_message
{
    uint32_t custom_mode;
    uint8_t type;
    uint8_t autopilot;
    uint8_t base_mode;
    uint8_t system_status;
    uint8_t mavlink_version;
} __attribute__((packed)) Heartbeat;

// MESSAGE ID 2: SYSTEM_TIME (12 bytes)
typedef struct system_time_message
{
    uint64_t time_unix_us; // UNIX epoch time in microseconds
    uint32_t time_boot_ms; // Time since system boot in ms
} __attribute__((packed)) SystemTime;

// MESSAGE ID 24: GPS_RAW_INT (30 bytes)
typedef struct gps_raw_int_message
{
    uint64_t time_usec;         // Timestamp (microseconds since UNIX epoch or system boot)
    int32_t lat;                // Latitude (degrees * 10^7)
    int32_t lon;                // Longitude (degrees * 10^7)
    int32_t alt;                // Altitude (mm above WGS84)
    uint16_t eph;               // GPS HDOP
    uint16_t epv;               // GPS VDOP
    uint16_t vel;               // GPS ground speed (cm/s)
    uint16_t cog;               // Course over ground (cdeg)
    uint8_t fix_type;           // 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: DGPS, 5: RTK float, 6: RTK fixed
    uint8_t satellites_visible; // Number of satellites visible
} GpsRawInt;

// MESSAGE ID 33: GLOBAL_POSITION_INT (36 bytes) - For location table
typedef struct global_position_int_message
{
    uint32_t time_boot_ms; // ms since system boot
    int32_t lat;           // Latitude (degrees * 10^7)
    int32_t lon;           // Longitude (degrees * 10^7)
    int32_t alt;           // Altitude MSL (mm)
    int32_t relative_alt;  // Altitude above ground (mm)
    int16_t vx;            // Ground X velocity (cm/s)
    int16_t vy;            // Ground Y velocity (cm/s)
    int16_t vz;            // Ground Z velocity (cm/s)
    uint16_t hdg;          // Vehicle heading (cdeg)
} GlobalPositionInt;

// MESSAGE ID 141: ALTITUDE (36 bytes)
typedef struct altitude_message
{
    uint64_t time_usec;         // Timestamp (microseconds)
    float altitude_monotonic;   // This is actually float (4 bytes)
    float altitude_amsl;        // This is actually float (4 bytes)
    float altitude_local;       // This is actually float (4 bytes)
    float altitude_relative;    // This is actually float (4 bytes)
    float altitude_terrain;     // This is actually float (4 bytes)
    float bottom_clearance;     // This is actually float (4 bytes)
} Altitude;

typedef struct
{
    unsigned long long SYS_TICK;
    double lon;
    double lat;
    double alt;
} ParsedData;

//functions
void parse_message(Message* massage);


