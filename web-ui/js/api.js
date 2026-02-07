class API {
    constructor() {
<<<<<<< HEAD
        this.pendingRequest = null;
        this.responseTimeout = 3000;
        this.commandQueue = [];
        this.processingQueue = false;
        this.requestId = 0;
=======
        this.pendingRequests = new Map();
        this.responseTimeout = 5000;
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
    }

    isConnected() {
        return serialConnection.getConnected();
    }

    ensureConnected() {
        if (!this.isConnected()) {
            throw new Error('Device not connected. Please connect via USB cable first.');
        }
    }

<<<<<<< HEAD
    async sendCommand(command, parseJSON = false, priority = false) {
        this.ensureConnected();
        
        return new Promise((resolve, reject) => {
            const item = {
                command,
                parseJSON,
                resolve,
                reject,
                timestamp: Date.now(),
                priority
            };
            
            if (priority) {
                // Clear non-priority commands from queue to reduce delay
                this.commandQueue = this.commandQueue.filter(c => c.priority);
                // Add to front of queue
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
            
            // Skip old non-priority commands (stale data)
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
            
            // Minimal delay - only 10ms for priority commands, 30ms for others
            await new Promise(r => setTimeout(r, priority ? 10 : 30));
        }
        
        this.processingQueue = false;
    }

    async executeCommand(command, parseJSON = false) {
        return new Promise((resolve, reject) => {
            // For non-priority commands (like GET_ALL), skip if another request is pending
            if (!command.startsWith('SET_') && !command.startsWith('RESET_') && !command.startsWith('ROLL_')) {
                if (this.pendingRequest) {
                    reject(new Error('Skipped - another command in progress'));
                    return;
                }
            }
            
            // Wait if there's already a pending request
            const waitAndExecute = () => {
                if (this.pendingRequest) {
                    setTimeout(waitAndExecute, 5);
                    return;
                }
                
                const timeout = setTimeout(() => {
                    if (this.pendingRequest && this.pendingRequest.command === command) {
                        console.warn(`Command timeout: ${command}`);
                        this.pendingRequest = null;
                        reject(new Error('Command timeout'));
                    }
                }, this.responseTimeout);
                
                this.pendingRequest = { 
                    command, 
                    resolve, 
                    reject, 
                    timeout, 
                    parseJSON,
                    id: ++this.requestId
                };
                
                console.log(`Sending command [${this.requestId}]: ${command}`);
                
                serialConnection.sendCommand(command).catch((error) => {
                    clearTimeout(timeout);
                    this.pendingRequest = null;
                    reject(error);
                });
            };
            
            waitAndExecute();
=======
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
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
        });
    }

    handleSerialData(data) {
        let processed = data.startsWith('STATUS:') ? data.substring(7).trim() : data;
<<<<<<< HEAD
        
        console.log(`Serial received: ${processed}`);
        
        // Handle JSON status broadcasts
        if (processed.startsWith('{')) {
            try {
                const jsonData = JSON.parse(processed);
                window.dispatchEvent(new CustomEvent('statusUpdate', { detail: jsonData }));
            } catch (e) {}
            
            // If this JSON is a response to a pending request
            if (this.pendingRequest && this.pendingRequest.parseJSON) {
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
                return;
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
=======
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
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
        if (typeof r === 'string') {
            try {
                return JSON.parse(r);
            } catch (e) {
<<<<<<< HEAD
=======
                // If JSON parsing fails, return a default valid response
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
                return {value: 1, diceValue: 1, diceA: 1, diceB: 1};
            }
        }
        return r;
    }

<<<<<<< HEAD
    async resetFlipCounter() {
        return await this.sendCommand('RESET_FLIP', false, true);
    }

    async setBrightness(level) {
        return await this.sendCommand(`SET_BRIGHTNESS ${level}`, false, true);
    }

    // Low-priority commands (polling)
    async getStatus() {
        const r = await this.sendCommand('GET_STATUS', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

=======
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
    async getFlipCount() {
        const r = await this.sendCommand('GET_FLIP_COUNT', true);
        return typeof r === 'string' ? JSON.parse(r) : r;
    }

<<<<<<< HEAD
=======
    async resetFlipCounter() {
        return await this.sendCommand('RESET_FLIP');
    }

>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
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
<<<<<<< HEAD
=======

    async setBrightness(level) {
        return await this.sendCommand(`SET_BRIGHTNESS ${level}`);
    }
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
}

const api = new API();
