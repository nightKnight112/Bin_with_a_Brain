import paho.mqtt.client as mqtt
import app_auto
mqtt_Broker = "192.168.23.105"  # localhost ip(use pc mainframe ip)
pub_topic = "/feeds/ml_result"
sub_topic = "/feeds/Obj_distance_pub"


def on_connect(client, userdata, flags, rc):  # The callback for when the client connects to the broker 
    print("Connected with result code {0}".format(str(rc)))  
    # Print result of connection attempt client.subscribe("digitest/test1")  
    # Subscribe to the topic “digitest/test1”, receive any messages published on it


def on_message(client, userdata, msg):  # The callback for when a PUBLISH message is received from the server. 
    message = int(msg.payload.decode("utf-8"))
    print("Message received-> "+ msg.topic + " " + message)  # Print a received msg
    if message == 1:
        img = app_auto.capture()
        app_auto.Predict(img)

client = mqtt.Client("MainFrame")  # Create instance of client with client ID “digi_mqtt_test”
client.on_connect = on_connect  # Define callback function for successful connection
client.on_message = on_message  # Define callback function for receipt of a message
# client.connect("m2m.eclipse.org", 1883, 60)  # Connect to (broker, port, keepalive-time)
client.connect(mqtt_Broker, 1883)
client.subscribe(sub_topic)
client.loop_forever()  # Start networking daemon