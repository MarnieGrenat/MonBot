# MonBot - Intelligent Home Safety System
### Author: Gabriela Dellamora
### Version: 1.0.9
## Overview
MonBot is an innovative open-source project designed for personal use, with a primary focus on democratizing access to safety and quality of life. This intelligent home safety system incorporates advanced sensors, including the ESP32, DHT22, and MQ2, to monitor gas levels and generate alerts via WhatsApp through a personal-use-only API. The open-source nature of MonBot encourages collaboration and community involvement for continuous improvement and customization to individual needs.

## Key Features
**Gas Monitoring**: MonBot actively monitors gas levels within the home environment, utilizing the ESP32 and MQ2 sensor to provide real-time insights for enhanced safety measures.

**Fire Prevention**: With a primary focus on preventing residential fires, MonBot employs cutting-edge technology to detect potential hazards and promptly alert users via WhatsApp.

## Why MonBot?
In a world where safety is paramount, MonBot stands out as a reliable and user-friendly solution. By combining advanced sensors and intelligent algorithms, it empowers individuals to take proactive steps towards ensuring the safety of their homes.

## How it Works
### Hardware Requirements:
- **ESP32**: MonBot utilizes the ESP32 microcontroller for efficient data processing and communication.
- **DHT22**: The DHT22 sensor is employed to measure temperature and humidity levels within the home.
- **MQ2**: MonBot incorporates the MQ2 gas sensor to detect and monitor gas levels.
## Installation Guide:

### Hardware Connection:
- Connect the ESP32 to the DHT22 and MQ2 sensors using appropriate wiring. Refer to the datasheets or documentation for pin configurations.
- Ensure a stable power supply for the ESP32 and sensors.

### Upload Code:
- Open the MonBot Arduino sketch in the Arduino IDE.
- Select the ESP32 board and the appropriate COM port.
- Upload the code to the ESP32.

### Device Configuration:
To configure MonBot for your specific needs, follow these steps:

**Access Point Setup:**

Power on MonBot and connect to its default Access Point (AP).
Access the AP by connecting your device to MonBot's Wi-Fi network.

**Connect to Local Wi-Fi:**

Within the AP interface, provide your local Wi-Fi credentials for MonBot to connect to your home network.

**Access Web Server:**

Once connected to the local Wi-Fi, access the MonBot web server by entering the ESP32 IP address in your web browser.

**Web Server Configuration:**

In the web interface, you can:
- Change the WhatsApp API key for message alerts.
- Update the phone number for receiving updates.
- Configure MQTT server settings for home automation.

**Save and Apply Changes:**

Save the configured settings through the web interface to apply changes to MonBot.

**Access via Local Network:**

After configuration, you can access MonBot through your local network by entering its assigned IP address in the browser.
  
### Testing:
- Power on the ESP32 with the connected sensors.
- Monitor the serial console for initialization messages.
- Introduce controlled changes in gas levels or environment to observe real-time alerts.
  
## License and Usage
MonBot is designed exclusively for personal use, and any commercial utilization is strictly prohibited. Selling or distributing MonBot for commercial purposes is not permitted without explicit authorization from the project's author, [Gabriela Dellamora](https://github.com/MarnieGrenat), and the WhatsApp API provider, [CallMeBot](https://www.callmebot.com/blog/free-api-whatsapp-messages/).

## Future Enhancements
As part of our commitment to continuous improvement, future releases of MonBot will include additional features.

## Support and Contributions
We welcome contributions from the community to enhance the capabilities of MonBot. If you encounter any issues or have suggestions for improvements, please visit our [GitHub repository](https://github.com/MarnieGrenat/MonBot) and create an issue or pull request.

## Contact
For inquiries and further information, please contact the author, Gabriela Dellamora, at [gabriela.dellamora@gmail.com].
Thank you for choosing MonBot - Your Partner in Home Safety.
