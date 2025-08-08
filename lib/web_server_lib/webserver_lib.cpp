#include "web_server_lib.h"
#include "web_server_config.h"
#include "local_storage_lib.h"
// Create a DNS server object
DNSServer dnsServer;
// Create an AsyncWebServer object on port 80
AsyncWebServer server(AP_PORT);
// --- HTML & CSS for the Web Page ---
// We store the HTML, CSS, and JavaScript in a raw string literal (R"rawliteral(...)rawliteral")
// This makes it easy to write and manage the web page content directly in the C++ code.
const char* HTML_CONTENT = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Configuration</title>
    <style>
        /* Import a modern font from Google Fonts */
        @import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;500;600&display=swap');

        /* CSS Reset and Basic Styling */
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Poppins', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            background: linear-gradient(45deg, #4e54c8, #8f94fb, #4facfe, #00f2fe);
            background-size: 400% 400%;
            animation: gradientBG 15s ease infinite;
            color: #333;
            overflow-x: hidden;
        }

        /* Keyframes for the animated gradient background */
        @keyframes gradientBG {
            0% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
            100% { background-position: 0% 50%; }
        }

        /* Main container for the form */
        .container {
            background: rgba(255, 255, 255, 0.9);
            padding: 2.5rem 3rem;
            border-radius: 20px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
            width: 90%;
            max-width: 450px;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.3);
            animation: fadeIn 1s ease-out;
        }

        /* Fade-in animation for the container */
        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        /* Form header styling */
        h1 {
            text-align: center;
            margin-bottom: 1.5rem;
            color: #4e54c8;
            font-weight: 600;
        }

        /* Styling for each input group (label + input) */
        .input-group {
            position: relative;
            margin-bottom: 2rem;
        }

        /* Input field styling */
        .input-field {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid #ddd;
            border-radius: 10px;
            background-color: #f9f9f9;
            font-size: 1rem;
            color: #555;
            transition: border-color 0.3s, box-shadow 0.3s;
        }

        .input-field:focus {
            outline: none;
            border-color: #8f94fb;
            box-shadow: 0 0 8px rgba(143, 148, 251, 0.5);
        }

        /* Label styling */
        .input-label {
            position: absolute;
            left: 15px;
            top: 13px;
            color: #999;
            pointer-events: none;
            transition: all 0.3s ease;
            background: #f9f9f9;
            padding: 0 5px;
        }

        /* Animation for the label when the input is focused or has content */
        .input-field:focus + .input-label,
        .input-field:not(:placeholder-shown) + .input-label {
            top: -10px;
            left: 10px;
            font-size: 0.8rem;
            color: #4e54c8;
        }

        /* Submit button styling */
        .submit-btn {
            width: 100%;
            padding: 15px;
            border: none;
            border-radius: 10px;
            background: linear-gradient(45deg, #4e54c8, #8f94fb);
            color: white;
            font-size: 1.1rem;
            font-weight: 500;
            cursor: pointer;
            transition: transform 0.2s, box-shadow 0.2s;
        }

        .submit-btn:hover {
            transform: translateY(-3px);
            box-shadow: 0 8px 20px rgba(78, 84, 200, 0.4);
        }

        /* Styling for the success message box */
        #message-box {
            text-align: center;
            padding: 15px;
            margin-top: 1.5rem;
            border-radius: 10px;
            font-weight: 500;
            display: none; /* Hidden by default */
        }
        #message-box.success {
            background-color: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ALEEF tracker Setup</h1>
        <form action="/save" method="GET" id="config-form">
            <!-- WiFi SSID Input -->
            <div class="input-group">
                <input type="text" id="ssid" name="ssid" class="input-field" placeholder=" " required>
                <label for="ssid" class="input-label">WiFi Network Name (SSID)</label>
            </div>
            <!-- WiFi Password Input -->
            <div class="input-group">
                <input type="password" id="pass" name="pass" class="input-field" placeholder=" ">
                <label for="pass" class="input-label">WiFi Password</label>
            </div>
            <!-- APN Input -->
            <div class="input-group">
                <input type="text" id="apn" name="apn" class="input-field" placeholder=" " required>
                <label for="apn" class="input-label">APN</label>
            </div>
            <!-- Bluetooth Name Input -->
            <div class="input-group">
                <input type="text" id="btname" name="btname" class="input-field" placeholder=" " required>
                <label for="btname" class="input-label">Bluetooth Name</label>
            </div>
            <!-- Dog Name Input -->
            <div class="input-group">
                <input type="text" id="dogname" name="dogname" class="input-field" placeholder=" " required>
                <label for="dogname" class="input-label">Pet's Name</label>
            </div>
            <!-- Submit Button -->
            <button type="submit" class="submit-btn">Save Settings</button>
        </form>
        <!-- Message box for success notification -->
        <div id="message-box"></div>
    </div>

    <script>
        // JavaScript to show a success message without reloading the page
        document.getElementById('config-form').addEventListener('submit', function(e) {
            e.preventDefault(); // Prevent the default form submission
            
            const formData = new FormData(this);
            const query = new URLSearchParams(formData).toString();
            
            // Send form data in the background
            fetch('/save?' + query)
                .then(response => response.text())
                .then(text => {
                    const messageBox = document.getElementById('message-box');
                    messageBox.textContent = text;
                    messageBox.className = 'success'; // Apply success styling
                    messageBox.style.display = 'block'; // Make it visible
                    
                    // Hide the message after 5 seconds
                    setTimeout(() => {
                        messageBox.style.display = 'none';
                    }, 5000);
                })
                .catch(error => console.error('Error:', error));
        });
    </script>
