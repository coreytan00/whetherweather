from flask import Flask, request, url_for, redirect, render_template, jsonify, session
from collections import defaultdict

app = Flask(__name__)
app.secret_key="notsecret"
app.config["SESSION_TYPE"]="filesystem"
app.config["SESSION_PERMANENT"] = True

temps = defaultdict(list)
leds = defaultdict(list)
temp = 0
user_led = 0
tmp_measured = 25.26

led1, led2, led3 = 0, 0, 0

@app.route("/", methods=["POST", "GET"])
def index():
    session["tmp"] = tmp_measured  #request.args.get("var")
    print(session["tmp"], "not saved")
    temp_range = ["<18", "18-25", ">25"]
    led_range = ["easter", "bingbong", "fire", "party", "america"]
    if request.method == "POST":
        global temp
        global user_led
        temp = request.form["temp"]
        user_led = request.form["led"]
        return redirect(url_for("led_config"))
    else:
        return render_template("lights.html", user_t=session["tmp"], tr=temp_range, lr=led_range)

@app.route("/lights", methods = ["POST", "GET"])
def lights():
    if request.method == "POST":
        global temp
        global user_led
        temp = 60
        user_led = request.form["led"]
        return redirect(url_for("led_config"))
    else:
        return render_template("lights.html")

@app.route("/led_config", methods = ["POST", "GET"])
def led_config():
    global led1
    global led2
    global led3
    if temp == "<18":
        led1 = user_led
    elif temp == "18-25":
        led2 = user_led
    else:
        led3 = user_led
    return "{},{},{},".format(led1, led2, led3)
