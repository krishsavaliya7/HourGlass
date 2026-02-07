class API {
    constructor() {
        this.pendingRequests = new Map();
        this.responseTimeout = 5000;
    }

    isConnected() {
        return serialConnection.getConnected();
    }

    ensureConnected() {
        if (!this.isConnected()) {
            throw new Error('Device not connected. Please connect via USB cable first.');
        }
    }

    async sendCommand(command, parseJSON = false) {
        this.ensureConnected();
        return new Promise((resolve, reject) => {
            const timeout = setTimeout(() => {
                this.pendingRequests.delete(command);
                reject(new Error('Command timeout'));
            }, this.responseTimeout);
            this.pendingRequests.set(command, { resolve, reject, timeout, parseJSON });
            serialConnection.sendCommand(command).catch((error) => {
                clearTimeout(timeout);
                this.pendingRequests.delete(command);
                reject(error);
            });
        });
    }

    handleSerialData(data) {
        let processed = data.startsWith('STATUS:') ? data.substring(7).trim() : data;
        if (this.pendingRequests.size > 0) {
            const [command, handler] = this.pendingRequests.entries().next().value;
            if (processed.startsWith('OK') || processed.startsWith('ERR') || processed.startsWith('{')) {
                clearTimeout(handler.timeout);
                this.pendingRequests.delete(command);
                if (processed.startsWith('ERR')) {
                    handler.reject(new Error(processed.substring(4).trim() || 'Unknown error'));
                } else {
                    try {
                        handler.resolve(handler.parseJSON ? JSON.parse(processed) : processed);
                    } catch (e) {
                        handler.resolve(processed);
                    }
                }
                return;
            }
        }
        if (processed.startsWith('{')) {
            try {
                window.dispatchEvent(new CustomEvent('statusUpdate', { detail: JSON.parse(processed) }));
            } catch (e) {}
        }
    }

    async getStatus() {
        const r = await this.sendCommand('GET_STATUS', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

    async setMode(mode) {
        return await this.sendCommand(`SET_MODE ${mode}`);
    }

    async setClockTime(hours, minutes) {
        return await this.sendCommand(`SET_TIME ${hours} ${minutes}`);
    }

    async setHourglassDuration(hours, minutes) {
        return await this.sendCommand(`SET_HG ${hours} ${minutes}`);
    }

    async resetHourglass() {
        return await this.sendCommand('RESET_HG');
    }

    async rollDice() {
        const r = await this.sendCommand('ROLL_DICE', true);
        if (typeof r === 'string') {
            try {
                return JSON.parse(r);
            } catch (e) {
                // If JSON parsing fails, return a default valid response
                return {value: 1, diceValue: 1, diceA: 1, diceB: 1};
            }
        }
        return r;
    }

    async getFlipCount() {
        const r = await this.sendCommand('GET_FLIP_COUNT', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

    async resetFlipCounter() {
        return await this.sendCommand('RESET_FLIP');
    }

    async getOrientation() {
        const r = await this.sendCommand('GET_ORIENTATION', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

    async getDisplay() {
        const r = await this.sendCommand('GET_DISPLAY', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

    async getAll() {
        const r = await this.sendCommand('GET_ALL', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

    async setBrightness(level) {
        return await this.sendCommand(`SET_BRIGHTNESS ${level}`);
    }
}

const api = new API();
