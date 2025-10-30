from pymavlink.dialects.v10 import common as mavlink_v1

def process_file(input_path, output_path):
    # Read the input file and process
    with open(input_path, 'r', encoding='utf-8') as infile:
        # Remove the first 25 chars from each line and remove spaces
        data = ''.join(line[25:].strip().replace(' ', '') for line in infile)
    
    # Split into lines of 32 characters
    lines = [data[i:i+32] for i in range(0, len(data), 32)]
    
    # Write to the output file
    with open(output_path, 'w', encoding='utf-8') as outfile:
        outfile.write('\n'.join(lines))

# Example usage:
# process_file('input.txt', 'output.txt')


def hexfile_to_binfile(input_path, output_path):
    with open(input_path, 'r', encoding='utf-8') as infile:
        # Read all lines and remove whitespace/newlines
        hex_data = ''.join(line.strip() for line in infile)

    # Convert hex string to bytes
    binary_data = bytes.fromhex(hex_data)

    # Write to binary file
    with open(output_path, 'wb') as outfile:
        outfile.write(binary_data)

# Example usage:
# hexfile_to_binfile('input.txt', 'output.bin')

def parse_mavlink(input_path):
    type_list = []
    messege_list = []
    parser = mavlink_v1.MAVLink(None)
    with open(input_path, 'r', encoding='utf-8') as infile:
        # Read all lines and remove whitespace/newlines
        hex_data = ''.join(line.strip() for line in infile)
    
    msg_bytes = bytes.fromhex(hex_data)
    for index, b in enumerate(msg_bytes):
        try:
            msg = parser.parse_char(bytes([b]))  # wrap each byte as bytes object
            if msg:  # parse_char returns a MAVLink_message when a full frame is parsed
                #print("Valid MAVLink message:", msg.get_type(), msg.to_dict())
                type_list.append(msg.get_type())
                messege_list.append(msg.to_dict())
        except Exception as e:
            parser = mavlink_v1.MAVLink(None)
    
    return type_list, messege_list


def parse_corupted_mavlink(input_path):
    parser = mavlink_v1.MAVLink(None)
    with open(input_path, 'r', encoding='utf-8') as infile:
        # Read all lines and remove whitespace/newlines
        hex_data = ''.join(line.strip() for line in infile)
    
    msg_bytes = bytes.fromhex(hex_data)

    potential_starts = [i for i, b in enumerate(msg_bytes) if b==254]

    for start in potential_starts:
        rel_bytes = msg_bytes[start:]
        failed = False
        type_list = []
        messege_list = []
        for index, b in enumerate(rel_bytes):
            try:
                msg = parser.parse_char(bytes([b]))  # wrap each byte as bytes object
                if msg:  # parse_char returns a MAVLink_message when a full frame is parsed
                    #print("Valid MAVLink message:", msg.get_type(), msg.to_dict())
                    type_list.append(msg.get_type())
                    messege_list.append(msg.to_dict())
            except Exception as e:
                failed = True
                parser = mavlink_v1.MAVLink(None)
                break
        if not failed:
            return type_list, messege_list
        
    return [], []
    
    




if __name__ == "__main__":
    #process_file("few_messeges.txt", "no_methadata.txt")
    #process_file("corupted_start.txt", "no_methadata.txt")
    #process_file("with_methadata.txt", "no_methadata.txt")
    #process_file("badly_corupted_start.txt", "no_methadata.txt")
    process_file("long_corrupted_file.txt", "no_methadata.txt")
    parser = mavlink_v1.MAVLink(None)
    hexfile_to_binfile("no_methadata.txt", "mavlink.bin")
   
    type_list, messege_list = parse_corupted_mavlink("no_methadata.txt")
    #type_list, messege_list = parse_corupted_mavlink("recordings_from_company/manually_edited_clean_hexa_dump.txt")
    type_set = set(type_list)
    pass
