/**
 * API Communication Module
 * Handles serial communication with the device via Web Serial API
 */

class API {
    constructor() {
        this.responseHandlers = new Map();
        this.responseTimeout = 2000;
        this.pendingRequests = new Map();
        this.requestId = 0;
    }

    /**
     * Check if device is connected
     */
    isConnected() {
        return serialConnection.getConnected();
    }

    /**
     * Ensure device is connected before operation
     */
    ensureConnected() {
        if (!this.isConnected()) {
            throw new Error('Device not connected. Please connect via USB cable first.');
        }
    }

    /**
     * Send command and wait for response
     */
    async sendCommand(command, parseJSON = false) {
        this.ensureConnected();
        
        return new Promise((resolve, reject) => {
            const timeout = setTimeout(() => {
                this.pendingRequests.delete(command);
                reject(new Error('Command timeout'));
            }, this.responseTimeout);
            
            this.pendingRequests.set(command, { resolve, reject, timeout, parseJSON });
            
            serialConnection.sendCommand(command).catch((error) => {
                // Clean up on send failure
                clearTimeout(timeout);
                this.pendingRequests.delete(command);
                reject(error);
            });
        });
    }

    /**
     * Handle incoming serial data
     */
    handleSerialData(data) {
        // Match response to the first pending request (FIFO)
        // For more robust matching, responses should include request IDs
        if (this.pendingRequests.size > 0) {
            const firstEntry = this.pendingRequests.entries().next().value;
            if (firstEntry) {
                const [command, handler] = firstEntry;
                
                // Check if response matches command pattern
                if (data.startsWith('OK') || data.startsWith('ERR') || data.startsWith('{')) {
                    clearTimeout(handler.timeout);
                    this.pendingRequests.delete(command);
                    
                    if (data.startsWith('ERR')) {
                        const errMsg = data.substring(4).trim() || 'Unknown error';
                        handler.reject(new Error(errMsg));
                    } else {
                        try {
                            const result = handler.parseJSON ? JSON.parse(data) : data;
                            handler.resolve(result);
                        } catch (e) {
                            handler.resolve(data);
                        }
                    }
                    return;
                }
            }
        }
        
        // If no pending request match, try to parse as JSON status update
        if (data.startsWith('{')) {
            try {
                const status = JSON.parse(data);
                // Emit status update event
                window.dispatchEvent(new CustomEvent('statusUpdate', { detail: status }));
            } catch (e) {
                // Not JSON, ignore
            }
        }
    }

    /**
     * Get device status
     */
    async getStatus() {
        this.ensureConnected();
        const response = await this.sendCommand('GET_STATUS', true);
        return typeof response === 'string' ? JSON.parse(response) : response;
    }

    /**
     * Change operating mode
     */
    async setMode(mode) {
        this.ensureConnected();
        return await this.sendCommand(`SET_MODE ${mode}`);
    }

    /**
     * Set clock time
     */
    async setClockTime(hours, minutes) {
        this.ensureConnected();
        return await this.sendCommand(`SET_TIME ${hours} ${minutes}`);
    }

    /**
     * Set hourglass duration
     */
    async setHourglassDuration(hours, minutes) {
        this.ensureConnected();
        return await this.sendCommand(`SET_HG ${hours} ${minutes}`);
    }

    /**
     * Reset hourglass
     */
    async resetHourglass() {
        this.ensureConnected();
        return await this.sendCommand('RESET_HG');
    }

    /**
     * Roll dice
     */
    async rollDice() {
        this.ensureConnected();
        const response = await this.sendCommand('ROLL_DICE', true);
        return typeof response === 'string' ? JSON.parse(response) : response;
    }

    /**
     * Get flip counter count
     */
    async getFlipCount() {
        this.ensureConnected();
        const response = await this.sendCommand('GET_FLIP_COUNT', true);
        return typeof response === 'string' ? JSON.parse(response) : response;
    }

    /**
     * Reset flip counter
     */
    async resetFlipCounter() {
        this.ensureConnected();
        return await this.sendCommand('RESET_FLIP');
    }

    /**
     * Get current orientation
     */
    async getOrientation() {
        this.ensureConnected();
        const response = await this.sendCommand('GET_ORIENTATION', true);
        return typeof response === 'string' ? JSON.parse(response) : response;
    }

    /**
     * Get LED display state
     */
    async getDisplay() {
        this.ensureConnected();
        const response = await this.sendCommand('GET_DISPLAY', true);
        return typeof response === 'string' ? JSON.parse(response) : response;
    }

    /**
     * Set display brightness
     */
    async setBrightness(level) {
        this.ensureConnected();
        return await this.sendCommand(`SET_BRIGHTNESS ${level}`);
    }
}

// Export singleton instance
const api = new API();

