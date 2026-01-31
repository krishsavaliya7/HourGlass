/**
 * Main Application Logic
 * Coordinates all modules and handles updates
 */

class App {
    constructor() {
        this.updateInterval = null;
        this.autoRefresh = true;
        this.refreshRate = 500; // 500ms = 2 FPS for 9600 baud compatibility
        this.init();
    }

    /**
     * Initialize application
     */
    init() {
        // Ensure DOM is ready before setting up
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', () => {
                this.setupEventListeners();
                this.loadSettings();
                // Show test pattern to verify display is working
                if (display && display.showTestPattern) {
                    display.showTestPattern();
                }
                this.startAutoRefresh();
                this.updateStatus();
            });
        } else {
            this.setupEventListeners();
            this.loadSettings();
            // Show test pattern to verify display is working
            if (display && display.showTestPattern) {
                display.showTestPattern();
            }
            this.startAutoRefresh();
            this.updateStatus();
        }
    }

    /**
     * Setup event listeners
     */
    setupEventListeners() {
        // Wait for DOM to be ready
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', () => this.attachListeners());
            return;
        }
        this.attachListeners();
    }
    
    /**
     * Attach event listeners to DOM elements
     */
    attachListeners() {
        // Connect/Disconnect buttons
        const connectBtn = document.getElementById('btn-connect');
        const disconnectBtn = document.getElementById('btn-disconnect');
        
        if (connectBtn) {
            connectBtn.addEventListener('click', () => this.connectDevice());
        }
        
        if (disconnectBtn) {
            disconnectBtn.addEventListener('click', () => this.disconnectDevice());
        }

        // Setup serial data handler
        serialConnection.onData((data) => {
            api.handleSerialData(data);
        });

        serialConnection.onDisconnect(() => {
            this.handleDisconnect();
        });

        // Brightness slider
        const brightnessSlider = document.getElementById('brightness-slider');
        if (brightnessSlider) {
            brightnessSlider.addEventListener('input', (e) => {
                const value = e.target.value;
                document.getElementById('brightness-value').textContent = value;
                if (api.isConnected()) {
                    this.setBrightness(value);
                }
            });
        }

        // Auto refresh toggle
        const autoRefreshToggle = document.getElementById('auto-refresh');
        if (autoRefreshToggle) {
            autoRefreshToggle.addEventListener('change', (e) => {
                this.autoRefresh = e.target.checked;
                
                // Update visual indicator
                const refreshStatus = document.getElementById('refresh-status');
                if (refreshStatus) {
                    if (this.autoRefresh) {
                        refreshStatus.textContent = '● LIVE';
                        refreshStatus.style.color = '#00ff88';
                    } else {
                        refreshStatus.textContent = '○ PAUSED';
                        refreshStatus.style.color = '#ff6b6b';
                    }
                }
                
                if (this.autoRefresh && api.isConnected()) {
                    this.startAutoRefresh();
                } else {
                    this.stopAutoRefresh();
                }
            });
        }

        // Listen for status updates from serial
        window.addEventListener('statusUpdate', (e) => {
            this.updateUI(e.detail);
        });
        
        // Initialize mode manager
        if (typeof modeManager !== 'undefined') {
            // Mode manager already initialized
        }

        // Disable all controls initially
        this.setControlsEnabled(false);
    }

    /**
     * Connect to device
     */
    async connectDevice() {
        try {
            await serialConnection.connect();
            this.setConnectionStatus(true);
            this.setControlsEnabled(true);
            document.getElementById('btn-connect').style.display = 'none';
            document.getElementById('btn-disconnect').style.display = 'inline-block';
            
            // Update refresh indicator
            const refreshStatus = document.getElementById('refresh-status');
            if (refreshStatus) {
                if (this.autoRefresh) {
                    refreshStatus.textContent = '● LIVE';
                    refreshStatus.style.color = '#00ff88';
                } else {
                    refreshStatus.textContent = '○ PAUSED';
                    refreshStatus.style.color = '#ff6b6b';
                }
            }
            
            // Start auto-refresh if enabled
            if (this.autoRefresh) {
                this.startAutoRefresh();
            }
            
            // Get initial status
            this.updateStatus();
        } catch (error) {
            alert(`Failed to connect: ${error.message}

Make sure:
- Device is connected via USB
- Using Chrome/Edge browser
- Device is powered on`);
            console.error('Connection error:', error);
        }
    }

    /**
     * Disconnect from device
     */
    async disconnectDevice() {
        try {
            await serialConnection.disconnect();
            this.handleDisconnect();
        } catch (error) {
            console.error('Disconnect error:', error);
        }
    }

    /**
     * Handle disconnect
     */
    handleDisconnect() {
        this.setConnectionStatus(false);
        this.setControlsEnabled(false);
        this.stopAutoRefresh();
        
        // Update refresh indicator
        const refreshStatus = document.getElementById('refresh-status');
        if (refreshStatus) {
            refreshStatus.textContent = '○ OFFLINE';
            refreshStatus.style.color = '#666';
        }
        
        document.getElementById('btn-connect').style.display = 'inline-block';
        document.getElementById('btn-disconnect').style.display = 'none';
    }

    /**
     * Enable/disable all controls based on connection
     */
    setControlsEnabled(enabled) {
        // Mode buttons
        document.querySelectorAll('.mode-btn').forEach(btn => {
            btn.disabled = !enabled;
        });

        // All action buttons
        document.querySelectorAll('.action-btn').forEach(btn => {
            if (btn.id !== 'btn-connect' && btn.id !== 'btn-disconnect') {
                btn.disabled = !enabled;
            }
        });

        // Input fields
        document.querySelectorAll('input[type="number"]').forEach(input => {
            input.disabled = !enabled;
        });

        // Brightness slider
        const brightnessSlider = document.getElementById('brightness-slider');
        if (brightnessSlider) {
            brightnessSlider.disabled = !enabled;
        }

        // Add visual feedback
        const controlsSection = document.querySelector('.controls-section');
        if (controlsSection) {
            controlsSection.style.opacity = enabled ? '1' : '0.5';
            controlsSection.style.pointerEvents = enabled ? 'auto' : 'none';
        }
    }

    /**
     * Load settings from localStorage
     */
    loadSettings() {
        const savedBrightness = localStorage.getItem('brightness');
        if (savedBrightness) {
            const slider = document.getElementById('brightness-slider');
            const valueDisplay = document.getElementById('brightness-value');
            if (slider) {
                slider.value = savedBrightness;
            }
            if (valueDisplay) {
                valueDisplay.textContent = savedBrightness;
            }
        }
    }

    /**
     * Start auto-refresh
     */
    startAutoRefresh() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
        }

        if (this.autoRefresh && api.isConnected()) {
            this.updateInterval = setInterval(() => {
                if (api.isConnected()) {
                    this.updateStatus();
                } else {
                    this.stopAutoRefresh();
                }
            }, this.refreshRate);
        }
    }

    /**
     * Stop auto-refresh
     */
    stopAutoRefresh() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
            this.updateInterval = null;
        }
    }

    /**
     * Update device status
     */
    async updateStatus() {
        if (!api.isConnected()) {
            return;
        }

        try {
            const status = await api.getStatus();
            this.updateUI(status);

            // Update display if available
            try {
                const displayData = await api.getDisplay();
                display.updateFromAPI(displayData);
            } catch (error) {
                console.warn('Failed to update display:', error);
            }

            // Update orientation if available
            try {
                const orientation = await api.getOrientation();
                display.updateOrientation(orientation);
            } catch (error) {
                console.warn('Failed to update orientation:', error);
            }

            this.setConnectionStatus(true);
        } catch (error) {
            console.error('Failed to update status:', error);
            // Don't set offline on error, might be temporary
        }
    }

    /**
     * Update UI with status data
     */
    updateUI(status) {
        if (!status) return;

        // Update mode display (don't call setMode as that would send command)
        if (status.mode && modeManager) {
            modeManager.currentMode = status.mode;
            modeManager.updateUI();
        }

        // Update hourglass progress
        if (status.hourglassProgress !== undefined) {
            modeManager.updateHourglassProgress(status.hourglassProgress);
        }

        // Update dice value
        if (status.diceValue !== undefined && status.mode === 'dice') {
            modeManager.displayDiceValue(status.diceValue);
        }

        // Update flip count
        if (status.flipCount !== undefined) {
            modeManager.updateFlipCount(status.flipCount);
        }

        // Update clock time display
        if (status.time && status.mode === 'clock') {
            const [hours, minutes] = status.time.split(':');
            const hoursInput = document.getElementById('clock-hours');
            const minutesInput = document.getElementById('clock-minutes');
            if (hoursInput) hoursInput.value = parseInt(hours) || 12;
            if (minutesInput) minutesInput.value = parseInt(minutes) || 0;
        }

        // Update last update time
        const lastUpdateEl = document.getElementById('last-update');
        if (lastUpdateEl) {
            lastUpdateEl.textContent = new Date().toLocaleTimeString();
        }
    }

    /**
     * Set connection status
     */
    setConnectionStatus(connected) {
        const statusEl = document.getElementById('connection-status');
        if (statusEl) {
            if (connected) {
                statusEl.textContent = 'Connected';
                statusEl.className = 'status-badge online';
            } else {
                statusEl.textContent = 'Not Connected';
                statusEl.className = 'status-badge offline';
            }
        }
    }

    /**
     * Set display brightness
     */
    async setBrightness(level) {
        localStorage.setItem('brightness', level);
        try {
            await api.setBrightness(parseInt(level));
        } catch (error) {
            console.error('Failed to set brightness:', error);
        }
    }
}

// Export for use in global initialization
const app = new App();

