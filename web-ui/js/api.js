class API {
    constructor() {
        this.pendingRequest = null;
        this.responseTimeout = 3000;
        this.commandQueue = [];
        this.processingQueue = false;
        this.requestId = 0;
    }

    isConnected() {
        return serialConnection.getConnected();
    }

    ensureConnected() {
        if (!this.isConnected()) {
            throw new Error('Device not connected. Please connect via USB cable first.');
        }
    }

    async sendCommand(command, parseJSON = false, priority = false) {
        this.ensureConnected();
        
        return new Promise((resolve, reject) => {
            const item = { command, parseJSON, resolve, reject, timestamp: Date.now(), priority };
            
            if (priority) {
                this.commandQueue = this.commandQueue.filter(c => c.priority);
                this.commandQueue.unshift(item);
            } else {
                this.commandQueue.push(item);
            }
            
            this.processQueue();
        });
    }

    async processQueue() {
        if (this.processingQueue || this.commandQueue.length === 0) return;
        
        this.processingQueue = true;
        
        while (this.commandQueue.length > 0) {
            const { command, parseJSON, resolve, reject, timestamp, priority } = this.commandQueue.shift();
            
            if (!priority && Date.now() - timestamp > 2000) {
                reject(new Error('Command stale'));
                continue;
            }
            
            try {
                const result = await this.executeCommand(command, parseJSON);
                resolve(result);
            } catch (error) {
                reject(error);
            }
            
            await new Promise(r => setTimeout(r, priority ? 10 : 30));
        }
        
        this.processingQueue = false;
    }

    async executeCommand(command, parseJSON = false) {
        return new Promise((resolve, reject) => {
            // Skip non-priority commands if another request is pending
            if (!command.startsWith('SET_') && !command.startsWith('RESET_') && !command.startsWith('ROLL_')) {
                if (this.pendingRequest) {
                    reject(new Error('Skipped - another command in progress'));
                    return;
                }
            }
            
            const execute = () => {
                if (this.pendingRequest) {
                    setTimeout(execute, 5);
                    return;
                }
                
                const timeout = setTimeout(() => {
                    if (this.pendingRequest?.command === command) {
                        console.warn(`Command timeout: ${command}`);
                        this.pendingRequest = null;
                        reject(new Error('Command timeout'));
                    }
                }, this.responseTimeout);
                
                this.pendingRequest = { command, resolve, reject, timeout, parseJSON, id: ++this.requestId };
                
                console.log(`Sending command [${this.requestId}]: ${command}`);
                
                serialConnection.sendCommand(command).catch((error) => {
                    clearTimeout(timeout);
                    this.pendingRequest = null;
                    reject(error);
                });
            };
            
            execute();
        });
    }

    handleSerialData(data) {
        let processed = data.startsWith('STATUS:') ? data.substring(7).trim() : data;
        
        console.log(`Serial received: ${processed}`);
        
        // Handle JSON status broadcasts
        if (processed.startsWith('{')) {
            try {
                const jsonData = JSON.parse(processed);
                window.dispatchEvent(new CustomEvent('statusUpdate', { detail: jsonData }));
            } catch (e) {}
            
            if (this.pendingRequest?.parseJSON) {
                const handler = this.pendingRequest;
                clearTimeout(handler.timeout);
                this.pendingRequest = null;
                try {
                    handler.resolve(JSON.parse(processed));
                } catch (e) {
                    handler.resolve(processed);
                }
            }
            return;
        }
        
        // Handle OK/ERR responses
        if (this.pendingRequest) {
            const handler = this.pendingRequest;
            
            if (processed.startsWith('OK')) {
                console.log(`Command [${handler.id}] succeeded: ${processed}`);
                clearTimeout(handler.timeout);
                this.pendingRequest = null;
                handler.resolve(processed);
                return;
            }
            
            if (processed.startsWith('ERR')) {
                console.error(`Command [${handler.id}] failed: ${processed}`);
                clearTimeout(handler.timeout);
                this.pendingRequest = null;
                handler.reject(new Error(processed.substring(4).trim() || 'Unknown error'));
            }
        }
    }

    // High-priority commands (user-initiated)
    async setMode(mode) {
        return await this.sendCommand(`SET_MODE ${mode}`, false, true);
    }

    async setClockTime(hours, minutes) {
        return await this.sendCommand(`SET_TIME ${hours} ${minutes}`, false, true);
    }

    async setHourglassDuration(hours, minutes) {
        return await this.sendCommand(`SET_HG ${hours} ${minutes}`, false, true);
    }

    async resetHourglass() {
        return await this.sendCommand('RESET_HG', false, true);
    }

    async rollDice() {
        const r = await this.sendCommand('ROLL_DICE', true, true);
        if (typeof r === 'string') {
            try {
                return JSON.parse(r);
            } catch (e) {
                return {value: 1, diceValue: 1, diceA: 1, diceB: 1};
            }
        }
        return r;
    }

    async getFlipCount() {
        return this._parseResponse(await this.sendCommand('GET_FLIP_COUNT', true));
    }

    async resetFlipCounter() {
        return await this.sendCommand('RESET_FLIP', false, true);
    }

    async setBrightness(level) {
        return await this.sendCommand(`SET_BRIGHTNESS ${level}`, false, true);
    }

    // Low-priority commands (polling)
    async getStatus() {
        return this._parseResponse(await this.sendCommand('GET_STATUS', true));
    }

    async getOrientation() {
        return this._parseResponse(await this.sendCommand('GET_ORIENTATION', true));
    }

    async getDisplay() {
        return this._parseResponse(await this.sendCommand('GET_DISPLAY', true));
    }

    async getAll() {
        return this._parseResponse(await this.sendCommand('GET_ALL', true));
    }

    _parseResponse(r) {
        return typeof r === 'string' ? JSON.parse(r) : r;
    }
}

const api = new API();
