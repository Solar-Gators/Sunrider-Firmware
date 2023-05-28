import uart

while 1:
    print(
        uart.wait_for_transmission()
    )
