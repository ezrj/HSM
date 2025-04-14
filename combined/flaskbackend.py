from flask import Flask, jsonify
import serial
import threading

app = Flask(__name__)
entropy_lock = threading.Lock()
latest_entropy = None


SERIAL_PORT = '/dev/cu.usbmodem1101'
BAUD_RATE = 115200

def read_entropy():
    global latest_entropy
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
        while True:
            line = ser.readline().decode(errors='ignore').strip()
            if line.startswith("ENTROPY:"):
                value = line.split("ENTROPY:")[1].strip()
                with entropy_lock:
                    latest_entropy = value

@app.route("/entropy", methods=["GET"])
def get_entropy():
    with entropy_lock:
        if latest_entropy:
            return jsonify({"entropy": latest_entropy})
        return jsonify({"error": "No entropy available"}), 503

if __name__ == "__main__":
    threading.Thread(target=read_entropy, daemon=True).start()
    app.run(host="0.0.0.0", port=5050)
