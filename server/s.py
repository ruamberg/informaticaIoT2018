from flask import Flask
from flask import request
from flask import jsonify

app = Flask(__name__)

@app.route('/')
def index():
    return 'Index'

@app.route('/hello')
def hello():
    return 'Hello, World'

@app.route('/user/<name>')
def show_name(name):
    return 'Post %s' %name

value = 0

@app.route('/id/<int:post_id>', methods=['GET', 'POST'])
def show_post(post_id):
    global value
    if request.method == 'GET':
        return 'id = %d value = %d' %(post_id, value) 
    if request.method == 'POST':
        if request.json:
            data = request.json
            value = data.get("value")
            print('%d' %value)
            return 'id = %d value = %d' %(post_id, value)
 
sensors = [0]*5
sensors_available = []

@app.route('/sensor', methods=['GET', 'POST'])
def sensor_get():
    if request.method == 'GET':
        if  request.args.get('id'):
            sensor_id = request.args.get('id')
            global sensors
            return 'id = %d value = %d' %(int(sensor_id), sensors[int(sensor_id)])
        else:
            global sensors_available
            return str(sensors_available)

    if request.method == 'POST':
        if request.args.get('id'):
            if request.json:
                sensor_id = request.args.get('id')
                data = request.json
                global sensor_value
                sensor_value = data.get("value")
                 
                sensors[int(sensor_id)] = sensor_value
                return 'id = %d value = %d' %(int(sensor_id), sensors[int(sensor_id)])
        else:
            if request.json:
                data = request.json
                
                sensors_available = data.get("available")
                return str(sensors_available)
                

