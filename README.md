# solarization-occultation
temerature and humidity monitors for weed control experiment using clear/black poly covering

Processor will be an ESP8266 (Wemos d1 mini) from inventory
Sensors will be DHT22, also from inventory
There will be three sensors, one under the middle of each covering and one nearby outside the covering to measure ambient

A computer on the same WiFi network will be running an MQTT  server and a Node Red server. The sensor connected ESP8266 will publish environmental data. The Node Red server will subscribe to the MQTT server as a client, pull in the sensor data and display it graphically as a time series.