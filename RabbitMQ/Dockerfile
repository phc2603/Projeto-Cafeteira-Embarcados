FROM rabbitmq:management

RUN rabbitmq-plugins enable rabbitmq_mqtt

EXPOSE 15672 1883 5672

CMD ["rabbitmq-server"]
