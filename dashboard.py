# Pet Health Monitor Dashboard (No SpO2)
# This application creates a web-based dashboard to monitor pet health data sent via MQTT.
# It uses the Dash framework for the UI and Paho-MQTT for communication.

# --- Installation ---
# Before running, install the required Python libraries:
# pip install dash dash-bootstrap-components dash-daq paho-mqtt pandas plotly

import dash
from dash import dcc, html, Input, Output, State
import dash_bootstrap_components as dbc
import dash_daq as daq
import plotly.graph_objects as go
import paho.mqtt.client as mqtt
import threading
import time
from collections import deque

# --- Global Variables & Configuration ---
# Dictionary to store the latest data from the pet tracker
pet_data = {
    "battery_percentage": 0,
    "latitude": 29.9792,  # Default to Giza, Egypt
    "longitude": 31.1342,
    "heart_rate": 0,
    "temperature": 0,
    "last_update": "Never"
}

# Thread-safe lock for accessing pet_data
data_lock = threading.Lock()

# Deques for historical data (for charts)
HEART_RATE_HISTORY = deque(maxlen=50)
TIME_HISTORY = deque(maxlen=50)

# Global variable to hold the MQTT client thread
mqtt_thread = None

# --- MQTT Client Handling ---
class MqttClientThread(threading.Thread):
    """A separate thread to handle MQTT communication non-blockingly."""
    def __init__(self, broker, port, username, password, pet_name):
        super().__init__()
        self.broker = broker
        self.port = int(port)
        self.username = username
        self.password = password
        self.pet_name = pet_name
        self.client = mqtt.Client(client_id=f"dashboard_{pet_name}", protocol=mqtt.MQTTv311)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.username_pw_set(self.username, self.password)
        self.stop_event = threading.Event()

    def on_connect(self, client, userdata, flags, rc):
        """Callback for when the client connects to the broker."""
        if rc == 0:
            print("Connected to MQTT Broker!")
            # Subscribe to all relevant topics
            topics = [
                f"{self.pet_name}/battery/precentage", # Typo matches ESP32 code
                f"{self.pet_name}/gps/latitude",
                f"{self.pet_name}/gps/longitude",
                f"{self.pet_name}/activity/heart_rate",
                f"{self.pet_name}/activity/temperature",
            ]
            for topic in topics:
                print(f"Subscribing to topic: {topic}")
                client.subscribe(topic)
        else:
            print(f"Failed to connect, return code {rc}\n")

    def on_message(self, client, userdata, msg):
        """Callback for when a message is received from the broker."""
        global pet_data, HEART_RATE_HISTORY, TIME_HISTORY
        try:
            payload = msg.payload.decode('utf-8')
            topic = msg.topic
            print(f"Received `{payload}` from `{topic}` topic")

            with data_lock:
                current_time = time.strftime('%H:%M:%S')
                pet_data["last_update"] = current_time

                # Update data based on topic
                if "battery/precentage" in topic:
                    pet_data["battery_percentage"] = float(payload)
                elif "gps/latitude" in topic:
                    pet_data["latitude"] = float(payload)
                elif "gps/longitude" in topic:
                    pet_data["longitude"] = float(payload)
                elif "activity/heart_rate" in topic:
                    hr_val = int(float(payload))
                    pet_data["heart_rate"] = hr_val
                    HEART_RATE_HISTORY.append(hr_val)
                    # Time is only added when primary data comes in
                    TIME_HISTORY.append(current_time)
                elif "activity/temperature" in topic:
                    pet_data["temperature"] = float(payload)

        except Exception as e:
            print(f"Error processing message: {e}")

    def run(self):
        """Start the MQTT client loop."""
        print("MQTT thread started")
        try:
            self.client.connect(self.broker, self.port, 60)
            self.client.loop_start()
            self.stop_event.wait() # Wait until stop is called
            self.client.loop_stop()
            self.client.disconnect()
            print("MQTT thread stopped")
        except Exception as e:
            print(f"MQTT connection error: {e}")

    def stop(self):
        """Stop the MQTT client."""
        self.stop_event.set()

    def publish(self, topic, payload):
        """Publish a message to the broker."""
        full_topic = f"{self.pet_name}/{topic}"
        print(f"Publishing `{payload}` to `{full_topic}`")
        self.client.publish(full_topic, payload, qos=1)


# --- Dash App Layout ---
app = dash.Dash(__name__, external_stylesheets=[dbc.themes.CYBORG, dbc.icons.FONT_AWESOME])
app.title = "Pet Health Monitor"

# Helper function to create styled info cards
def make_info_card(title, content_id, icon):
    return dbc.Card(
        dbc.CardBody([
            html.H4(title, className="card-title"),
            html.Div([
                html.I(className=f"{icon} fa-2x"),
                html.Div(id=content_id, className="ms-3 fs-3")
            ], className="d-flex align-items-center")
        ]),
        className="text-center m-2",
        style={"height": "150px"}
    )

