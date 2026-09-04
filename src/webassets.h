#ifndef WEBASSETS_H
#define WEBASSETS_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Smart Vase Dashboard</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-color: #0f172a;
            --card-bg: rgba(30, 41, 59, 0.7);
            --primary: #10b981;
            --primary-glow: rgba(16, 185, 129, 0.4);
            --water: #3b82f6;
            --water-glow: rgba(59, 130, 246, 0.4);
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
            --danger: #ef4444;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Inter', sans-serif; }
        body { background: var(--bg-color); color: var(--text-main); padding: 20px; display: flex; flex-direction: column; align-items: center; min-height: 100vh; }
        .container { max-width: 600px; width: 100%; }
        
        /* Header */
        header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 30px; margin-top: 10px; }
        h1 { font-size: 1.5rem; font-weight: 700; background: linear-gradient(to right, #10b981, #3b82f6); -webkit-background-clip: text; color: transparent; display: flex; align-items: center; gap: 8px;}
        .battery { display: flex; align-items: center; gap: 8px; font-weight: 600; color: var(--text-muted); }
        .battery-icon { width: 30px; height: 14px; border: 2px solid var(--text-muted); border-radius: 3px; position: relative; padding: 1px; }
        .battery-icon::after { content: ''; position: absolute; right: -4px; top: 3px; width: 2px; height: 4px; background: var(--text-muted); border-radius: 0 2px 2px 0; }
        .battery-level { height: 100%; background: var(--primary); border-radius: 1px; width: 0%; transition: width 0.3s, background-color 0.3s; }
        
        /* Grid */
        .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-bottom: 30px; }
        .card { background: var(--card-bg); backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,0.05); padding: 20px; border-radius: 20px; display: flex; flex-direction: column; justify-content: space-between; transition: transform 0.2s, box-shadow 0.2s; }
        .card:hover { transform: translateY(-2px); box-shadow: 0 10px 25px rgba(0,0,0,0.2); }
        .card-title { font-size: 0.85rem; color: var(--text-muted); text-transform: uppercase; letter-spacing: 1px; margin-bottom: 10px; font-weight: 600; }
        .card-value { font-size: 2rem; font-weight: 700; }
        .card-unit { font-size: 1rem; font-weight: 500; color: var(--text-muted); margin-left: 2px; }
        
        .soil-card { grid-column: span 2; display: flex; flex-direction: row; align-items: center; justify-content: space-between; background: linear-gradient(145deg, rgba(16,185,129,0.1) 0%, rgba(30,41,59,0.7) 100%); border-color: rgba(16,185,129,0.2); }
        .soil-card .card-value { font-size: 3.5rem; color: var(--primary); text-shadow: 0 0 20px var(--primary-glow); }
        
        .alert { color: var(--danger) !important; text-shadow: 0 0 10px rgba(239,68,68,0.4); }
        
        /* Controls */
        .controls { background: var(--card-bg); padding: 25px; border-radius: 20px; border: 1px solid rgba(255,255,255,0.05); margin-bottom: 30px; }
        .slider-container { margin-bottom: 25px; }
        .slider-header { display: flex; justify-content: space-between; margin-bottom: 10px; color: var(--text-muted); font-size: 0.9rem; font-weight: 500; }
        input[type=range] { -webkit-appearance: none; width: 100%; height: 6px; background: rgba(255,255,255,0.1); border-radius: 3px; outline: none; }
        input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 22px; height: 22px; border-radius: 50%; background: var(--water); cursor: pointer; box-shadow: 0 0 15px var(--water-glow); transition: transform 0.1s; }
        input[type=range]::-webkit-slider-thumb:hover { transform: scale(1.1); }
        
        .btn { width: 100%; padding: 16px; border: none; border-radius: 12px; font-size: 1.1rem; font-weight: 600; cursor: pointer; transition: all 0.2s; display: flex; justify-content: center; align-items: center; gap: 10px; }
        .btn-pump { background: var(--water); color: white; box-shadow: 0 4px 15px var(--water-glow); }
        .btn-pump:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(59,130,246,0.6); }
        .btn-pump:active { transform: translateY(0); }
        
        .btn-stop { background: var(--danger); color: white; display: none; box-shadow: 0 4px 15px rgba(239,68,68,0.4); }
        .btn-stop:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(239,68,68,0.6); }
        
        .btn-settings { background: rgba(255,255,255,0.05); color: var(--text-main); margin-top: 15px; border: 1px solid rgba(255,255,255,0.1); }
        .btn-settings:hover { background: rgba(255,255,255,0.1); }
        
        /* Modals */
        .modal-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); backdrop-filter: blur(5px); display: flex; justify-content: center; align-items: center; opacity: 0; pointer-events: none; transition: opacity 0.3s; z-index: 50; }
        .modal-overlay.active { opacity: 1; pointer-events: auto; }
        .modal { background: var(--bg-color); border: 1px solid rgba(255,255,255,0.1); padding: 30px; border-radius: 20px; max-width: 90%; width: 400px; transform: translateY(20px); transition: transform 0.3s; }
        .modal-overlay.active .modal { transform: translateY(0); }
        .modal h3 { margin-bottom: 15px; font-size: 1.4rem; font-weight: 700; }
        .modal p { color: var(--text-muted); margin-bottom: 25px; line-height: 1.5; }
        .modal-actions { display: flex; gap: 10px; }
        .modal-actions .btn { flex: 1; padding: 12px; font-size: 1rem; }
        
        .input-group { margin-bottom: 15px; text-align: left; }
        .input-group label { display: block; font-size: 0.85rem; font-weight: 500; color: var(--text-muted); margin-bottom: 8px; }
        .input-group input { width: 100%; padding: 14px; border-radius: 10px; border: 1px solid rgba(255,255,255,0.15); background: rgba(0,0,0,0.3); color: white; outline: none; font-size: 1rem; transition: border-color 0.2s; }
        .input-group input:focus { border-color: var(--primary); }
        
        .status-dot { display: inline-block; width: 10px; height: 10px; border-radius: 50%; background: var(--danger); transition: background 0.3s, box-shadow 0.3s; }
        .status-dot.online { background: var(--primary); box-shadow: 0 0 10px var(--primary-glow); }
        
        #pump-status { font-size: 0.9rem; font-weight: 500; margin-top: 5px; transition: color 0.3s; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1><span class="status-dot" id="conn-status"></span> Smart Vase</h1>
            <div class="battery">
                <span id="bat-val">--%</span>
                <div class="battery-icon"><div class="battery-level" id="bat-fill"></div></div>
            </div>
        </header>
        
        <div class="grid">
            <div class="card soil-card">
                <div>
                    <div class="card-title">Soil Moisture</div>
                    <div id="pump-status" style="color: var(--text-muted);">Idle</div>
                </div>
                <div><span class="card-value" id="soil">--</span><span class="card-unit">%</span></div>
            </div>
            
            <div class="card">
                <div class="card-title">Temperature</div>
                <div><span class="card-value" id="temp">--</span><span class="card-unit">°C</span></div>
            </div>
            
            <div class="card">
                <div class="card-title">Humidity</div>
                <div><span class="card-value" id="hum">--</span><span class="card-unit">%</span></div>
            </div>
            
            <div class="card">
                <div class="card-title">Light Level</div>
                <div><span class="card-value" id="light">--</span></div>
            </div>
            
            <div class="card">
                <div class="card-title">Reservoir</div>
                <div><span class="card-value" id="water">--</span></div>
            </div>
        </div>
        
        <div class="controls">
            <div class="slider-container">
                <div class="slider-header">
                    <span>Pump Duration</span>
                    <span id="dur-val">5s</span>
                </div>
                <input type="range" id="duration" min="1" max="10" value="5" oninput="document.getElementById('dur-val').innerText = this.value + 's'">
            </div>
            
            <button class="btn btn-pump" id="btn-pump" onclick="requestPump()">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v20M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/></svg>
                Pump Now
            </button>
            <button class="btn btn-stop" id="btn-stop" onclick="stopPump()">Stop Pump</button>
            
            <button class="btn btn-settings" onclick="openSettings()">Network Settings</button>
        </div>
    </div>
    
    <!-- Warning Modal -->
    <div class="modal-overlay" id="warning-modal">
        <div class="modal">
            <h3 id="warning-title" style="color: var(--danger)">Warning</h3>
            <p id="warning-msg">Message here.</p>
            <div class="modal-actions">
                <button class="btn" style="background: rgba(255,255,255,0.1); color: white;" onclick="closeModal('warning-modal')">Cancel</button>
                <button class="btn" style="background: var(--danger); color: white;" onclick="forcePump()">Pump Anyway</button>
            </div>
        </div>
    </div>
    
    <!-- Settings Modal -->
    <div class="modal-overlay" id="settings-modal">
        <div class="modal">
            <h3>Wi-Fi Setup</h3>
            <div class="input-group">
                <label>Network SSID</label>
                <input type="text" id="wifi-ssid" placeholder="Home Network Name">
            </div>
            <div class="input-group">
                <label>Network Password</label>
                <input type="password" id="wifi-pass" placeholder="Network Password">
            </div>
            <div class="input-group">
                <label>Device Admin Password</label>
                <input type="password" id="admin-pass" placeholder="Required for changes">
            </div>
            <div class="modal-actions">
                <button class="btn" style="background: rgba(255,255,255,0.1); color: white;" onclick="closeModal('settings-modal')">Cancel</button>
                <button class="btn btn-pump" onclick="saveSettings()">Save & Reboot</button>
            </div>
        </div>
    </div>
    
    <script>
        let currentData = { soil: 0, water: 1 };
        
        function updateUI(data) {
            currentData = data;
            document.getElementById('conn-status').classList.add('online');
            
            document.getElementById('soil').innerText = data.soil;
            document.getElementById('temp').innerText = parseFloat(data.temp).toFixed(1);
            document.getElementById('hum').innerText = Math.round(data.hum);
            document.getElementById('light').innerText = data.light;
            
            const wEl = document.getElementById('water');
            if(data.water) { 
                wEl.innerText = "OK"; 
                wEl.classList.remove('alert'); 
            } else { 
                wEl.innerText = "EMPTY"; 
                wEl.classList.add('alert'); 
            }
            
            document.getElementById('bat-val').innerText = data.bat + "%";
            const batFill = document.getElementById('bat-fill');
            batFill.style.width = data.bat + "%";
            
            if(data.bat <= 20) { 
                batFill.style.background = "var(--danger)"; 
                document.getElementById('bat-val').style.color = "var(--danger)";
            } else { 
                batFill.style.background = "var(--primary)"; 
                document.getElementById('bat-val').style.color = "var(--text-muted)";
            }
        }
        
        async function fetchSensors() {
            try {
                const res = await fetch('/api/sensors');
                if(res.ok) updateUI(await res.json());
            } catch(e) {
                document.getElementById('conn-status').classList.remove('online');
            }
        }
        
        setInterval(fetchSensors, 5000);
        fetchSensors(); // Initial fetch
        
        function requestPump() {
            if(currentData.water === 0) {
                showWarning("Reservoir Empty", "The water reservoir is empty. Running the pump dry may damage the motor. Please refill first.");
                return;
            }
            if(currentData.soil > 80) {
                showWarning("High Moisture", `Soil moisture is already high (${currentData.soil}%). Pumping now may cause root rot and overwatering.`);
                return;
            }
            forcePump();
        }
        
        function forcePump() {
            closeModal('warning-modal');
            const dur = document.getElementById('duration').value;
            fetch('/api/pump', { 
                method: 'POST', 
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({action: 'start', duration: parseInt(dur)}) 
            });
            
            document.getElementById('btn-pump').style.display = 'none';
            document.getElementById('btn-stop').style.display = 'flex';
            
            const statusText = document.getElementById('pump-status');
            statusText.innerText = "Pumping...";
            statusText.style.color = "var(--water)";
            
            // Auto reset UI after duration
            setTimeout(resetPumpUI, dur * 1000);
        }
        
        function stopPump() {
            fetch('/api/pump', { 
                method: 'POST', 
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({action: 'stop'}) 
            });
            resetPumpUI();
        }
        
        function resetPumpUI() {
            document.getElementById('btn-stop').style.display = 'none';
            document.getElementById('btn-pump').style.display = 'flex';
            const statusText = document.getElementById('pump-status');
            statusText.innerText = "Idle";
            statusText.style.color = "var(--text-muted)";
        }
        
        function showWarning(title, msg) {
            document.getElementById('warning-title').innerText = title;
            document.getElementById('warning-msg').innerText = msg;
            document.getElementById('warning-modal').classList.add('active');
        }
        
        function openSettings() { 
            document.getElementById('settings-modal').classList.add('active'); 
        }
        
        function closeModal(id) { 
            document.getElementById(id).classList.remove('active'); 
        }
        
        function saveSettings() {
            const payload = {
                ssid: document.getElementById('wifi-ssid').value,
                password: document.getElementById('wifi-pass').value,
                admin_pass: document.getElementById('admin-pass').value
            };
            fetch('/api/wifi', { 
                method: 'POST', 
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload) 
            }).then(r => { 
                if(r.ok) { 
                    alert("Settings saved successfully! The device will now reboot."); 
                    closeModal('settings-modal'); 
                } else {
                    alert("Unauthorized or Invalid Input. Did you enter the correct admin password?"); 
                }
            }).catch(e => {
                alert("Failed to reach device. Are you still connected to the network?");
            });
        }
    </script>
</body>
</html>
)rawliteral";

#endif // WEBASSETS_H
