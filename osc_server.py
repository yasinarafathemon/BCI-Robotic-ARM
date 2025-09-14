from pythonosc import dispatcher, osc_server

# -----------------------------
# 1) Define EEG Data Handler
# -----------------------------
def eeg_handler(address, *args):
    """ Extract RAW_TP9 from incoming EEG data """
    if address == "/muse/eeg":  # Ensure it's EEG data
        raw_tp9 = args[0]  # First value corresponds to RAW_TP9
        print(f"RAW_TP9: {raw_tp9}")

# -----------------------------
# 2) Set Up OSC Server
# -----------------------------
ip = "192.168.0.107"  # Set this to your local PC's IP from ipconfig
port = 5000  # This must match the Mind Monitor settings

# Create dispatcher (to handle incoming data)
dispatch = dispatcher.Dispatcher()
dispatch.set_default_handler(eeg_handler)  # Print all incoming OSC messages

# Start OSC server
server = osc_server.ThreadingOSCUDPServer((ip, port), dispatch)
print(f"Listening for OSC data on {ip}:{port}...")

# Start server loop
server.serve_forever()
