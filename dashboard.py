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

MAP_STYLE_CHOICES = {
    "maplibre_demo": {
        "label": "MapLibre Streets (English)",
        "style": "https://demotiles.maplibre.org/style.json"
    },
    "carto_positron": {
        "label": "Carto Positron",
        "style": "https://basemaps.cartocdn.com/gl/positron-gl-style/style.json"
    },
    "carto_dark": {
        "label": "Carto Dark Matter",
        "style": "https://basemaps.cartocdn.com/gl/dark-matter-gl-style/style.json"
    },
    "osm_light": {
        "label": "OpenStreetMap",
        "style": "open-street-map"
    }
}

# --- Helper Utilities ---
def format_coordinate(value, positive_suffix, negative_suffix):
    suffix = positive_suffix if value >= 0 else negative_suffix
    return f"{abs(value):.4f}° {suffix}"


def compute_zoom(latitudes, longitudes):
    if len(latitudes) < 2 or len(longitudes) < 2:
        return 15

    lat_span = max(latitudes) - min(latitudes)
    lon_span = max(longitudes) - min(longitudes)
    max_span = max(lat_span, lon_span)

    if max_span < 0.0005:
        return 17
    if max_span < 0.005:
        return 15
    if max_span < 0.05:
        return 12
    if max_span < 0.5:
        return 10
    if max_span < 1.5:
        return 8
    return 6

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

# Location history for map auto-centering
LAT_HISTORY = deque([pet_data["latitude"]], maxlen=200)
LON_HISTORY = deque([pet_data["longitude"]], maxlen=200)

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
        global pet_data, HEART_RATE_HISTORY, TIME_HISTORY, LAT_HISTORY, LON_HISTORY
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

                if "gps/" in topic:
                    lat = pet_data["latitude"]
                    lon = pet_data["longitude"]
                    if LAT_HISTORY and LON_HISTORY:
                        if abs(LAT_HISTORY[-1] - lat) > 1e-6 or abs(LON_HISTORY[-1] - lon) > 1e-6:
                            LAT_HISTORY.append(lat)
                            LON_HISTORY.append(lon)
                    else:
                        LAT_HISTORY.append(lat)
                        LON_HISTORY.append(lon)

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

# Helper function to create styled metric cards
def make_metric_card(title, content_id, icon, description=None):
    return dbc.Card(
        dbc.CardBody([
            html.Div([
                html.I(className=f"{icon} fa-lg text-info"),
                html.Span(title, className="ms-2 text-uppercase fw-semibold text-secondary")
            ], className="d-flex align-items-center justify-content-start"),
            html.Div(id=content_id, className="display-6 fw-bold text-light mt-3"),
            html.Small(description, className="text-muted") if description else None
        ]),
        className="shadow-sm bg-dark border-0 text-light h-100"
    )

