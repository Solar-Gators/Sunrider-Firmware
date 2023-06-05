import os, pytest
from . import lib


@pytest.mark.timeout(4)
def test_start_condition_error():
    """
    The first byte received should always be a start byte, so if anything else is received then ignore it.
    """
    collector, master = lib.default_collector()

    # Send a none start condition
    os.write(master, b'\x12')

    assert collector.stdout.readline() == "Error: There must be a start condition before a transmission can occur\n"

    collector.terminate()

@pytest.mark.timeout(4)
def test_end_without_start():
    """
        The first byte received should always be a start byte, so if it's an end byte ignore it.
    """
    # Start collector
    collector, master = lib.default_collector()

    os.write(master, b'\x3F')

    assert collector.stdout.readline() == "Error: There was an end condition without a start\n"

    collector.terminate()

@pytest.mark.timeout(4)
def test_multiple_start_conditions():
    """
    If there are multiple start conditions then ignore it.
    """
    # Start collector
    collector, master = lib.default_collector()

    os.write(master, b'\xFF')
    os.write(master, b'\xFF')

    assert collector.stdout.readline() == "Error: There was multiple start conditions\n"

    collector.terminate()
