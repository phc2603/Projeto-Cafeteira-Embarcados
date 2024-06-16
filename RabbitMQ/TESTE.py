import pika
import paho.mqtt.client as mqtt
from urllib.parse import urlparse

connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
channel = connection.channel()

channel.basic_publish(exchange='',
                      routing_key='mqtt-subscription-coffeqos0',
                      body='M')
channel.basic_publish(exchange='',
                      routing_key='mqtt-subscription-coffeqos0',
                      body='S')