# Main layout of the dashboard
app.layout = dbc.Container([
    dcc.Store(id='mqtt-client-store', data=None),
    dcc.Store(id='pet-name-store', data=None),
    dcc.Store(id='auto-center-store', data=True),
    dcc.Store(id='map-view-store', data={"lat": pet_data["latitude"], "lon": pet_data["longitude"], "zoom": 15}),

    dbc.Row([
        dbc.Col([
            html.Div([
                html.Span("Live Pet Telemetry", className="text-info text-uppercase fw-semibold"),
                html.H1("🐾 Pet Health & Location Dashboard", className="display-5 fw-bold text-light mt-2"),
                html.P(
                    "Monitor vitals, map movement, and push device updates with a single responsive dashboard.",
                    className="text-muted mb-0"
                )
            ], className="py-3")
        ], width=12)
    ], className="mb-2"),

    dbc.Row([
        dbc.Col(
            dbc.Button(
                [html.I(className="fas fa-link me-2"), "Connection Settings"],
                id="collapse-settings-button",
                color="info",
                outline=False,
                className="shadow-sm"
            ),
            className="d-flex justify-content-end",
            width=12
        )
    ], className="mb-3"),

    dbc.Collapse(
        dbc.Card(
            dbc.CardBody([
                dbc.Row([
                    dbc.Col(dbc.Input(id='mqtt-broker-input', placeholder='MQTT Broker (e.g., 4.tcp.ngrok.io:12345)', value='broker.hivemq.com'), lg=3, md=6, className="mb-2 mb-lg-0"),
                    dbc.Col(dbc.Input(id='mqtt-username-input', placeholder='MQTT Username (optional)'), lg=2, md=6, className="mb-2 mb-lg-0"),
                    dbc.Col(dbc.Input(id='mqtt-password-input', placeholder='MQTT Password (optional)', type='password'), lg=2, md=6, className="mb-2 mb-lg-0"),
                    dbc.Col(dbc.Input(id='pet-name-input', placeholder='Pet Name (e.g., PET)', value='PET'), lg=2, md=6, className="mb-2 mb-lg-0"),
                    dbc.Col(dbc.Button("Connect", id='connect-button', color='primary', className="w-100"), lg=2, md=6, className="mb-2 mb-lg-0"),
                    dbc.Col(html.Div(id='connection-status', className="text-center mt-2"), lg=1, md=6)
                ])
            ]),
            className="bg-dark border-0 shadow-sm mb-4"
        ),
        id="settings-collapse",
        is_open=False
    ),

    dbc.Row([
        dbc.Col(make_metric_card("Temperature", "temperature-display", "fas fa-temperature-high", "Body surface"), lg=3, md=6, className="mb-4"),
        dbc.Col(make_metric_card("Battery", "battery-display", "fas fa-battery-half", "Device charge"), lg=3, md=6, className="mb-4"),
        dbc.Col(make_metric_card("Last Update", "last-update-display", "fas fa-history", "Latest packet"), lg=3, md=6, className="mb-4"),
        dbc.Col(make_metric_card("Coordinates", "coordinates-display", "fas fa-map-marker-alt", "Current position"), lg=3, md=6, className="mb-4"),
    ], className="g-3"),

    dbc.Row([
        dbc.Col(
            dbc.Card(
                dbc.CardBody([
                    html.Div([
                        html.Div([
                            html.H4("Live Location", className="mb-0 text-light"),
                            html.Small("Track movement history and current fix", className="text-muted")
                        ]),
                        html.Div([
                            dcc.Dropdown(
                                id='map-style-dropdown',
                                options=[
                                    {"label": choice["label"], "value": key}
                                    for key, choice in MAP_STYLE_CHOICES.items()
                                ],
                                value='maplibre_demo',
                                clearable=False,
                                className='flex-grow-1',
                                style={'minWidth': '200px'}
                            ),
                            dbc.Button(
                                "Auto Center: On",
                                id='auto-center-toggle',
                                color='info',
                                outline=False,
                                size='sm',
                                className='ms-lg-2 mt-2 mt-lg-0'
                            )
                        ], className='d-flex flex-column flex-lg-row align-items-lg-center gap-2')
                    ], className="d-flex flex-column flex-lg-row align-items-lg-center justify-content-between mb-3 gap-2"),
                    dcc.Graph(id='gps-map', style={'height': '420px'})
                ]),
                className="shadow-sm bg-dark border-0 h-100"
            ),
            lg=8,
            className="mb-4"
        ),
        dbc.Col(
            [
                dbc.Card(
                    dbc.CardBody([
                        html.H4("Heart Rate", className="text-light"),
                        html.P("Live BPM feed with color-coded zones", className="text-muted"),
                        daq.Gauge(
                            id='heart-rate-gauge',
                            label="",
                            value=0,
                            min=40,
                            max=220,
                            color={"gradient": True, "ranges": {"green": [40, 140], "yellow": [140, 180], "red": [180, 220]}},
                            showCurrentValue=True,
                            units="BPM",
                            style={'height': '320px'}
                        )
                    ]),
                    className="shadow-sm bg-dark border-0"
                ),
                dbc.Card(
                    dbc.CardBody([
                        html.H5("Device Actions", className="text-light mb-3"),
                        dbc.Button(
                            [html.I(className="fas fa-cogs me-2"), "Device Configuration"],
                            id="collapse-device-config-button",
                            color="warning",
                            outline=False,
                            className="w-100"
                        )
                    ]),
                    className="shadow-sm bg-dark border-0 mt-4"
                )
            ],
            lg=4,
            className="mb-4"
        )
    ], className="g-4"),

    dbc.Collapse(
        dbc.Card(
            dbc.CardBody([
                dbc.Row([
                    dbc.Col(dbc.Input(id='wifi-ssid-input', placeholder='New WiFi SSID'), lg=4, md=6, className="mb-3"),
                    dbc.Col(dbc.Input(id='wifi-password-input', placeholder='New WiFi Password'), lg=4, md=6, className="mb-3"),
                    dbc.Col(dbc.Input(id='bluetooth-name-input', placeholder='New Bluetooth Name'), lg=4, md=6, className="mb-3"),
                ], className="g-3"),
                dbc.Button("Send Configuration to Device", id='publish-button', color='warning', className="mt-2 w-100")
            ]),
            className="shadow-sm bg-dark border-0"
        ),
        id="device-config-collapse",
        is_open=False
    ),

    dbc.Row([
        dbc.Col(
            dbc.Card(
                dbc.CardBody([
                    html.H4("Heart Rate History", className="text-light"),
                    dcc.Graph(id='heart-rate-chart', style={'height': '320px'})
                ]),
                className="shadow-sm bg-dark border-0"
            ),
            width=12,
            className="mt-4"
        )
    ]),

    dcc.Interval(id='interval-component', interval=1*1000, n_intervals=0),
    html.Div(id="placeholder-output")
], fluid=True, className="py-4 px-3 bg-black min-vh-100")


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
    [Output('auto-center-store', 'data'),
     Output('auto-center-toggle', 'children'),
     Output('auto-center-toggle', 'color')],
    [Input('auto-center-toggle', 'n_clicks')],
    [State('auto-center-store', 'data')],
    prevent_initial_call=True
)
def toggle_auto_center(n_clicks, is_enabled):
    """Enable or disable automatic map centering."""
    if is_enabled is None:
        is_enabled = True
    new_state = not is_enabled
    label = "Auto Center: On" if new_state else "Auto Center: Off"
    color = 'info' if new_state else 'secondary'
    return new_state, label, color

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
     Output('coordinates-display', 'children'),
     Output('last-update-display', 'children'),
     Output('gps-map', 'figure'),
     Output('heart-rate-chart', 'figure'),
     Output('map-view-store', 'data')],
    [Input('interval-component', 'n_intervals'),
     Input('gps-map', 'relayoutData')],
    [State('pet-name-store', 'data'),
     State('auto-center-store', 'data'),
     State('map-view-store', 'data'),
     State('map-style-dropdown', 'value')]
)
def update_dashboard(n, relayout_data, pet_name, auto_center_enabled, map_view, map_style_key):
    """Periodically update all visual components with the latest data."""
    with data_lock:
        # Make a local copy to avoid issues with data changing mid-update
        local_data = pet_data.copy()
        lat_history = list(LAT_HISTORY)
        lon_history = list(LON_HISTORY)
        hr_history = list(HEART_RATE_HISTORY)
        time_history = list(TIME_HISTORY)

    if auto_center_enabled is None:
        auto_center_enabled = True

    if not map_view:
        map_view = {"lat": local_data['latitude'], "lon": local_data['longitude'], "zoom": 15}

    view_data = {
        "lat": map_view.get("lat", local_data['latitude']),
        "lon": map_view.get("lon", local_data['longitude']),
        "zoom": map_view.get("zoom", 15)
    }

    if relayout_data and not auto_center_enabled:
        center_dict = relayout_data.get('map.center') or relayout_data.get('mapbox.center')
        if isinstance(center_dict, dict):
            view_data['lat'] = center_dict.get('lat', view_data['lat'])
            view_data['lon'] = center_dict.get('lon', view_data['lon'])

        lat_value = relayout_data.get('map.center.lat') or relayout_data.get('mapbox.center.lat')
        lon_value = relayout_data.get('map.center.lon') or relayout_data.get('mapbox.center.lon')
        if lat_value is not None:
            view_data['lat'] = lat_value
        if lon_value is not None:
            view_data['lon'] = lon_value

        zoom_value = relayout_data.get('map.zoom') or relayout_data.get('mapbox.zoom')
        if zoom_value is not None:
            try:
                zoom_clamped = max(1.0, min(float(zoom_value), 20.0))
                view_data['zoom'] = zoom_clamped
            except (TypeError, ValueError):
                pass

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

    # --- Coordinates & Timestamp ---
    latitude = lat_history[-1] if lat_history else local_data['latitude']
    longitude = lon_history[-1] if lon_history else local_data['longitude']

    coords_display = html.Span(
        f"{format_coordinate(latitude, 'N', 'S')}, {format_coordinate(longitude, 'E', 'W')}",
        className="fw-semibold"
    )

    last_update_value = local_data['last_update'] if local_data['last_update'] != "Never" else "Waiting for data"
    last_update_display = html.Span(last_update_value, className="fw-semibold")

    # --- Update GPS Map ---
    map_figure = go.Figure()

    map_figure.add_trace(go.Scattermap(
        lat=[latitude],
        lon=[longitude],
        mode='markers+text',
        marker=dict(size=22, color='#ff4757'),
        text=[f"{pet_name}'s location" if pet_name else "Current location"],
        textposition="top right",
        name='Pet'
    ))

    computed_zoom = compute_zoom(lat_history, lon_history)
    if auto_center_enabled:
        view_data = {
            "lat": latitude,
            "lon": longitude,
            "zoom": computed_zoom
        }
    else:
        view_data['lat'] = view_data.get('lat', latitude)
        view_data['lon'] = view_data.get('lon', longitude)
        view_data['zoom'] = view_data.get('zoom', computed_zoom)

    center_lat = view_data['lat']
    center_lon = view_data['lon']
    center_zoom = view_data['zoom'] if view_data['zoom'] is not None else computed_zoom

    try:
        center_lat = float(center_lat)
        center_lon = float(center_lon)
    except (TypeError, ValueError):
        center_lat = latitude
        center_lon = longitude

    try:
        center_zoom = float(center_zoom)
    except (TypeError, ValueError):
        center_zoom = computed_zoom

    view_data['lat'] = center_lat
    view_data['lon'] = center_lon
    view_data['zoom'] = center_zoom

    style_entry = MAP_STYLE_CHOICES.get(map_style_key) or MAP_STYLE_CHOICES['maplibre_demo']
    map_style = style_entry['style']

    map_figure.update_layout(
        map=dict(style=map_style, center=dict(lat=center_lat, lon=center_lon), zoom=center_zoom),
        margin={"r":0,"t":0,"l":0,"b":0},
        template="plotly_dark",
        paper_bgcolor='rgba(0,0,0,0)',
        plot_bgcolor='rgba(0,0,0,0)',
        showlegend=False,
        transition={'duration': 500, 'easing': 'cubic-in-out'}
    )

    # --- Update Historical Charts ---
    hr_chart_fig = go.Figure()
    if hr_history and time_history:
        hr_chart_fig.add_trace(go.Scatter(
            x=time_history,
            y=hr_history,
            mode='lines+markers',
            name='Heart Rate',
            line=dict(color='#17a2b8', width=3),
            marker=dict(color='#f9c74f', size=8)
        ))
    hr_chart_fig.update_layout(
        template='plotly_dark',
        yaxis_title='BPM',
        xaxis_title='Time',
        margin=dict(l=40, r=20, t=10, b=40),
        paper_bgcolor='rgba(0,0,0,0)',
        plot_bgcolor='rgba(0,0,0,0)'
    )
    
    return hr, temp, battery_display, coords_display, last_update_display, map_figure, hr_chart_fig, view_data


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