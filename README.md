# esp8266-thermal-printing
# Thermal Printer Integration with ESP8266

This project provides a simple implementation to send POST requests for thermal printing using ESC/POS commands via an ESP8266 microcontroller. It supports basic text printing but does not support Chinese characters.

## Features
- Receive JSON data via HTTP POST requests.
- Extract printer IP and print data from the JSON payload.
- Send print commands to a thermal printer using ESC/POS commands.
- Supports basic text printing, line feeds, and paper cutting.
- Automatically reconnects to Wi-Fi if the connection is lost.

## Limitations
- **Does not support Chinese character printing.**

---

## Hardware Requirements
- ESP8266 microcontroller.
- Thermal printer with ESC/POS compatibility.
- Wi-Fi network.

## Software Requirements
- Arduino IDE.
- Libraries:
  - `ESP8266WiFi.h`
  - `ESP8266WebServer.h`
  - `ArduinoJson.h`

---

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/your-repo/esp8266-thermal-printer.git
   ```

2. Open the project in the Arduino IDE.

3. Install the required libraries via the Arduino Library Manager.

4. Update the following variables in the code with your Wi-Fi credentials:
   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_PASSWORD";
   ```

5. Upload the code to your ESP8266 device.

---

## API Usage

### Endpoint
- **POST** `/postdata`

### JSON Payload Format
```json
{
  "printerIP": "<PRINTER_IP_ADDRESS>",
  "printData": "<TEXT_TO_PRINT>"
}
```

### Example Request
Use any HTTP client (e.g., Postman, cURL, or your custom app) to send a POST request:

#### cURL Example
```bash
curl -X POST http://<ESP8266_IP>/postdata \
  -H "Content-Type: application/json" \
  -d '{
    "printerIP": "192.168.1.100",
    "printData": "Hello, this is a test print!\nThank you.\n"
  }'
```

### Response
- **200 OK**: Request received successfully.
- **400 Bad Request**: Invalid JSON or missing data.

---

## How It Works

1. The ESP8266 connects to the configured Wi-Fi network and sets up a web server on port `80`.
2. When a POST request is sent to `/postdata`, the ESP8266 extracts the `printerIP` and `printData` fields from the JSON payload.
3. The ESP8266 establishes a TCP connection to the printer on port `9100` and sends the data using ESC/POS commands.
4. The printer prints the received text and performs actions such as feeding and cutting paper.

---

## Known Issues
- The code does not support Chinese or other multi-byte characters.
- Ensure the `printerIP` is within the same network as the ESP8266.

---

## Contribution
Feel free to contribute by submitting issues, feature requests, or pull requests.

---

## License
This project is licensed under the MIT License. See the LICENSE file for details.

