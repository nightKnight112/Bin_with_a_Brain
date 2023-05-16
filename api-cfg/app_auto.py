import streamlit as st  # localhost api
import tensorflow as tf
from keras.utils import save_img
from tensorflow import keras
from PIL import Image
from fpdf import FPDF
import webbrowser
import random
from streamlit_folium import st_folium
import paho.mqtt.client as mqtt
import pygame
import pygame.camera
import time

mqtt_Broker = "192.168.0.105"  # localhost ip(use pc mainframe ip)
pub_topic = "/feeds/ml_result"
Log = "/feeds/log_monitor"
sub_topic = "/feeds/Obj_distance_pub"

MODELSPATH = 'bin_witha_brain_modelf.h5'
DATAPATH = 'waste1.jpg'  # sample image


def on_connect(client, userdata, flags, rc):  # The callback for when the client connects to the broker 
    print("Connected with result code {0}".format(str(rc)))
    return

def on_message(client, userdata, msg):  # The callback for when a PUBLISH message is received from the server. 
    message = int(msg.payload.decode("utf-8"))
    print("Message received-> "+ msg.topic + " " + str(message))  # Print a received msg
    
    if message<=30:
        st.write("Starting Camera. Place the wastes in-front of camera.")
        client.publish(Log,message)
        client.publish(Log,"Starting Camera...")
        img = capture()
        Predict(img)
    elif message == -10:
        st.write("Quitting...")
        client.publish(Log,"QUIT command recieved... Quitting...")
        quit()

def capture():
    pygame.camera.init()
    # initializing the cam variable with default camera
    cam = pygame.camera.Camera(0)
    # opening the camera
    cam.start()
    client.publish(Log,"Camera Started")
    # capturing the single image
    image = cam.get_image()
    client.publish(Log,"Getting Image...")
    # saving the image
    pygame.image.save(image, "captured_img.jpg")
    client.publish(Log,"Image saved.")
    return image

# ..................................................................................................

def load_models():
    model = keras.models.load_model(MODELSPATH, compile=False)
    return model
# .................................................................................................

def Predict(image):
    st.header("Your prediction of waste")
    if image is not None:
        img = Image.open("captured_img.jpg")
        st.info("Photo Captured looks like:")
        st.image(img, caption="Captured Image", use_column_width=True)
        img = img.resize((64, 64))
        tf.keras.utils.save_img('default.jpg', img)
        img = tf.keras.preprocessing.image.img_to_array(img)
        img = img.reshape((1, 64, 64, 3))
        st.subheader("Check waste prediction")
        
        model = load_models()
        result = model.predict(img)
        st.write(result)
        if result[0][0] > 0.65:  # based on confusion matrix[0][0]
            prediction = 'Recyclable/Non-Biodegradable Waste'
            client.publish(pub_topic, 1)
        else:
            prediction = 'Organic/Biodegradable Waste'
            client.publish(pub_topic, 2)
        st.write(prediction)
        st.success("Successfully predicted :)")
        time.sleep(10)
        client.publish(Log,"Prediction Successfull")

# Main Section .............................................................................................
st.title('Bins with a Brain')  # project title
st.header('A Waste Management System for the Future')
st.text("Go in front of the Dustbin and wait... ")

client = mqtt.Client("MainFrame")  # Create instance of client with client ID “digi_mqtt_test”
client.on_connect = on_connect  # Define callback function for successful connection
client.on_message = on_message  # Define callback function for receipt of a message
# client.connect("m2m.eclipse.org", 1883, 60)  # Connect to (broker, port, keepalive-time)
#client.loop_start()
client.connect(mqtt_Broker, 1883)
client.subscribe(sub_topic)
client.loop_forever()  # Start networking daemon