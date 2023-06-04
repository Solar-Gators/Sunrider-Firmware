# Name: Telemetry Check
# Description: This test checks that data is being sent to the service
###

import requests, sys

BASE_URL = "http://localhost:9000"

to_check = {
    "bms": ["rx1", "rx2", "rx3", "rx4", "rx5"],
    "mitsuba": ["rx1", "rx2"],
    "mppt": ["rx1", "rx5"],
    "gps": ["rx0"]
}

initial_state = {}

# get the initial counts
for telemetry_type in to_check:
    for msg in to_check[telemetry_type]:

        initial_state.setdefault(telemetry_type, {})
        initial_state[telemetry_type][msg] = requests.get(
            BASE_URL + f"/api/{telemetry_type}/{msg}/cnt"
        ).json()

for telemetry_type in to_check:
    for msg in to_check[telemetry_type]:
        # wait for data to come over
        while requests.get(BASE_URL + f"/api/{telemetry_type}/{msg}/cnt").json() == initial_state[telemetry_type][msg]:
            ...

sys.stdout.write("All can messages received")
