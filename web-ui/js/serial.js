class SerialConnection {
    constructor() {
        this.port = null;
        this.reader = null;
        this.writer = null;
        this.textEncoder = null;
        this.textDecoder = null;
        this.isConnected = false;
        this.readLoopRunning = false;
        this.onDataCallback = null;
        this.onDisconnectCallback = null;
        this.lineBuffer = '';
        this._serialAvailable = 'serial' in navigator;
    }

    isAvailable() {
        return this._serialAvailable;
    }

    async connect() {
        if (!this._serialAvailable) {
            throw new Error('Web Serial API not supported. Use Chrome/Edge.');
        }
        try {
            this.port = await navigator.serial.requestPort();
            await this.port.open({ baudRate: 9600 });
            this.textEncoder = new TextEncoderStream();
            this.textDecoder = new TextDecoderStream();
            this.textEncoder.readable.pipeTo(this.port.writable).catch(() => {});
            this.port.readable.pipeTo(this.textDecoder.writable).catch(() => {});
            this.writer = this.textEncoder.writable.getWriter();
            this.reader = this.textDecoder.readable.getReader();
            this.isConnected = true;
            this.lineBuffer = '';
            this.startReadLoop();
            return true;
        } catch (error) {
            if (error.name === 'NotFoundError') throw new Error('No device selected');
            throw error;
        }
    }

    async disconnect() {
        this.isConnected = false;
        try {
            if (this.textEncoder) {
                try { await this.textEncoder.writable.close(); } catch (e) {}
                this.textEncoder = null;
            }
            if (this.writer) {
                try { await this.writer.close(); } catch (e) {}
                this.writer = null;
            }
            if (this.reader) {
                try { 
                    await this.reader.cancel(); 
                    await this.reader.releaseLock(); 
                } catch (e) {}
                this.reader = null;
            }
            this.textDecoder = null;
            if (this.port) {
                try { await this.port.close(); } catch (e) {}
                this.port = null;
            }
        } catch (error) {
            console.error('Error disconnecting:', error);
        }
        this.onDisconnectCallback?.();
    }

    async startReadLoop() {
        if (this.readLoopRunning) return;
        this.readLoopRunning = true;
        while (this.isConnected && this.reader) {
            try {
                const { value, done } = await this.reader.read();
                if (done) break;
                if (value) {
                    this.lineBuffer += value;
                    let newlineIndex;
                    while ((newlineIndex = this.lineBuffer.indexOf('\n')) !== -1) {
                        const line = this.lineBuffer.substring(0, newlineIndex).trim();
                        this.lineBuffer = this.lineBuffer.substring(newlineIndex + 1);
                        if (line && this.onDataCallback) {
                            this.onDataCallback(line);
                        }
                    }
                    if (this.lineBuffer.length > 2000) {
                        this.lineBuffer = '';
                    }
                }
            } catch (error) {
                if (this.isConnected) console.error('Read error:', error);
                break;
            }
        }
        this.readLoopRunning = false;
        if (this.isConnected) {
            this.isConnected = false;
            this.onDisconnectCallback?.();
        }
    }

    async sendCommand(command) {
        if (!this.isConnected || !this.writer) throw new Error('Device not connected');
        await this.writer.write(command + '\r\n');
        await new Promise(resolve => setTimeout(resolve, 20));
        return true;
    }

    onData(callback) {
        this.onDataCallback = callback;
    }

    onDisconnect(callback) {
        this.onDisconnectCallback = callback;
    }

    getConnected() {
        return this.isConnected;
    }
}

const serialConnection = new SerialConnection();
