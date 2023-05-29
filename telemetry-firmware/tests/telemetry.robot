*** Settings ***
Test Teardown         Test Teardown
Test Setup            Test Setup
Resource        ${RENODEKEYWORDS}
Library         OperatingSystem

*** Keywords ***
Test Setup
    Execute Command     logLevel 1
    Execute Command     mach add "telemetry"
    Execute Command     machine LoadPlatformDescription @platforms/cpus/stm32f072.repl
    Execute Command     sysbus LoadELF @${CURDIR}/../Debug/OpenTelemetry.elf

Test Teardown
    Reset Emulation


*** Test Cases ***
Check For Telemetry Messages
    [Documentation]             Checks to see that telemetry messages are being sent

    Execute Command     emulation CreateUartPtyTerminal "term" "/tmp/unit-uart" true
    Execute Command     connector Connect sysbus.usart2 term

    Start Emulation

    ${result}=  Run Process  python3  ${CURDIR}/telemetry_check.py  timeout=2min
    Should Contain   ${result.stderr}    ${EMPTY}
    Should Contain   ${result.stdout}    All can messages received

Check That Service Receives Data

    Execute Command     emulation CreateUartPtyTerminal "term" "/tmp/uart" true
    Execute Command     connector Connect sysbus.usart2 term

    Start Emulation

    ${result}=  Run Process  python3  ${CURDIR}/telemetry_integration.py  timeout=2min
    Should Contain   ${result.stderr}    ${EMPTY}
    Should Contain   ${result.stdout}    All can messages received
