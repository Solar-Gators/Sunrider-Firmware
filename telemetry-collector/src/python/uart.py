import serial, os, socket, json, time

ser = None
sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP

while not ser:
    try:
        ser = serial.Serial(os.environ.get("UART_PORT"))
    except:
        print("Cannot connect to serial port, trying again in 250ms")
        time.sleep(.25)


INTERNAL_UDP_PORT = os.environ.get("INTERNAL_UDP_PORT")
INTERNAL_UDP_HOST = os.environ.get("INTERNAL_UDP_HOST")
EXTERNAL_UDP_PORT = os.environ.get("EXTERNAL_UDP_PORT")
EXTERNAL_UDP_HOST = os.environ.get("EXTERNAL_UDP_HOST")

def runs(path, body):
    data = json.dumps({
        "model": path,
        **body,
    }).encode()
    sock.sendto(data, (INTERNAL_UDP_HOST, int(INTERNAL_UDP_PORT)))
    # Try to send to external server, but if it fails then ignore it
    try:
        sock.sendto(data, (EXTERNAL_UDP_HOST, int(EXTERNAL_UDP_PORT)))
    except socket.error:
        pass

def wait_for_transmission():
    global ser
    d = ser.read(1)
    return int.from_bytes(d, "little")
