import threading
import pika
import paho.mqtt.client as mqtt
import os 
from urllib.parse import urlparse

# Define event callbacks for MQTT
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def on_publish(client, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)

# Function to handle MQTT connection
def mqtt_thread():
    mqttc = mqtt.Client()
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_publish = on_publish
    mqttc.on_subscribe = on_subscribe

    url_str = os.environ.get('CLOUDMQTT_URL', 'mqtt://localhost:1883')
    url = urlparse(url_str)
    topic = 'coffe_machine'  # define topic here
    username = "guest"
    password = "guest"

    mqttc.username_pw_set(username, password)
    mqttc.connect(url.hostname, url.port)
    mqttc.subscribe(topic, 0)
    mqttc.loop_forever()

# Function to handle RabbitMQ connection
def rabbitmq_thread():
    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()
    channel.queue_declare(queue='coffe_machine')

    def callback(ch, method, properties, body):
        print("Received message from RabbitMQ:", body)

    channel.basic_consume(queue='coffe_machine', on_message_callback=callback, auto_ack=True)
    channel.start_consuming()

# Start MQTT and RabbitMQ threads
mqtt_thread = threading.Thread(target=mqtt_thread)
mqtt_thread.daemon = True
mqtt_thread.start()

rabbitmq_thread = threading.Thread(target=rabbitmq_thread)
rabbitmq_thread.daemon = True
rabbitmq_thread.start()

connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
channel = connection.channel()

channel.queue_declare(queue='coffe_machine')

channel.basic_publish(exchange='',
                      routing_key='coffe_machine',
                      body='L')


# Keep the main thread alive
while True:
    pass


