*** Settings ***
Suite Setup     Setup
Suite Teardown  Teardown
Test Teardown   Test Teardown
Test Setup      Test Setup
Resource        ${RENODEKEYWORDS}
Library         OperatingSystem

*** Keywords ***
Test Setup
    Execute Command     logLevel 1
    Execute Command     mach add "telemetry"
    Execute Command     machine LoadPlatformDescription @platforms/cpus/stm32f072.repl
    Execute Command     sysbus LoadELF @${CURDIR}/../Debug/OpenTelemetry.elf

*** Test Cases ***
Verify For Telemetry Messages
    [Documentation]     Verifies that telemetry messages are being sent

    Execute Command     emulation CreateUartPtyTerminal "term" "/tmp/unit-uart" true
    Execute Command     connector Connect sysbus.usart2 term

    Start Emulation

    ${result}=  Run Process  python3  ${CURDIR}/telemetry_check.py  timeout=20s
    Should Be Equal   ${result.stderr}    ${EMPTY}
    Should Contain   ${result.stdout}    All can messages received

Verify That Service Receives Data
    [Documentation]     Verifies that the pit-gui service received the telemetry

    Execute Command     emulation CreateUartPtyTerminal "term" "/tmp/uart" true
    Execute Command     connector Connect sysbus.usart2 term

    Start Emulation

    ${result}=  Run Process  python3  ${CURDIR}/telemetry_integration.py  timeout=20s
    Should Be Equal   ${result.stderr}    ${EMPTY}
    Should Contain   ${result.stdout}    All can messages received


# Verify GPS Telemetry
#     [Documentation]     Verifies GPS telemetry

#     Execute Command     emulation CreateUartPtyTerminal "gps_term" "/tmp/uart-gps" true
#     Execute Command     emulation CreateUartPtyTerminal "rf_term" "/tmp/uart" true
#     Execute Command     connector Connect sysbus.usart2 rf_term
#     Execute Command     connector Connect sysbus.usart4 gps_term

#     Start Emulation

#     ${result}=  Run Process  python3  ${CURDIR}/telemetry_gps.py  timeout=20s
#     Log To Console   ${result.stdout}
#     Should Be Equal  ${result.stderr}    ${EMPTY}
#     Should Contain   ${result.stdout}    Passed
