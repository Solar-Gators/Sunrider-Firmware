###
# Name: Telemetry Check
# Description: This test simply checks to see if we are sending telemetry
###
import serial, sys

ser = serial.Serial("/tmp/unit-uart")

START_BYTE = b'\xFF'
END_BYTE = b'\x3F'
ESCAPE_BYTE = b'\x2F'

BMS_RX0_MSG_ID = 0x6B0
BMS_RX1_MSG_ID = 0x6B1
BMS_RX2_MSG_ID = 0x6B2
BMS_RX3_MSG_ID = 0x6B3
BMS_RX4_MSG_ID = 0x6B4
BMS_RX5_MSG_ID = 0x6B5

MOTORRX0_RL_MSG_ID = 0x08850225
MOTORRX1_RL_MSG_ID = 0x08950225

GPS_RX0_MSG_ID = 0x565

can_id_recved = []
can_ids = [BMS_RX0_MSG_ID, BMS_RX1_MSG_ID, BMS_RX2_MSG_ID, BMS_RX3_MSG_ID, BMS_RX4_MSG_ID, GPS_RX0_MSG_ID, ]

msg_in_progress = False
escaping = False
bytes_read = bytearray()

while(1):
    data = ser.read(1)
    if not msg_in_progress and data == START_BYTE:
        msg_in_progress = True
    elif msg_in_progress:
        bytes_read.append(ord(data))

        if not escaping and data == ESCAPE_BYTE:
            escaping = True
        else:
            escaping = False

        if not escaping and data == END_BYTE:
            msg_in_progress = False
            can_id = int.from_bytes(bytes_read[:4], byteorder='big', signed=False)
            can_id_recved.append(can_id)
            bytes_read = bytearray()

            # If all can messages were sent
            if all(e in can_id_recved for e in can_ids):
                sys.stdout.write("All can messages received")
                break
