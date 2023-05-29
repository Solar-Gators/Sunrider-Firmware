module.exports = {
    apps : [
        {
          name: "telemetry_collector",
          script: "./collector",
          env: {
            "INTERNAL_UDP_HOST": "localhost",
            "INTERNAL_UDP_PORT": "8000",
            "EXTERNAL_UDP_HOST": "api.ufsolargators.org",
            "EXTERNAL_UDP_PORT": "8000",

            "UART_BAUD_RATE": "57600",
            "UART_PORT": "/tmp/uart",
          }
        }
    ]
}
