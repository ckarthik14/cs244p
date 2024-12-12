import time
import uuid
from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, emit
import boto3
from decimal import Decimal
from botocore.exceptions import NoCredentialsError, PartialCredentialsError
import os

# Flask setup
app = Flask(__name__)
socketio = SocketIO(app)

# DynamoDB setup
table_name = os.getenv("DYNAMODB_TABLE_NAME", "cs244p")
region_name = os.getenv("AWS_REGION", "us-east-1")

try:
    dynamodb = boto3.resource('dynamodb', region_name=region_name)
    table = dynamodb.Table(table_name)
except NoCredentialsError:
    print("No AWS credentials found. Make sure you have configured them or attached an IAM role.")
    exit(1)
except PartialCredentialsError:
    print("Partial AWS credentials found. Please check your configuration.")
    exit(1)
except Exception as e:
    print(f"Failed to initialize DynamoDB resource: {e}")
    exit(1)

# Flask routes
@app.route('/')
def index():
    return render_template('index.html')  # Load the main page

@app.route('/send_data', methods=['GET'])
def send_data():
    """
    Endpoint for sending accelerometer and/or heartbeat data.
    - For accelerometer: accelX, accelY, accelZ.
    - For heartbeat: heartbeat (BPM value).
    """
    response = {'status': 'success', 'emitted': [], 'dynamodb': []}

    accelX = request.args.get('accelX', type=float)
    accelY = request.args.get('accelY', type=float)
    accelZ = request.args.get('accelZ', type=float)
    heartbeat = request.args.get('heartbeat', type=int)

    # Prepare item for DynamoDB
    record_id = str(uuid.uuid4())
    timestamp = int(time.time() * 1000)

    item = {'record_id': record_id, 'timestamp': timestamp}
    if accelX is not None and accelY is not None and accelZ is not None:
        item.update({
            'accelX': Decimal(str(accelX)),
            'accelY': Decimal(str(accelY)),
            'accelZ': Decimal(str(accelZ)),
        })
        accel_data = {'x': accelX, 'y': accelY, 'z': accelZ}
        socketio.emit('update_graph', accel_data)  # Emit accelerometer data
        response['emitted'].append({'type': 'accelerometer', 'data': accel_data})

    if heartbeat is not None:
        item['heartbeat'] = Decimal(str(heartbeat))
        heartbeat_data = {'bpm': heartbeat}
        socketio.emit('update_heartbeat', heartbeat_data)  # Emit heartbeat data
        response['emitted'].append({'type': 'heartbeat', 'data': heartbeat_data})

    # Insert item into DynamoDB
    if 'accelX' in item or 'heartbeat' in item:
        try:
            table.put_item(Item=item)
            response['dynamodb'].append({'record_id': record_id, 'status': 'inserted'})
            print(f"Inserted: record_id={record_id}, item={item}")
        except Exception as e:
            response['status'] = 'error'
            response['dynamodb'].append({'record_id': record_id, 'error': str(e)})
            print(f"Failed to insert item {record_id}: {e}")

    # Handle missing parameters
    if not response['emitted']:
        response['status'] = 'error'
        response['message'] = 'No valid parameters provided.'

    return jsonify(response)

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)
