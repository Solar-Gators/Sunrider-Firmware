import pty, subprocess, os

def fake_port():
    """
        Open a fake virtual port for serial mocking
    """
    master, slave = pty.openpty()
    s_name = os.ttyname(slave)

    return s_name, master, slave

def start_collector(port, service_url):
    """
        Start the telemetry collector executable
    """
    my_env = {}
    my_env["UART_PORT"] = port
    my_env["UART_BAUD_RATE"] = '98000'
    my_env["GUI_SERVICE"] = service_url
    return subprocess.Popen(
        "./collector",
        env=my_env,
        stdout=subprocess.PIPE,
        universal_newlines=True
    )

def wait_for_uart(collector):
    """
        Wait for the collector to configure UART
    """
    assert collector.stdout.readline() == "Loaded uart\n"


def default_collector():
    """
        Create a default collector
    """
    s_name, master, slave = fake_port()

    collector = start_collector(s_name, "test")
    wait_for_uart(collector)

    return collector, master
