*** Settings ***
Suite Setup     Setup
Suite Teardown  Teardown
Test Teardown   Test Teardown
Resource        ${RENODEKEYWORDS}

*** Keywords ***
Start Test
    Execute Command     logLevel 1
    Execute Command     mach add "telemetry"
    Execute Command     machine LoadPlatformDescription @platforms/cpus/stm32f072.repl
    Execute Command     sysbus LoadELF @${CURDIR}/../Debug/OpenTelemetry.elf
    Start Emulation


*** Test Cases ***
Check For Telemetry Messages
    [Documentation]             Checks to see that telemetry messages are being sent

    Start Test

    Execute Command   emulation CreateUartPtyTerminal "term" "/tmp/uart" true
    Execute Command   connector Connect sysbus.usart2 term
    ${result}=  Run Process  python3  ${CURDIR}/telemetry_check.py  timeout=2min
    Should Contain   ${result.stderr}    ${EMPTY}
    Should Contain   ${result.stdout}    All can messages received