# Main layout of the dashboard
app.layout = dbc.Container([
    # Store for holding the MQTT client instance (as a dictionary) and pet name
    dcc.Store(id='mqtt-client-store', data=None),
    dcc.Store(id='pet-name-store', data=None),

    # Header
    dbc.Row([
        dbc.Col(html.H1("🐾 Pet Health & Location Dashboard", className="text-center text-primary p-3"), width=12)
    ]),

    # Settings Toggle Button
    dbc.Row([
        dbc.Col(
            dbc.Button(
                [html.I(className="fas fa-cog me-2"), "Connection Settings"],
                id="collapse-settings-button",
                className="mb-3",
                color="info",
                outline=True
            ),
            className="d-flex justify-content-end"
        )
    ]),

    # Configuration Section (now collapsible)
    dbc.Collapse(
        dbc.Card(
            dbc.CardBody([
                dbc.Row([
                    dbc.Col(dbc.Input(id='mqtt-broker-input', placeholder='MQTT Broker Address (e.g., 4.tcp.ngrok.io:12345)', value='broker.hivemq.com'), md=3),
                    dbc.Col(dbc.Input(id='mqtt-username-input', placeholder='MQTT Username (optional)'), md=2),
                    dbc.Col(dbc.Input(id='mqtt-password-input', placeholder='MQTT Password (optional)', type='password'), md=2),
                    dbc.Col(dbc.Input(id='pet-name-input', placeholder='Pet Name (e.g., PET)', value='PET'), md=2),
                    dbc.Col(dbc.Button("Connect", id='connect-button', color='primary', className="w-100"), md=2),
                    dbc.Col(html.Div(id='connection-status', className="text-center mt-2"), md=1)
                ]),
            ]),
            className="mb-4"
        ),
        id="settings-collapse",
        is_open=False, # Start hidden
    ),

    # Live Data Section
    dbc.Row([
        # Column 1: Heart Rate Gauge
        dbc.Col([
            daq.Gauge(
                id='heart-rate-gauge',
                label="Heart Rate (BPM)",
                value=0,
                min=40, max=220,
                color={"gradient":True,"ranges":{"green":[40,140],"yellow":[140,180],"red":[180,220]}},
                showCurrentValue=True,
                units="BPM",
                className="mb-3",
                style={'height': '320px'} # Increased height to fill space
            ),
        ], md=3),

        # Column 2: Other Info Cards
        dbc.Col([
            make_info_card("Temperature", "temperature-display", "fas fa-thermometer-half"),
            make_info_card("Battery", "battery-display", "fas fa-battery-full"),
        ], md=3),

        # Column 3: GPS Map
        dbc.Col([
            dbc.Card(
                dbc.CardBody([
                    html.H4("Live Location", className="card-title text-center"),
                    dcc.Graph(id='gps-map', style={'height': '320px'})
                ])
            )
        ], md=6),
    ]),

    # Historical Data Charts
    dbc.Row([
        dbc.Col(dcc.Graph(id='heart-rate-chart'), md=12), # Changed to md=12
    ], className="mt-4"),

    # Device Config Toggle Button
    dbc.Row([
        dbc.Col(
            dbc.Button(
                [html.I(className="fas fa-cogs me-2"), "Device Configuration"],
                id="collapse-device-config-button",
                className="mt-4 mb-3",
                color="warning",
                outline=True
            ),
            className="d-flex justify-content-end"
        )
    ]),

    # Control Section to publish data to ESP32 (now collapsible)
    dbc.Collapse(
        dbc.Card(
            dbc.CardBody([
                dbc.Row([
                    dbc.Col(dbc.Input(id='wifi-ssid-input', placeholder='New WiFi SSID'), md=4),
                    dbc.Col(dbc.Input(id='wifi-password-input', placeholder='New WiFi Password'), md=4),
                    dbc.Col(dbc.Input(id='bluetooth-name-input', placeholder='New Bluetooth Name'), md=4),
                ]),
                dbc.Button("Send Configuration to Device", id='publish-button', color='warning', className="mt-3 w-100")
            ]),
        ),
        id="device-config-collapse",
        is_open=False # Start hidden
    ),

    # Interval component to trigger updates
    dcc.Interval(id='interval-component', interval=1*1000, n_intervals=0), # 1 second
    html.Div(id="placeholder-output") # Placeholder for the publish callback
], fluid=True)


# --- Callbacks ---

@app.callback(
    Output("settings-collapse", "is_open"),
    [Input("collapse-settings-button", "n_clicks")],
    [State("settings-collapse", "is_open")],
)
def toggle_settings_collapse(n, is_open):
    """Toggle the visibility of the settings panel."""
    if n:
        return not is_open
    return is_open

@app.callback(
    Output("device-config-collapse", "is_open"),
    [Input("collapse-device-config-button", "n_clicks")],
    [State("device-config-collapse", "is_open")],
)
def toggle_device_config_collapse(n, is_open):
    """Toggle the visibility of the device config panel."""
    if n:
        return not is_open
    return is_open

