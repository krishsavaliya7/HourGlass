/**
 * Serial Communication Module
 * Handles Web Serial API communication with Arduino Nano R3
 * 
 * Hardware:
 * - Microcontroller: Arduino Nano R3 (ATmega328P)
 * - LED Display: 2× Max7219 8×8 LED Matrix (16×8 total)
 * - Connection: USB Serial @9600 baud
 * - Sensor: MPU6050 6-axis accelerometer/gyroscope (I2C)
 * - Button: Single push button for mode selection
 */

class SerialConnection {
    constructor() {
        this.port = null;
        this.reader = null;
        this.writer = null;
        this.textDecoder = null;
        this.textEncoder = null;
        this.isConnected = false;
        this.readLoopRunning = false;
        this.onDataCallback = null;
        this.onDisconnectCallback = null;
        this.hardwareInfo = {
            type: 'Arduino Nano R3',
            board: 'ATmega328P',
            displays: '2× Max7219 8×8 LED Matrix',
            sensors: 'MPU6050 (I2C)',
            baudRate: 9600
        };
    }

    /**
     * Check if Web Serial API is available
     */
    isAvailable() {
        return 'serial' in navigator;
    }

    /**
     * Connect to serial device
     */
    async connect() {
        if (!this.isAvailable()) {
            throw new Error('Web Serial API not supported in this browser. Use Chrome/Edge.');
        }

        try {
            // Request port
            this.port = await navigator.serial.requestPort();
            
            // Open port at 9600 baud (standard Arduino rate)
            await this.port.open({ baudRate: 9600 });
            
            // Setup text encoder/decoder
            this.textEncoder = new TextEncoderStream();
            this.textDecoder = new TextDecoderStream();
            
            // Setup streams - handle promise chains properly
            this.textEncoder.readable.pipeTo(this.port.writable).catch(() => {});
            this.port.readable.pipeTo(this.textDecoder.writable).catch(() => {});
            
            // Get writer and reader
            this.writer = this.textEncoder.writable.getWriter();
            this.reader = this.textDecoder.readable.getReader();
            
            this.isConnected = true;
            this.startReadLoop();
            
            return true;
        } catch (error) {
            if (error.name === 'NotFoundError') {
                throw new Error('No device selected');
            }
            throw error;
        }
    }

    /**
     * Disconnect from serial device
     */
    async disconnect() {
        this.isConnected = false;
        
        try {
            // Close text encoder first
            if (this.textEncoder) {
                try {
                    await this.textEncoder.writable.close();
                } catch (e) {
                    console.warn('Error closing textEncoder.writable:', e);
                }
                this.textEncoder = null;
            }
            
            // Close writer
            if (this.writer) {
                try {
                    await this.writer.close();
                } catch (e) {
                    console.warn('Error closing writer:', e);
                }
                this.writer = null;
            }
            
            // Cancel and release reader
            if (this.reader) {
                try {
                    await this.reader.cancel();
                    await this.reader.releaseLock();
                } catch (e) {
                    console.warn('Error canceling/releasing reader:', e);
                }
                this.reader = null;
            }
            
            // Close text decoder (do not cancel readable separately)
            if (this.textDecoder) {
                this.textDecoder = null;
            }
            
            // Close port last
            if (this.port) {
                try {
                    await this.port.close();
                } catch (e) {
                    console.warn('Error closing port:', e);
                }
                this.port = null;
            }
        } catch (error) {
            console.error('Error disconnecting:', error);
        }
        
        if (this.onDisconnectCallback) {
            this.onDisconnectCallback();
        }
    }

    /**
     * Start reading loop
     */
    async startReadLoop() {
        if (this.readLoopRunning) return;
        this.readLoopRunning = true;
        
        while (this.isConnected && this.reader) {
            try {
                const { value, done } = await this.reader.read();
                
                if (done) {
                    break;
                }
                
                if (value && this.onDataCallback) {
                    // Handle incoming data (lines)
                    const lines = value.split('\n').filter(line => line.trim());
                    lines.forEach(line => {
                        this.onDataCallback(line.trim());
                    });
                }
            } catch (error) {
                if (this.isConnected) {
                    console.error('Read error:', error);
                }
                break;
            }
        }
        
        this.readLoopRunning = false;
        
        if (this.isConnected) {
            // Unexpected disconnect
            this.isConnected = false;
            if (this.onDisconnectCallback) {
                this.onDisconnectCallback();
            }
        }
    }

    /**
     * Send command to device
     */
    async sendCommand(command) {
        if (!this.isConnected || !this.writer) {
            throw new Error('Device not connected');
        }
        
        try {
            const data = command + '\n';
            await this.writer.write(data);
            return true;
        } catch (error) {
            console.error('Send error:', error);
            throw new Error('Failed to send command');
        }
    }

    /**
     * Set callback for incoming data
     */
    onData(callback) {
        this.onDataCallback = callback;
    }

    /**
     * Set callback for disconnect
     */
    onDisconnect(callback) {
        this.onDisconnectCallback = callback;
    }

    /**
     * Get connection status
     */
    getConnected() {
        return this.isConnected;
    }
}

// Export singleton instance
const serialConnection = new SerialConnection();

