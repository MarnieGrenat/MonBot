const  char webpage[] = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel = "icon" href ="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAACXBIWXMAAC4jAAAuIwF4pT92AAAAG3RFWHRTb2Z0d2FyZQBDZWxzeXMgU3R1ZGlvIFRvb2zBp+F8AAAEB0lEQVRYw82XX2yTVRTAf19btq7bgsyxuo1dB1lLYgjClIQwnVuikVVjAng1QIwo0UQiMWh8cfEN9WUxJApqJAIPoK4hJEbYSDRjLFEeRthionBBYN0fZrPNza2lK13rA7e1G1//sS3xPPXe8/Wc3733nHvONWKxGLmI2+uzA51A7RzVJaBeSRHKxZ5xHwBrgd4U6nVKit7FBngU6EmhXq+k6Jk3gNvrcwM/AkHgOSXFoJ63AC8DJ1LY2wl8p6SI6u8rtJ1i4HklxZWMAG6vrwY4B1TqKQV4gC3AXkBkWJQP+Bw4CZwBVuv5QaBBSXEtJYDb61ulnVfNMRoFLOQmZv/p1xDX7wFwe30GcAVwsbhyDVgdP6ZkACswDJQuMsAY4FRSRMyOoA5oB4oWyfkU4FFSdKULwmc1xGKIR0nRlikL3gNaAAqsBhvL7Dhsxn15C0ZiXPCHuD2T8PG+kqIlXRZYgBuAKHdYOd7gZEWhbV5LHgxE2HnOz1AwEs+ClUqKmVQATwBdAPsfK+GlVQsTCt4bUzR3j8WHTykpzicAXK19XwPb9dgG5AMcb3CyYXn+rJUcuzpJbWk+m1c4TB21DwS5ODLNLlcxlUk71z0yzY6Ov+LDMHBH//7ecLX2zZhdMicanDyeBLC7y0/XcAgD+Kmpgqqi2UczEIjwdNsQ0RjUOe0cqS9L6C6OTLP9P4BkiRmu1r4oYGQC2PvLCGcHgyyxGPzsqeChAuus7/23Z2g8M8SdaIxnKgs4uGl5NgBkDTARjnLqZoC1JXnUluabGrs0Ok3vaJgt1YUszbMsLECytPUHWVOSR9dwiEj0bhCX2q14qsxjIxPAGLAsF4APuse4PB7mj/Ew8RTfVl3IJxsezBVgwnC19tXpcmvo0rk1E8BQMMKOjkRuU+6w8m2jkwqHLRuAU8BlIAa0zb0HKnU9t6QDAAhEonTeChEDGsrtFNpSV+skgCjwsJJiIN1VfBrwHK0vY5PTviAX0a/+EK92+gHalRRNmWrBk8D5V2qK+XD9sgUB+Kjnb45dnUQ3I52ZasFh4DWLAW8/spQXq4tMi1FxnmVW6vwTjpoWo5M3p/js9wl0shwFdsebETOAQ8Bb2azqwMbSRNq19Qd558JIthvyhZJij1lLtgQIZdv7VTpstG8uB6Dp7C0GApFcesUCJUXYbAc+BfZla+ndNQ9gMaDlt/FcQuKAkmJfqiMwgIMmx/AD0Kj7+4QU2u7GwVTknvOfBDqAF+bMfwnsUVLE0mWBAXwFvKGnPlZSNLu9vhLgdf34WJdidT360fKNkmLU7fXtB5q17jDwZrLzdC8ji96FoJLiyHyeZm6vb5ducg8lR///+22YAWAlcD2FukZJ8Wcu9v4FpQnneXOCDkoAAAAASUVORK5CYII="
  type = "image/x-icon">
  <title>MonBot</title>
  <style>
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #282c35;
      color: #fff;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
    }
    #loadingImage {
      transform: rotate(0deg);
      animation: rotate 1.7s infinite;
    }
    @keyframes rotate {
      0% {
        transform: rotate(0deg);
      }
      100% {
        transform: rotate(360deg);
      }
    }

    header {
      position: fixed;
      top: 0;
      background-color: #25AAE1;
      padding: 20px;
      text-align: center;
      border-radius: 0 0 12px 12px;
    }

    h1 {
      color: #fff;
      margin: 0;
    }

    .container {
      width: 80%;
      background-color: #fff;
      border-radius: 12px;
      box-shadow: 0 0 60px rgba(0, 0, 0, 0.6);
      overflow: hidden;
      margin-top: 20px;
      flex: 1;
      display: flex;
      flex-direction: column;
      position: relative;
      max-height: 70vh;
      /* Define uma altura máxima para o container */
      align-items: center;
    }

    .tabs {
      display: flex;
      background-color: #25AAE1;
      border-radius: 12px 12px 0 0;
      overflow: hidden;
      position: absolute;
      width: 100%;
      top: 0;
    }

    .tab {
      flex: 1;
      padding: 15px;
      text-align: center;
      cursor: pointer;
      color: #fff;
      font-weight: bold;
      transition: background-color 0.3s ease;
    }

    .tab:hover {
      background-color: #1C84AE;
    }

    #liveDataTab {
      background-color: #1C84AE;
    }

    .terminal {
      width: 100%;
      border-radius: 3px;
      border: 2px solid #3d3d3d;
      background-color: #f2f2f2;
      opacity: 0.9;
      height: 340px;
      word-wrap: break-word;
      border-bottom-left-radius: 5px;
      border-bottom-right-radius: 5px;
      overflow-y: scroll;

    }

    .terminal p {
      color: #000000;
      font-weight: 200;
      font-size: 14px;
    }

    .terminal div {
      display: inline-block;
    }

    .terminal .static {
      color: #5ed7ff;
      font-weight: bold;
    }

    #terminalText {
      color: #000;
      /* Define a cor do texto como preto */
    }

    #settingsTab {
      background-color: #25AAE1;
    }

    .tab-content {
      flex: 1;
      overflow-y: auto;
      /* Alterado para auto para permitir rolagem quando necessário */
      padding: 20px;
      background-color: #282c35;
      border-radius: 0 0 12px 12px;
      position: absolute;
      width: 100%;
      bottom: 0;
      top: 50px;
      transition: top 0.3s ease;
    }

    .tab-content.up {
      top: 50px;
    }

    p {
      margin: 0 0 10px;
      line-height: 1.5;
      color: #fff;
    }

    form {
      display: flex;
      justify-content: space-between;
      flex-wrap: wrap;
      max-width: 800px;
      /* Ajuste conforme necessário */
      margin: 0 auto;
    }

    .settings-container {
      width: 800px;
      height: 190px;
      padding: 35px 15px 5px;
    }

    .settings-container:before,
    .settings-container:after {
      content: "";
      display: table;
      clear: both;
    }

    .settings-container div {
      float: left;
      width: 180px;
      height: 160px;
    }

    #mqtt-box {
      margin-left: 20px;
      margin-right: 20px;
    }

    .config-form {
      width: calc(50% - 10px);
      box-sizing: border-box;
      margin-right: 10px;
    }

    label {
      display: block;
      margin-bottom: 5px;
      font-weight: bold;
    }

    input {
      width: calc(100% - 10px);
      padding: 8px;
      margin-bottom: 10px;
      box-sizing: border-box;
    }

    button {
      background-color: #25AAE1;
      color: #fff;
      padding: 10px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      width: 100%;
      margin-top: 10px;
    }

    footer {
      text-align: center;
      padding: 10px;
      background-color: #363b48;
      color: #fff;
      border-radius: 12px;
      width: 100%;
      position: fixed;
      bottom: 0;
    }
  </style>
