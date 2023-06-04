###
# Name: Telemetry GPS
# Description: This test verifies that GPS telemetry gets correctly parsed
###
import serial, sys, requests
gps_ser = serial.Serial("/tmp/uart-gps")

gps_ser.write(b"$GPGGA, 130113.00, 37XX.XXXX,N, 07XXX.XXXX, E,1,04,3.97,404.9,M,45.7,M,,*79")


while 1:
    response = requests.get("http://localhost:3000/api/live/data")
    response.raise_for_status()

    data = response.json()

    if data["gps"] is not None:
        break


sys.stdout.write("Passed")
