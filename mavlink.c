#include "mavlink.h"

#define FILENAME "mavlink.bin"

int main(){
    uint8_t byte;
    HEADER header;
    Message message;
    int read_lenght;
    printf("Hellow World\n");

    //open binary file (not corupted)
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    while(1){
        byte = fgetc(fp);
        if(byte == EOF) break;

        if(byte == SYNC_BYTE){
            // read header
            read_lenght = fread(&header, 1, sizeof(header), fp);
            if(read_lenght != sizeof(header)) break; // could not read successfuly
            
            // read payload
            read_lenght = fread(message.payload, 1, header.payload_length, fp);
            if(read_lenght != header.payload_length) break; // could not read successfuly

            fread(&message.checksum, 1, sizeof(message.checksum), fp);
            printf("messege read! message type: %d. message number: %d\n", header.messege_id, header.packet_sequence);
        } 
        else{
            perror("error in parsing - sync byte read wrong!\n");
            return 0;
        }

    }
    //read from file (byte by byte)

    //parse for messeges, create messege list
}


void parse_message(Message* massage){

    int copied;
    switch(massage->header.messege_id){
        case HEARTBEAT_ID:


    }
}


