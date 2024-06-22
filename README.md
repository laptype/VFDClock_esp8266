# VFDClock_esp8266🚀

Most existing ESP8266 VFD clock projects rely on connecting to a WiFi network to automatically update the time and often lack control features for the VFD display, such as switching and brightness adjustment. Since my workspace doesn't have direct access to an available WiFi network, I implemented a solution where the ESP8266 creates an AP (Access Point) that connects to a phone, automatically updates the ESP8266's time, and adds control features for the VFD clock display.

<img src="img/README_img/timer.gif" alt="timer" style="zoom: 50%;" />

## Project Features 🎛️

1. **No External WiFi Required**:
   - 📶 The ESP8266 operates as a WiFi hotspot, not requiring an external WiFi network.
   - 📱 Users connect to the ESP8266's AP using their phone or other devices.

2. **Automatic Time Update ⏰**:
   - 🔄 After connecting to the ESP8266's AP, users access the device via a browser to set the current time, which the ESP8266 then uses to update its system time.

3. **VFD Display Control 💡**:
   - **On/Off Control**: Users can turn the VFD display on or off via a web interface.
   - **Brightness Adjustment**: A slider allows users to adjust the VFD display brightness in real-time via the web interface.
   - **Scrolling Update Animation**: Implements a scrolling display of the time for enhanced visual effects.
   - **Font Switching**: Users can switch between two different fonts for diverse and personalized displays.

4. **Low Power Mode 💤**:
   - 🔋 When no device is connected to the ESP8266's WiFi network, the device automatically enters light sleep mode to save power.
   - 🔌 The device only handles client requests when not in light sleep mode, ensuring efficient power management.

5. **User IP Address Recognition**:
   - 🌐 Each user generates a unique token based on their device's IP address to individually determine their login status.
   - 🔑 Once logged in, users don't need to log in again for a short period, enhancing the user experience.

## Technical Implementation 🛠️

1. **ESP8266 as an AP**:
   - 📡 The ESP8266 is configured as an open WiFi hotspot that users can connect to without a password.
   - 🔗 Uses the `WiFi.softAP` function to set up AP mode and `DNSServer` to redirect all domain requests to the ESP8266's IP address.

2. **Web Control Interface 🌐**:
   - 📄 Built-in simple HTML/CSS/JavaScript pages served by `ESP8266WebServer`.
   - 🔐 The web interface includes a login page and a main control page. Users enter a password on the login page and are granted access to the main control page upon successful authentication.
   - 🎛️ The main control page includes buttons to turn the VFD display on/off, a brightness adjustment slider, a font switch button, and a button to set the current time.

3. **Time Synchronization 📅**:
   - ⏳ Users set the current time, which is sent to the ESP8266 via JavaScript. The ESP8266 parses the time and updates its system time accordingly.

4. **Security 🔒**:
   - 🔑 A simple login mechanism ensures that only authorized users can access the control interface, maintaining device security.

5. **Scrolling Time Display Animation 🎞️**:
   - 🌀 Implements scrolling time updates for enhanced visual effects and user experience.

6. **Low Power Mode Management**:
   - 🌙 The device automatically enters light sleep mode when no devices are connected.
   - 🛌 In light sleep mode, power consumption is reduced, and the device only handles necessary network requests upon waking.

7. **Font Switching**:
   - ✒️ Users can switch between two different font styles via the web interface, adding variety to the display.

8. **User IP Address Recognition**:
   - 🌐 The `server.client().remoteIP().toString()` method retrieves the user's IP address and generates a unique token based on it.
   - 🛡️ Each user is identified by their IP address token, which records their login status, eliminating the need for repeated logins in a short period.
   - When no devices are connected to the ESP8266's WiFi network, the device resets all user login statuses.

## Usage Instructions 📋

1. Power on the ESP8266 device, which will operate as a WiFi hotspot.
2. Connect your phone or other device to the WiFi network named `Loading...🚀`.
3. Open your browser and navigate to any URL; you will be automatically redirected to the device's login page.
4. Enter the correct password to access the main control interface.
5. In the control interface, users can:
   - Turn the VFD display on or off using the button.
   - Adjust the VFD display brightness using the slider.
   - Set the current time by clicking the button to ensure accurate time display on the VFD clock.
6. The device will automatically enter light sleep mode to save power when no devices are connected. It will wake up and handle requests when a device connects.

# Login Page

<img src="img/README_img/image-20240615231837256.png" alt="login page" style="zoom:67%;" />

# Font Switching

Bold font display:

<img src="img/README_img/image-20240617172340668.png" alt="bold font" style="zoom:50%;" />

Font design:

<img src="img/README_img/image-20240617172558478.png" alt="font design" style="zoom:50%;" />

![font design example](img/README/e3f7060a5dccdef12487e0c1453e141.jpg)

# Scrolling Animation

![scrolling animation](img/README/demo.gif)
