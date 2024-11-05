import dash
from dash import dcc, html
from dash.dependencies import Input, Output, State
import plotly.graph_objs as go
import requests
from datetime import datetime
import pytz

# Constants for IP and port
IP_ADDRESS = "20.206.204.120"
PORT_STH = 8666
DASH_HOST = "0.0.0.0"


# Function to retrieve data for a given attribute
def get_sensor_data(attribute, lastN):
    url = f"http://{IP_ADDRESS}:{PORT_STH}/STH/v1/contextEntities/type/Lamp/id/urn:ngsi-ld:Lamp:06x/attributes/{attribute}?lastN={lastN}"
    headers = {
        'fiware-service': 'smart',
        'fiware-servicepath': '/'
    }
    response = requests.get(url, headers=headers)
    if response.status_code == 200:
        data = response.json()
        try:
            values = data['contextResponses'][0]['contextElement']['attributes'][0]['values']
            return values
        except KeyError as e:
            print(f"Key error: {e}")
            return []
    else:
        print(f"Error accessing {url}: {response.status_code}")
        return []


# Convert UTC timestamps to Lisbon time
def convert_to_lisbon_time(timestamps):
    utc = pytz.utc
    lisbon = pytz.timezone('Europe/Lisbon')
    converted_timestamps = []
    for timestamp in timestamps:
        try:
            timestamp = timestamp.replace('T', ' ').replace('Z', '')
            converted_time = utc.localize(datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S.%f')).astimezone(lisbon)
        except ValueError:
            converted_time = utc.localize(datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S')).astimezone(lisbon)
        converted_timestamps.append(converted_time)
    return converted_timestamps


lastN = 30

app = dash.Dash(__name__)

app.layout = html.Div([
    html.H1('Sensor Data Viewer'),
    dcc.Graph(id='temperature-graph'),
    dcc.Graph(id='humidity-graph'),
    dcc.Graph(id='luminosity-graph'),
    dcc.Store(id='sensor-data-store', data={'timestamps': [], 'temperature_values': [], 'humidity_values': [], 'luminosity_values': []}),
    dcc.Interval(id='interval-component', interval=10*1000, n_intervals=0)
])


@app.callback(
    Output('sensor-data-store', 'data'),
    Input('interval-component', 'n_intervals'),
    State('sensor-data-store', 'data')
)
def update_data_store(n, stored_data):
    lastN = 30  # Define o número máximo de registros a serem exibidos
    data_temperature = get_sensor_data('temperature', lastN)
    data_humidity = get_sensor_data('humidity', lastN)
    data_luminosity = get_sensor_data('luminosity', lastN)

    if data_temperature and data_humidity and data_luminosity:
        temperature_values = [float(entry['attrValue']) for entry in data_temperature]
        humidity_values = [float(entry['attrValue']) for entry in data_humidity]
        luminosity_values = [float(entry['attrValue']) for entry in data_luminosity]
        timestamps = [entry['recvTime'] for entry in data_luminosity]

        timestamps = convert_to_lisbon_time(timestamps)

        # Substituir os dados antigos por novos até lastN
        stored_data['timestamps'] = timestamps[-lastN:]
        stored_data['temperature_values'] = temperature_values[-lastN:]
        stored_data['humidity_values'] = humidity_values[-lastN:]
        stored_data['luminosity_values'] = luminosity_values[-lastN:]

        return stored_data

    return stored_data


def create_graph(trace_values, trace_name, color, y_title, y_min=None, y_max=None):
    fig = go.Figure(data=[go.Scatter(
        x=trace_values['timestamps'],
        y=trace_values[trace_name],
        mode='lines+markers',
        name=trace_name,
        line=dict(color=color)
    )])

    # Adiciona traços de limite com legendas
    if y_min is not None:
        fig.add_trace(go.Scatter(
            x=trace_values['timestamps'],
            y=[y_min] * len(trace_values['timestamps']),
            mode='lines',
            line=dict(color='black', dash='dash'),
            name=f'{y_title} Min'
        ))

    if y_max is not None:
        fig.add_trace(go.Scatter(
            x=trace_values['timestamps'],
            y=[y_max] * len(trace_values['timestamps']),
            mode='lines',
            line=dict(color='black', dash='dash'),
            name=f'{y_title} Max'
        ))

    fig.update_layout(
        title=f'{trace_name.capitalize()} Over Time',
        xaxis_title='Timestamp',
        yaxis_title=y_title,
        hovermode='closest'
    )

    return fig


@app.callback(
    Output('temperature-graph', 'figure'),
    Output('humidity-graph', 'figure'),
    Output('luminosity-graph', 'figure'),
    Input('sensor-data-store', 'data')
)
def update_graphs(stored_data):
    fig_temperature = create_graph(
        stored_data, 'temperature_values', 'red', 'Temperature (°C)', y_min=15, y_max=25
    )
    fig_humidity = create_graph(
        stored_data, 'humidity_values', 'blue', 'Humidity (%)', y_min=30, y_max=50
    )
    fig_luminosity = create_graph(
        stored_data, 'luminosity_values', 'orange', 'Luminosity (%)', y_min=0, y_max=30
    )

    return fig_temperature, fig_humidity, fig_luminosity


if __name__ == '__main__':
    app.run_server(debug=True, host=DASH_HOST, port=8050)
