docker build -t rabbitmq:mqtt .

docker run -d --name rabbitmq -p 15672:15672 -p 1883:1883 -p 5672:5672 rabbitmq:mqtt