@app.callback(
    [Output('connection-status', 'children'),
     Output('pet-name-store', 'data')],
    [Input('connect-button', 'n_clicks')],
    [State('mqtt-broker-input', 'value'),
     State('mqtt-username-input', 'value'),
     State('mqtt-password-input', 'value'),
     State('pet-name-input', 'value')],
    prevent_initial_call=True
)
def manage_connection(n_clicks, broker_addr, username, password, pet_name):
    """Handle the connect/disconnect logic for the MQTT client."""
    global mqtt_thread
    
    if n_clicks > 0:
        if mqtt_thread and mqtt_thread.is_alive():
            mqtt_thread.stop()
            mqtt_thread.join()
            mqtt_thread = None
            print("Disconnected from MQTT.")
        
        if not broker_addr or not pet_name:
            return "Broker and Pet Name are required.", dash.no_update

        try:
            # Split broker address and port
            if ':' in broker_addr:
                broker, port = broker_addr.split(':')
            else:
                broker, port = broker_addr, 1883 # Default MQTT port
            
            # Start the MQTT client in a new thread
            mqtt_thread = MqttClientThread(broker, port, username, password, pet_name)
            mqtt_thread.start()
            
            status_icon = html.I(className="fas fa-check-circle text-success")
            return status_icon, pet_name
        
        except Exception as e:
            status_icon = html.I(className="fas fa-times-circle text-danger")
            return f"{status_icon} Error: {e}", dash.no_update
            
    return "Not Connected", dash.no_update


@app.callback(
    [Output('heart-rate-gauge', 'value'),
     Output('temperature-display', 'children'),
     Output('battery-display', 'children'),
     Output('gps-map', 'figure'),
     Output('heart-rate-chart', 'figure')],
    [Input('interval-component', 'n_intervals')],
    [State('pet-name-store', 'data')]
)
def update_dashboard(n, pet_name):
    """Periodically update all visual components with the latest data."""
    with data_lock:
        # Make a local copy to avoid issues with data changing mid-update
        local_data = pet_data.copy()
        
    # --- Update Gauges and Cards ---
    hr = local_data['heart_rate']
    temp = f"{local_data['temperature']:.1f} °C"
    
    # --- Update Battery Icon and Color ---
    batt_perc = local_data['battery_percentage']
    if batt_perc > 80:
        batt_icon = "fas fa-battery-full"
        batt_color = "success"
    elif batt_perc > 50:
        batt_icon = "fas fa-battery-three-quarters"
        batt_color = "success"
    elif batt_perc > 20:
        batt_icon = "fas fa-battery-half"
        batt_color = "warning"
    else:
        batt_icon = "fas fa-battery-quarter"
        batt_color = "danger"
    
    battery_display = html.Span([
        f"{batt_perc:.0f}% ",
        html.I(className=batt_icon, style={'color': f'var(--bs-{batt_color})'})
    ])

    # --- Update GPS Map ---
    map_figure = go.Figure(go.Scattermapbox(
        lat=[local_data['latitude']],
        lon=[local_data['longitude']],
        mode='markers+text',
        marker=go.scattermapbox.Marker(size=20, color='red', symbol='marker'),
        text=[f"{pet_name}'s Location" if pet_name else "Location"],
        textposition="top right"
    ))
    map_figure.update_layout(
        mapbox_style="open-street-map",
        mapbox_center_lon=local_data['longitude'],
        mapbox_center_lat=local_data['latitude'],
        mapbox_zoom=15,
        margin={"r":0,"t":0,"l":0,"b":0},
        template="plotly_dark"
    )

    # --- Update Historical Charts ---
    hr_chart_fig = go.Figure()
    hr_chart_fig.add_trace(go.Scatter(x=list(TIME_HISTORY), y=list(HEART_RATE_HISTORY), mode='lines+markers', name='Heart Rate'))
    hr_chart_fig.update_layout(title='Heart Rate History', template='plotly_dark', yaxis_title='BPM')
    
    return hr, temp, battery_display, map_figure, hr_chart_fig


@app.callback(
    Output('publish-button', 'children'),
    [Input('publish-button', 'n_clicks')],
    [State('wifi-ssid-input', 'value'),
     State('wifi-password-input', 'value'),
     State('bluetooth-name-input', 'value'),
     State('pet-name-store', 'data')],
    prevent_initial_call=True
)
def publish_settings(n_clicks, ssid, password, bt_name, pet_name):
    """Publish new configuration settings to the ESP32."""
    global mqtt_thread
    ctx = dash.callback_context
    if not ctx.triggered:
        return "Send Configuration to Device"
        
    button_id = ctx.triggered[0]['prop_id'].split('.')[0]

    if button_id == 'publish-button' and mqtt_thread and mqtt_thread.is_alive():
        if ssid:
            mqtt_thread.publish("wifi/ssid", ssid)
        if password:
            mqtt_thread.publish("wifi/password", password)
        if bt_name:
            mqtt_thread.publish("bluetooth/name", bt_name)
        
        # Reset message after a delay could be implemented with another callback
        return "Configuration Sent!"
    elif not mqtt_thread or not mqtt_thread.is_alive():
        return "Error: Not Connected"
        
    return "Send Configuration to Device"


# --- Main Execution ---
if __name__ == '__main__':
    app.run(debug=True, port=8050)