</body>
</html>
)rawliteral";

void startWebServer() {
    // Start the Wi-Fi Access Point
    WiFi.softAP(AP_SSID + String(random(0xffff), HEX), AP_PASSWORD);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    // Print the IP address of the Access Point
    IPAddress myIP = WiFi.softAPIP();
    // --- Start DNS Server for Captive Portal ---
    // The '*' redirects all domain name requests to the ESP32's IP address.
    dnsServer.start(53, "*", myIP);
    Serial.println("DNS server started.");
    // --- Define Server Request Handlers ---
    // Route for root ("/")
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", HTML_CONTENT);
    });
      // Route to handle form submission ("save")
    server.on("/save", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("\n--- New Settings Received ---");
        
        // Retrieve form parameters
        String ssid = request->hasParam("ssid") ? request->getParam("ssid")->value() : "";
        String pass = request->hasParam("pass") ? request->getParam("pass")->value() : "";
        String apn = request->hasParam("apn") ? request->getParam("apn")->value() : "";
        String btname = request->hasParam("btname") ? request->getParam("btname")->value() : "";
        String dogname = request->hasParam("dogname") ? request->getParam("dogname")->value() : "";

        // Print the received values to the Serial Monitor
        Serial.print("WiFi SSID: ");
        Serial.println(ssid);
        Serial.print("WiFi Password: ");
        Serial.println(pass);
        Serial.print("APN: ");
        Serial.println(apn);
        Serial.print("Bluetooth Name: ");
        Serial.println(btname);
        Serial.print("Dog's Name: ");
        Serial.println(dogname);
        Serial.println("-----------------------------\n");
        // Send a success message back to the browser
        request->send(200, "text/plain", "Settings saved successfully!");
        saveString(SSID_KEY, ssid);
        saveString(WIFI_PASSWORD_KEY, pass);
        saveString(GSM_APN_KEY, apn);
        saveString(BLUETOOTH_NAME_KEY, btname);
        saveString(PET_NAME_KEY, dogname);
        // Restart the device to apply new settings
        Serial.println("Restarting device to apply new settings...");
        delay(1000);
        ESP.restart();
    });

    // Captive Portal: When a device tries to access any other URL,
    // it will be redirected to the main configuration page.
    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(200, "text/html", HTML_CONTENT);
    });

  // Start the server
  server.begin();
  Serial.println("HTTP server started. Ready for connections.");
}

void waitUntillSetupDevice() {
    // Check for client connections and handle DNS requests
    dnsServer.processNextRequest();
}