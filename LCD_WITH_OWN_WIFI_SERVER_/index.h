const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Robotic ARM Control</title>
  <style>
    html { font-family: Helvetica; text-align: center; }
    button { height: 40px; width: 120px; margin: 10px; font-size: 18px; border-radius: 5px; color: #FFF; background-color: #359B81; border: none; cursor: pointer; }
    button:hover { background-color: #727473; }
    .footer { position: fixed; left: 0; bottom: 0; width: 100%; background-color: #17202A; color: white; text-align: center; padding: 5px; }
    .btn-container { display: flex; flex-direction: column; align-items: center; gap: 10px; }
  </style>
</head>
<body>
  <h1>Robotic ARM Control</h1>
  <h2>Use the Buttons to Move the Arm</h2>

  <div class="btn-container">
    <button onclick="moveArm('up')">Up</button>
    <div>
      <button onclick="moveArm('left')">Left</button>
      <button onclick="moveArm('right')">Right</button>
    </div>
    <button onclick="moveArm('down')">Down</button>
  </div>

  <button onclick="moveArm('reset')">Reset</button>

  <div class="footer">
    <p>ESP8266 Servo Control Panel</p>
  </div>

  <script>
    function moveArm(direction) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/" + direction, true);
      xhttp.send();
    }
  </script>
</body>
</html>
)rawliteral";
