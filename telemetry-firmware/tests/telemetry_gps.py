###
# Name: Telemetry GPS
# Description: This test verifies that GPS telemetry gets correctly parsed
###
# TODO: get this working

import serial, sys, requests, time
gps_ser = serial.Serial("/tmp/uart-gps", 38400)

gps_ser.write(b"$GPRMC,123519,A,4807.0380,N,01131.0000,E,022.4,084.4,230394,003.1,W*6A")


while 1:
    time.sleep(0.1)
    response = requests.get("http://localhost:9000/api/live/data")
    response.raise_for_status()
    data = response.json()

    if data["gps"] is None:
        continue
    print(data["gps"]["latitude"])
    if data["gps"]["latitude"] == "48.117300":
        break


sys.stdout.write("Passed")
