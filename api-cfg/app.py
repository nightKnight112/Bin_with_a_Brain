import streamlit as st  # localhost api
import tensorflow as tf
from keras.utils import save_img
from tensorflow import keras
from PIL import Image
from fpdf import FPDF
import webbrowser
import random
import requests
import geocoder
import folium
from streamlit_folium import st_folium
import paho.mqtt.client as mqtt_client


# mqtt routes: Detection Biodegradable/Open and Detection Non-Biodegradable/Open


MODELSPATH = 'bin_witha_brain_modelf.h5'
DATAPATH = 'waste1.jpg'  # sample image
mqtt_Broker = ""  # localhost ip(use pc mainframe ip)
client = mqtt_client.Client("Mainframe") # Client name Mainframe
# client.connect(mqtt_Broker) # uncomment after setting mqtt_Broker
topic_1 = "Detection Biodegradable"  # topic for publishing if biodegradable
topic_2 = "Detection Non-Biodegradable"  # topic for publishing if non-biodegradable

i = 0


def pdf_name_gen(i):
    # gen pred wise
    name = 'pred' + str(i) + '.pdf'
    i += 1
    return name


def funcs(img_pth, label_p, token_and_extras):
    pdf = FPDF()

    # Add a page
    pdf.add_page()

    # set style and size of font
    # that you want in the pdf
    pdf.image(img_pth, x=50, y=100)
    pdf.set_font("Arial", size=15)

    # create a cell
    pdf.cell(200, 10, txt=label_p, ln=1, align='C')

    # add another cell
    pdf.cell(200, 10, txt=token_and_extras, ln=2, align='C')

    # pdf name
    n = pdf_name_gen(i)

    # save the pdf with name .pdf
    pdf.output(n)

    return n


@st.cache_data
def load_data():
    img = Image.open(DATAPATH)
    return img


def load_models():
    model = tf.keras.saving.load_model(MODELSPATH, compile=False)
    return model


st.title('Bins with a Brain')  # project title
st.header('A Waste Management Application for the Future!!!')
st.text('A Web Application for smart and efficient Waste Management')
st.markdown('To see prediction one can use a sample data(already provided) or upload their own image.')

st.sidebar.title('Select options:')
page = st.sidebar.selectbox("Choose a page:", ["Sample Data to Test App", "Upload an Image to Report Wastes",
                                               "Use Camera to Upload Image and Report Wastes",
                                               "Nearby Reporting point location"])

if page == "Sample Data to Test App":
    st.header("Sample prediction for testing if the model works")
    if st.checkbox('Sample Data exploration'):
        st.info("Sample image:")
        image = load_data()
        save_img('waste.jpg', image)
        st.image(image, caption='Sample Data', use_column_width=True)
        image = image.resize((64, 64))
        image = tf.keras.preprocessing.image.img_to_array(image)
        image = image.reshape((1, 64, 64, 3))
        st.subheader("Check the prediction")
        if st.checkbox('The Prediction is'):
            model = load_models()
            result = model.predict(image)
            st.write(result)
            if result[0][0] > 0.65:
                prediction = 'Recyclable/Non-Biodegradable Waste'
                client.publish(topic_2, "Open")

            else:
                prediction = 'Organic/Biodegradable Waste'
                client.publish(topic_1, "Open")
            st.write(prediction)
            st.success("It is prediction for sample image and the app is working")

elif page == "Upload an Image to Report Wastes":
    st.header("Please upload a photo of the lying wastes to report")
    uploaded_file = st.file_uploader("Choose your image", type=["jpg", "png"])
    if uploaded_file is not None:
        img = Image.open(uploaded_file)
        st.info("Your image when generated looks like")
        st.image(img, caption="Upload image", use_column_width=True)
        img = img.resize((64, 64))
        tf.keras.utils.save_img('default.jpg', img)
        img = tf.keras.preprocessing.image.img_to_array(img)
        img = img.reshape((1, 64, 64, 3))
        st.subheader("Waste Prediction")
        if st.checkbox('The prediction is'):
            model = load_models()
            result = model.predict(img)
            st.write(result)
            if result[0][0] > 0.65:
                prediction = 'Recyclable/Non-Biodegradable Waste'
                token = random.randint(100000, 99999999999)
                token = str(token)
                token = 'Token number is:' + str(token)
                filename = funcs('default.jpg', prediction, token)
            else:
                prediction = 'Organic/Biodegradable Waste'
                token = random.randint(100000, 99999999999)
                token = str(token)
                token = 'Token number is:' + str(token)
                filename = funcs('default.jpg', prediction, token)

            st.write(prediction)
            st.success("Successfully predicted, click below to report to the authorities")
            if st.button("File a Report", key='started', help='click here to report wastes lying around'):
                webbrowser.open_new_tab('https://www.wm.com/')
            st.download_button("Download the Generated Report", filename, key='download',
                               file_name='report.pdf',
                               help='click here to download the generated report for manual submission')

elif page == "Use Camera to Upload Image and Report Wastes":
    st.header("Your prediction of waste")
    image = st.camera_input("Take a picture")

    if image is not None:
        i += 1
        img = Image.open(image)
        st.info("Photo Captured looks like:")
        st.image(img, caption="Captured Image", use_column_width=True)
        img = img.resize((64, 64))
        tf.keras.utils.save_img('default.jpg', img)
        img = tf.keras.preprocessing.image.img_to_array(img)
        img = img.reshape((1, 64, 64, 3))
        st.subheader("Check waste prediction")
        if st.checkbox('Prediction of your uploaded photo is:'):
            model = load_models()
            result = model.predict(img)
            st.write(result)
            if result[0][0] > 0.65:  # based on confusion matrix[0][0]
                prediction = 'Recyclable/Non-Biodegradable Waste'
                client.publish(topic_2, "Open")
                token = random.randint(100000, 99999999999)
                token = str(token)
                token = 'Token number is:' + str(token)
                filename = funcs('default.jpg', prediction, token)
            else:
                prediction = 'Organic/Biodegradable Waste'
                client.publish(topic_1, "Open")
                token = random.randint(100000, 99999999999)
                token = str(token)
                token = 'Token number is:' + str(token)
                filename = funcs('default.jpg', prediction, token)

            st.write(prediction)
            st.success("Successfully predicted, click below to report to the authorities")
            if st.button("File a Report", key='started', help='click here to report wastes lying around'):
                webbrowser.open_new_tab('https://www.wm.com/')
            st.download_button("Download the Generated Report", filename, key='download',
                               file_name='report.pdf',
                               help='click here to download the generated report for manual submission')

elif page == "Nearby Reporting point location":

    st.header("Your Nearest Report Center is at: ")

    # g = geocoder.ip('me')
    # lcn = g.latlng # dynamic ip generation
    map_ac = folium.folium.Map(location=[22.977026925839883, 88.44656142617595], zoom_start=16)  # hcded to kly_munc
    folium.Marker([22.977026925839883, 88.44656142617595], popup="Waste disposal", tooltip="central disposal").add_to(
        map_ac)
    st_dta = st_folium(map_ac, width=725)
