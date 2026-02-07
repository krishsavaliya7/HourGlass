class App {
    constructor() {
        this.updateInterval = null;
        this.autoRefresh = true;
        this.refreshRate = 100;  // 100ms for 10 FPS - good balance of responsiveness and performance
        this.lastUpdateData = null; // Cache last data to avoid redundant updates
        this.init();
    }

    init() {
        const setup = () => {
            this.attachListeners();
            this.loadSettings();
            if (display?.showTestPattern) display.showTestPattern();
            this.startAutoRefresh();
            this.updateStatus();
        };
        document.readyState === 'loading' 
            ? document.addEventListener('DOMContentLoaded', setup) 
            : setup();
    }

    attachListeners() {
        const conn = document.getElementById('btn-connect');
        const disconn = document.getElementById('btn-disconnect');
        if (conn) conn.addEventListener('click', () => this.connectDevice());
        if (disconn) disconn.addEventListener('click', () => this.disconnectDevice());

        serialConnection.onData(data => api.handleSerialData(data));
        serialConnection.onDisconnect(() => this.handleDisconnect());

        const slider = document.getElementById('brightness-slider');
        if (slider) {
            slider.addEventListener('input', (e) => {
                document.getElementById('brightness-value').textContent = e.target.value;
                if (api.isConnected()) this.setBrightness(e.target.value);
            });
        }

        const toggle = document.getElementById('auto-refresh');
        if (toggle) {
            toggle.addEventListener('change', (e) => {
                this.autoRefresh = e.target.checked;
                const status = document.getElementById('refresh-status');
                if (status) {
                    status.textContent = this.autoRefresh ? '● LIVE' : '○ PAUSED';
                    status.style.color = this.autoRefresh ? '#00ff88' : '#ff6b6b';
                }
                this.autoRefresh && api.isConnected() ? this.startAutoRefresh() : this.stopAutoRefresh();
            });
        }

        window.addEventListener('statusUpdate', (e) => this.updateUI(e.detail));
        this.setControlsEnabled(false);
    }

    async connectDevice() {
        try {
            await serialConnection.connect();
            this.setConnectionStatus(true);
            this.setControlsEnabled(true);
            document.getElementById('btn-connect').style.display = 'none';
            document.getElementById('btn-disconnect').style.display = 'inline-block';
            const status = document.getElementById('refresh-status');
            if (status) {
                status.textContent = this.autoRefresh ? '● LIVE' : '○ PAUSED';
                status.style.color = this.autoRefresh ? '#00ff88' : '#ff6b6b';
            }
            if (this.autoRefresh) this.startAutoRefresh();
            this.updateStatus();
        } catch (error) {
            alert(`Failed to connect: ${error.message}\n\nMake sure:\n- Device is connected via USB\n- Using Chrome/Edge browser\n- Device is powered on`);
            console.error('Connection error:', error);
        }
    }

    async disconnectDevice() {
        try {
            await serialConnection.disconnect();
            this.handleDisconnect();
        } catch (error) {
            console.error('Disconnect error:', error);
        }
    }

    handleDisconnect() {
        this.setConnectionStatus(false);
        this.setControlsEnabled(false);
        this.stopAutoRefresh();
        const status = document.getElementById('refresh-status');
        if (status) {
            status.textContent = '○ OFFLINE';
            status.style.color = '#666';
        }
        document.getElementById('btn-connect').style.display = 'inline-block';
        document.getElementById('btn-disconnect').style.display = 'none';
    }

    setControlsEnabled(enabled) {
        document.querySelectorAll('.mode-btn').forEach(btn => btn.disabled = !enabled);
        document.querySelectorAll('.action-btn').forEach(btn => {
            if (btn.id !== 'btn-connect' && btn.id !== 'btn-disconnect') btn.disabled = !enabled;
        });
        document.querySelectorAll('input[type="number"]').forEach(input => input.disabled = !enabled);
        const slider = document.getElementById('brightness-slider');
        if (slider) slider.disabled = !enabled;
        const section = document.querySelector('.controls-section');
        if (section) {
            section.style.opacity = enabled ? '1' : '0.5';
            section.style.pointerEvents = enabled ? 'auto' : 'none';
        }
    }

    loadSettings() {
        const saved = localStorage.getItem('brightness');
        if (saved) {
            const slider = document.getElementById('brightness-slider');
            const displayEl = document.getElementById('brightness-value');
            if (slider) slider.value = saved;
            if (displayEl) displayEl.textContent = saved;
        }
    }

    startAutoRefresh() {
        if (this.updateInterval) clearInterval(this.updateInterval);
        if (this.autoRefresh && api.isConnected()) {
            this.updateInterval = setInterval(() => {
                api.isConnected() ? this.updateStatus() : this.stopAutoRefresh();
            }, this.refreshRate);
        }
    }

    stopAutoRefresh() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
            this.updateInterval = null;
        }
    }

    // Check if data has actually changed to avoid unnecessary DOM updates
    hasDataChanged(newData) {
        if (!this.lastUpdateData) return true;
        
        // Quick reference comparison for nested objects
        const oldStr = JSON.stringify(this.lastUpdateData);
        const newStr = JSON.stringify(newData);
        
        return oldStr !== newStr;
    }

    async updateStatus() {
        if (!api.isConnected()) return;
        try {
            const data = await api.getAll();
            
            // Only update if data changed (reduces unnecessary DOM manipulation)
            if (!this.hasDataChanged(data)) {
                return;
            }
            
            this.lastUpdateData = data;
            
            if (data.status) this.updateUI(data.status);
            if (data.display) display.updateFromAPI(data.display);
            if (data.orientation) display.updateOrientation(data.orientation);
            this.setConnectionStatus(true);
        } catch (error) {
            // Silently ignore errors to keep live updates running
        }
    }

    updateUI(status) {
        if (!status) return;
        if (status.mode && modeManager) {
            modeManager.syncModeFromHardware(status.mode);
        }
        if (status.hourglassProgress !== undefined) {
            modeManager.updateHourglassProgress(status.hourglassProgress);
            if (status.particlesRemaining !== undefined) {
                const el = document.getElementById('hourglass-particles');
                if (el) el.textContent = `${status.particlesRemaining}/60`;
            }
        }
        if (status.diceValue !== undefined && status.mode === 'dice') {
            // Show both dice if available, otherwise show single dice
            if (status.diceA !== undefined && status.diceB !== undefined) {
                modeManager.displayDualDice(status.diceA, status.diceB);
            } else {
                modeManager.displayDiceValue(status.diceValue);
            }
        }
        if (status.flipCount !== undefined && status.mode === 'flipcounter') {
            modeManager.updateFlipCount(status.flipCount);
        }
        if (status.time && status.mode === 'clock') {
            const [h, m] = status.time.split(':');
            const hi = document.getElementById('clock-hours');
            const mi = document.getElementById('clock-minutes');
            if (hi) hi.value = parseInt(h) || 12;
            if (mi) mi.value = parseInt(m) || 0;
        }
        if (status.orientation !== undefined && display) {
            display.updateOrientation({ angle: status.orientation });
        }
        const last = document.getElementById('last-update');
        if (last) last.textContent = new Date().toLocaleTimeString();
    }

    setConnectionStatus(connected) {
        const el = document.getElementById('connection-status');
        if (el) {
            el.textContent = connected ? 'Connected' : 'Not Connected';
            el.className = connected ? 'status-badge online' : 'status-badge offline';
        }
    }

    async setBrightness(level) {
        localStorage.setItem('brightness', level);
        try {
            await api.setBrightness(parseInt(level));
        } catch (error) {
            console.error('Failed to set brightness:', error);
        }
    }
}

const app = new App();
