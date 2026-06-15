# Smart Irrigation System using ESP32

## Project Overview

This project is an IoT-based Smart Irrigation System developed using ESP32. The system monitors soil moisture, rainfall, temperature, and humidity conditions in real time and automatically controls irrigation based on field conditions and weather forecasts obtained from the OpenWeather API.

A built-in web dashboard allows users to monitor sensor data, weather information, pump status, and operating mode (Auto/Manual) from any device connected to the same Wi-Fi network.

---

## Features

- Real-time soil moisture monitoring
- Rain detection using rain sensor
- Temperature and humidity monitoring using DHT11
- Weather forecast integration using OpenWeather API
- Automatic pump control based on sensor and weather data
- Manual pump control through web dashboard
- Auto and Manual operation modes
- Real-time web-based monitoring dashboard
- Wi-Fi connectivity status monitoring

---

## Hardware Components

- ESP32 Development Board
- DHT11 Temperature & Humidity Sensor
- Soil Moisture Sensor
- Rain Sensor Module
- Relay Module
- Water Pump
- Jumper Wires
- Power Supply

---

## Software and Technologies Used

- Arduino IDE
- ESP32 Wi-Fi Library
- WebServer Library
- HTTPClient Library
- ArduinoJson Library
- OpenWeather API
- HTML/CSS
- IoT

---

## System Workflow

1. ESP32 reads soil moisture, rain sensor, temperature, and humidity values.
2. Weather forecast data is fetched from OpenWeather API.
3. The system determines whether irrigation is required.
4. In Auto Mode:
   - Pump turns ON when soil is dry.
   - Pump remains OFF if rainfall is detected or rain is predicted.
5. In Manual Mode:
   - User can control the pump through the web dashboard.
6. Sensor readings and system status are displayed on the dashboard.

---

## Dashboard Features

The web dashboard displays:

- Weather Temperature
- Weather Humidity
- Rain Prediction Status
- Soil Condition (Dry/Wet)
- Current Rain Detection Status
- Field Temperature
- Pump Status (ON/OFF)
- Operating Mode (AUTO/MANUAL)
- Wi-Fi Connectivity Status
- API Connection Status

---

## Future Improvements

- Mobile application integration
- Cloud-based data logging
- SMS/Email notifications
- Machine Learning-based irrigation prediction
- Solar-powered deployment

---

## Author

Rajesh Anand M

Electronics and Communication Engineering (ECE)

---

## License

This project is developed for educational and academic purposes.
