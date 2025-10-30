import os
from pymavlink import mavutil

# Get absolute path of the file next to this script
script_dir = os.path.dirname(os.path.abspath(__file__))
log_path = os.path.join(script_dir, "mavlink.bin")

print("Reading:", log_path)

mav = mavutil.mavlink_connection(log_path,  dialect="common", robust_parsing=True)

while True:
    msg = mav.recv_match(blocking=False)
    if msg is None:
        break  # End of file
    print(msg.get_type(), msg.to_dict())
    pass
