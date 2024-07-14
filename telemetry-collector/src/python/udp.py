import os, socket, json

INTERNAL_UDP_PORT = os.environ.get("INTERNAL_UDP_PORT", "8000")
INTERNAL_UDP_HOST = os.environ.get("INTERNAL_UDP_HOST", "localhost")
EXTERNAL_UDP_PORT = os.environ.get("EXTERNAL_UDP_PORT", "8000")
EXTERNAL_UDP_HOST = os.environ.get("EXTERNAL_UDP_HOST", "api.ufsolargators.org")

sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP

def send_tlm(data):
    data = json.dumps(data).encode()
    sock.sendto(data, (INTERNAL_UDP_HOST, int(INTERNAL_UDP_PORT)))
    # Try to send to external server, but if it fails then ignore it
    try:
        sock.sendto(data, (EXTERNAL_UDP_HOST, int(EXTERNAL_UDP_PORT)))
    except socket.error:
        pass
