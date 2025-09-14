import requests
import numpy as np
from scipy.signal import butter, filtfilt, find_peaks
from pythonosc import dispatcher, osc_server
import threading
import time

# -----------------------------
# ESP8266 IP Address
# -----------------------------
ESP_IP = "http://192.168.4.3"  # Replace with your ESP8266 IP [assagined for robotic arm]

# -----------------------------
# EEG Parameters
# -----------------------------
fs = 256  # Sampling rate
window_duration = 4  # Window size in seconds
window_size = fs * window_duration  # Number of samples in 4 seconds

# Buffer for storing EEG data
eeg_buffer = []
processing_flag = False  # Prevent overlapping processing

# -----------------------------
# Bandpass Filter Function
# -----------------------------
def bandpass_filter(signal, fs, lowcut=1, highcut=15, order=4):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return filtfilt(b, a, signal)

# -----------------------------
# Blink Detection and Command Sending
# -----------------------------
def detect_blinks(signal):
    filtered_signal = bandpass_filter(signal, fs)

    # Detect peaks
    lower_peaks, _ = find_peaks(-filtered_signal, height=100, distance=32)  # Negative peaks
    upper_peaks, _ = find_peaks(filtered_signal, height=30, distance=32)   # Positive peaks

    # Identify blinks (valley followed by peak)
    blink_indices = []
    for valley in lower_peaks:
        valid_peaks = [p for p in upper_peaks if valley < p]
        if valid_peaks:
            blink_indices.append((valley, valid_peaks[0]))

    blink_count = len(blink_indices)
    print(f"Detected {blink_count} blinks.")

    # Send command based on blink count
    if blink_count == 1:
        send_command("/left")
    elif blink_count == 2:
        send_command("/right")
    elif blink_count == 3:
        send_command("/up")
    elif blink_count == 4:
        send_command("/down")
    elif blink_count >= 5:
        send_command("/reset")

# -----------------------------
# Send HTTP Command to ESP8266
# -----------------------------
def send_command(endpoint):
    url = f"{ESP_IP}{endpoint}"
    try:
        response = requests.get(url, timeout=20)
        print(f"Sent command: {endpoint}, Response: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Error sending command: {e}")

# -----------------------------
# EEG Data Handler (OSC)
# -----------------------------
def eeg_handler(address, *args):
    global eeg_buffer, processing_flag

    if address == "/muse/eeg" and not processing_flag:
        raw_tp9 = args[0]  # First value corresponds to RAW_TP9
        print (raw_tp9)
        eeg_buffer.append(raw_tp9)

        # Process when buffer reaches 1024 samples (4 sec)
        if len(eeg_buffer) >= window_size:
            processing_flag = True  # Prevent new data collection
            data_to_process = np.array(eeg_buffer[:window_size])  # Copy data to process
            eeg_buffer = eeg_buffer[window_size:]  # Remove processed data

            # Run processing in a separate thread
            processing_thread = threading.Thread(target=process_eeg_data, args=(data_to_process,))
            processing_thread.start()

# -----------------------------
# Processing Function (Runs in a Separate Thread)
# -----------------------------
def process_eeg_data(signal):
    global processing_flag
    print("Processing 4 seconds of EEG data...")
    detect_blinks(signal)  # Run blink detection
    time.sleep(2)  # Ensure the next batch starts after processing
    processing_flag = False  # Allow new data collection

# -----------------------------
# Set Up OSC Server
# -----------------------------
ip = "192.168.4.2"  # Use your local IPv4 address
port = 5000  # Must match Mind Monitor settings

dispatch = dispatcher.Dispatcher()
dispatch.map("/muse/eeg", eeg_handler)  # Handle EEG data

server = osc_server.ThreadingOSCUDPServer((ip, port), dispatch)
print(f"Listening for OSC data on {ip}:{port}...")

# Start server in a separate thread
server_thread = threading.Thread(target=server.serve_forever, daemon=True)
server_thread.start()

# Keep the script running
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Shutting down server...")