</head>

<body>
  <section id="loadingScreen" style="display:flex">
    <img id="loadingImage" src="data:image/png;base64,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"
    type = "image/png">
  </section>

  <section id="mainScreen" style="display:none">
    <header>
      <h1>MonBot Dashboard</h1>
    </header>

    <div class="container">
      <div class="tabs">
        <div id="liveDataTab" class="tab">Live Data</div>
        <div id="settingsTab" class="tab">MonBot Settings</div>
      </div>

      <div id="liveDataContent" class="tab-content">
        <h2 style="margin: 0; padding: 20px; background-color: #25AAE1; color: #fff;">Live Sensor Data</h2>
        <div id="messages">
          <section id=connModeContainer style="margin:10px;border:.2px ">
            <section class=grid-container-1-div style=margin:8px>
              <h2 class=text>Terminal</h2>
              <div class=grid-item>
                <div class=container-flex>
                  <div id="terminal" class="terminal">
                    <div id="terminalText"></div>
                  </div>
                  <br>
                  <button id=exportLog
                    style="padding:8px 16px;border:0;background:#25AAE1;color:#f2f2f2;letter-spacing:.03em;border-radius:3px"
                    onclick=exportLog()>Exportar Log</button>
                  <br>
                </div>
              </div>
            </section>
          </section>
        </div>
      </div>

      <div id="settingsContent" class="tab-content" style="display: none;">
        <h2 style="margin: 0; padding: 20px; background-color: #25AAE1; color: #fff;;">MonBot Device Settings</h2>
        <br>
        <div class="settings-container">
          <div id="mqtt-box">
            <h3> MQTT Settings</h3>
            <form id="configForm" onsubmit="saveConfig(); return false;">
              <label for="mqttServer">MQTT Server:</label>
              <input type="text" id="mqttServer" name="mqttServer", placeholder="mqtt.broker.io" required>

              <label for="mqttPort">MQTT Port:</label>
              <input type="text" id="mqttPort" name="mqttPort", placeholder="1883" required>

              <label for="mqttUsername">MQTT Username:</label>
              <input type="text" id="mqttUsername" name="mqttUsername", placeholder="Username" required>

              <label for="mqttPassword">MQTT Password:</label>
              <input type="password" id="mqttPassword" name="mqttPassword", placeholder="********" required>
              <button type="submit">Save MQTT Settings</button>
            </form>
          </div>

          <div id="wifi-box">
            <h3> Wi-Fi Settings</h3>
            <form id="wifiConfigForm" onsubmit="saveWiFiConfig(); return false;">
              <label for="wifiSSID">Wi-Fi SSID:</label>
              <input type="text" id="wifiSSID" name="wifiSSID", placeholder="SSID" required>

              <label for="wifiPassword">Wi-Fi Password:</label>
              <input type="password" id="wifiPassword" name="wifiPassword", placeholder="********" required>

              <label for="apSSID">Access Point SSID:</label>
              <input type="text" id="apSSID" name="apSSID", placeholder="MonBot_AP" required>

              <label for="apPassword">Access Point Password:</label>
              <input type="password" id="apPassword" name="apPassword", placeholder="********" required>

              <button type="submit">Save Wi-Fi Settings</button>
            </form>
          </div>
          <div id="api-box">
            <h3> Message Settings</h3>
            <form id="apiConfigForm" onsubmit="saveAPIConfig(); return false;">
              <label for="apiKey">API Key:</label>
              <input type="text" id="apiKey" name="apiKey", placeholder="0000000" required>

              <label for="phoneNumber">Phone Number:</label>
              <input type="tel" id="phoneNumber" name="phoneNumber", placeholder="(55) 55 9 9999-9999" required>
              <button type="submit">Save API Settings</button>
            </form>
          </div>
        </div>
      </div>
    </div>

    <footer>
      <p>Developer: Gabriela Dellamora Paim. Version:1.0.9</p>
    </footer>
  </section>

  <script>
    var messagesContainer = document.getElementById('messages');
    var liveDataTab = document.getElementById('liveDataTab');
    var settingsTab = document.getElementById('settingsTab');
    var liveDataContent = document.getElementById('liveDataContent');
    var settingsContent = document.getElementById('settingsContent');
    let exportButton = document.getElementById("exportLog");
    let autoScroll = true;

    liveDataTab.addEventListener('click', function () {
      liveDataContent.style.display = 'block';
      settingsContent.style.display = 'none';
      liveDataTab.style.backgroundColor = '#1C84AE';
      settingsTab.style.backgroundColor = '#25AAE1';
      liveDataContent.classList.add('up');
    });

    settingsTab.addEventListener('click', function () {
      liveDataContent.style.display = 'none';
      settingsContent.style.display = 'block';
      liveDataTab.style.backgroundColor = '#25AAE1';
      settingsTab.style.backgroundColor = '#1C84AE';
      liveDataContent.classList.remove('up');
    });

    // Terminal
    let dataReceived = []; // Armazena log

    async function loadScreen() {
      await getDeviceSettings();

      document.getElementById("loadingScreen").style.display = "none";
      document.getElementById("mainScreen").style.display = "flex";
    }

    async function getDeviceSettings() {
      await showAPIConfig();
      await delay(100);
      await showWifiConfig();
      await delay(100);
      await showMQTTConfig();
      await delay(100);
    }

    function delay(ms) {
      return new Promise(resolve => setTimeout(resolve, ms));
    }

    loadScreen();

    function requireData() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/getData", true);
      xhr.setRequestHeader("Accept", "text/plain");
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          showDataOnTerminal(xhr.responseText);
        }
      }
      xhr.send();
    }

    function showDataOnTerminal(output) {
      // Let output= output+"\n";  //Vou dar +="\n"  dentro da função de log do ESP
      dataReceived.push(output);
      document.getElementById('terminalText').innerText = dataReceived.join('');
      if (autoScroll) {
        scrollDown();
      }
    }

    document.getElementById("terminal").onscroll = function () {
      const scrollingElement = this;
      // Se a posição de rolagem atual estiver próxima do final da página, pare de rolar
      autoScroll = (scrollingElement.scrollTop + scrollingElement.offsetHeight >= scrollingElement.scrollHeight - 10);
    }

    exportLog = function () {
      let now = new Date();
      let year = now.getFullYear();
      let month = String(now.getMonth() + 1).padStart(2, '0');
      let day = String(now.getDate()).padStart(2, '0');
      let hour = String(now.getHours()).padStart(2, '0');
      let minute = String(now.getMinutes()).padStart(2, '0');
      let second = String(now.getSeconds()).padStart(2, '0');

      let fileName = "Data_" + year + "-" + month + "-" + day + "_" + hour + "-" + minute + "-" + second;

      let list = dataReceived.join('');
      const blob = new Blob([list], { type: 'text/plain' });
      const link = document.createElement('a');
      link.href = URL.createObjectURL(blob);
      link.download = fileName;
      link.click();
    }

    exportButton.addEventListener('click', exportLog);

    simulation = function () {
      setInterval(function () {
        showDataOnTerminal('[Sensor/Temp]: Temperature: 25°C\n');
      }, 1000);
    }
    // simulation();

    /* Configurations */
    function postRequest(request, data) {
      var xhr = new XMLHttpRequest();
      xhr.open("POST", request, true);
      xhr.setRequestHeader("Content-Type", "text/plain");
      xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
          alert("Configuração Salva com Sucesso! Dispositivo reiniciando...")
        } else if (xhr.readyState === 4 && xhr.status != 200) {
          alert("Erro ao Salvar Configuração. Response:\n" + xhr.responseText);
        }
      };
      xhr.send(JSON.stringify(data));
    }

    function showMQTTConfig() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/getMqttConfig", true);
      xhr.setRequestHeader("Accept", "application/json");
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          var mqttConfig = JSON.parse(xhr.responseText);
          document.getElementById('mqttServer').value = mqttConfig.mqttServer;
          document.getElementById('mqttPort').value = mqttConfig.mqttPort;
          document.getElementById('mqttUsername').value = mqttConfig.mqttUsername;
          document.getElementById('mqttPassword').value = mqttConfig.mqttPassword;
        }
      }
      xhr.send();
    }
    function saveMQTTConfig() {
      mqttConfig = {
        "mqttServer": document.getElementById('mqttServer').value,
        "mqttPort": document.getElementById('mqttPort').value,
        "mqttUsername": document.getElementById('mqttUsername').value,
        "mqttPassword": document.getElementById('mqttPassword').value
      }
      postRequest("/setMqttConfig", mqttConfig);
    }

    function showWifiConfig() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/getWiFiConfig", true);
      xhr.setRequestHeader("Accept", "application/json");
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          var wifiConfig = JSON.parse(xhr.responseText);
          document.getElementById('wifiSSID').value = wifiConfig.wifiSSID;
          document.getElementById('wifiPassword').value = wifiConfig.wifiPassword;
          document.getElementById('apSSID').value = wifiConfig.apSSID;
          document.getElementById('apPassword').value = wifiConfig.apPassword;
        }
      }
      xhr.send();
    }

    function saveWiFiConfig() {

      wifiConfig = {
        "wifiSSID": document.getElementById('wifiSSID').value,
        "wifiPassword": document.getElementById('wifiPassword').value,
        "apSSID": document.getElementById('apSSID').value,
        "apPassword": document.getElementById('apPassword').value
      }
      postRequest("/setWiFiConfig", wifiConfig);
    }

    function showAPIConfig() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/getAPIConfig", true);
      xhr.setRequestHeader("Accept", "application/json");
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          var apiConfig = JSON.parse(xhr.responseText);
          document.getElementById('apiKey').value = apiConfig.apiKey;
          document.getElementById('phoneNumber').value = apiConfig.phoneNumber;
        }
      }
      xhr.send();
    }
    function saveAPIConfig() {
      apiConfig = {
        "apiKey": document.getElementById('apiKey').value,
        "phoneNumber": document.getElementById('phoneNumber').value
      }
      postRequest("/setAPIConfig", apiConfig);
    }
    function showAPIConfig()
    {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/getAPIConfig", true);
      xhr.setRequestHeader("Accept", "application/json");
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          var apiConfig = JSON.parse(xhr.responseText);
          document.getElementById('apiKey').value = apiConfig.apiKey;
          document.getElementById('phoneNumber').value = apiConfig.phoneNumber;
        }
      }
      xhr.send();
    }
  </script>

</body>

</html>
)=====";