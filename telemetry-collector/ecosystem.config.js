const commonConfig = {
  "INTERNAL_UDP_HOST": "localhost",
  "INTERNAL_UDP_PORT": "8000",
  "EXTERNAL_UDP_PORT": "8000",
  "UART_BAUD_RATE": "3000000",
}

module.exports = {
    apps : [
        {
          name: "telemetry_collector",
          script: "./collector",
          env: {
            "EXTERNAL_UDP_HOST": "api.ufsolargators.org",
            "UART_PORT": "/dev/ttyACM0",
            ...commonConfig,
          },
          env_ci: {
            ...commonConfig,
            "EXTERNAL_UDP_HOST": "ufsolargators.org",
            "UART_PORT": "/tmp/uart",
          }
        },
        {
          name: "pi_health",
          script: "python src/python/is_alive.py"
        }
    ]
}
