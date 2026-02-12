class ModeManager {
    constructor() {
        this.currentMode = 'hourglass';
        this.modeControls = {};
        this.modeButtons = [];
        const init = () => this.initDOM();
        document.readyState === 'loading' 
            ? document.addEventListener('DOMContentLoaded', init) 
            : init();
    }
    
    initDOM() {
        this.modeControls = {
            clock: document.getElementById('clock-controls'),
            hourglass: document.getElementById('hourglass-controls'),
            dice: document.getElementById('dice-controls'),
            flipcounter: document.getElementById('flipcounter-controls')
        };
        this.modeButtons = document.querySelectorAll('.mode-btn');
        this.initEventListeners();
    }

    initEventListeners() {
        this.modeButtons.forEach(btn => {
            btn.addEventListener('click', () => this.setMode(btn.dataset.mode));
        });

        document.getElementById('btn-set-clock')?.addEventListener('click', () => this.setClockTime());
        document.getElementById('btn-sync-laptop-time')?.addEventListener('click', () => this.syncLaptopTime());
        document.getElementById('btn-set-hourglass')?.addEventListener('click', () => this.setHourglassDuration());
        document.getElementById('btn-reset-hourglass')?.addEventListener('click', () => this.resetHourglass());
        document.getElementById('btn-roll-dice')?.addEventListener('click', () => this.rollDice());
        document.getElementById('btn-reset-counter')?.addEventListener('click', () => this.resetFlipCounter());
    }

    async setMode(mode) {
        if (this.currentMode === mode) return;
        if (!api.isConnected()) {
            alert('Please connect device first using the "Connect Device" button.');
            return;
        }
        
        try {
            console.log(`Sending SET_MODE command: ${mode}`);
            await api.setMode(mode);
            console.log(`SET_MODE ${mode} successful`);
            this.currentMode = mode;
            this.updateUI();
        } catch (error) {
            console.error('Failed to set mode:', error);
            alert(`Failed to change mode: ${error.message}`);
        }
    }

    syncModeFromHardware(mode) {
        if (this.currentMode !== mode) {
            this.currentMode = mode;
            this.updateUI();
        }
    }

    updateUI() {
        this.modeButtons.forEach(btn => {
            btn.classList.toggle('active', btn.dataset.mode === this.currentMode);
        });
        Object.keys(this.modeControls).forEach(mode => {
            const control = this.modeControls[mode];
            if (control) control.classList.toggle('active', mode === this.currentMode);
        });
        const modeDisplay = document.getElementById('current-mode');
        if (modeDisplay) {
            modeDisplay.textContent = this.currentMode.charAt(0).toUpperCase() + this.currentMode.slice(1);
        }
    }

    async setClockTime() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }
        const hours = parseInt(document.getElementById('clock-hours')?.value) || 12;
        const minutes = parseInt(document.getElementById('clock-minutes')?.value) || 0;
        if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
            alert('Invalid time! Hours: 0-23, Minutes: 0-59');
            return;
        }
        try {
            await api.setClockTime(hours, minutes);
            alert('Clock time set successfully!');
        } catch (error) {
            console.error('Failed to set clock time:', error);
            alert(`Failed to set clock time: ${error.message}`);
        }
    }

    async syncLaptopTime() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }
        
        // Get current laptop time
        const now = new Date();
        const hours = now.getHours();
        const minutes = now.getMinutes();
        
        // Update input fields
        const hoursInput = document.getElementById('clock-hours');
        const minutesInput = document.getElementById('clock-minutes');
        if (hoursInput) hoursInput.value = hours;
        if (minutesInput) minutesInput.value = minutes;
        
        // Send to device
        try {
            await api.setClockTime(hours, minutes);
            alert(`Clock synced with laptop time: ${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}`);
        } catch (error) {
            console.error('Failed to sync laptop time:', error);
            alert(`Failed to sync time: ${error.message}`);
        }
    }

    async setHourglassDuration() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }
        const hours = parseInt(document.getElementById('hourglass-hours')?.value) || 0;
        const minutes = parseInt(document.getElementById('hourglass-minutes')?.value) || 1;
        if (hours < 0 || hours > 23 || minutes < 1 || minutes > 59) {
            alert('Invalid duration! Hours: 0-23, Minutes: 1-59');
            return;
        }
        if (hours === 0 && minutes === 0) {
            alert('Duration must be at least 1 minute!');
            return;
        }
        try {
            await api.setHourglassDuration(hours, minutes);
            alert('Hourglass duration set successfully!');
        } catch (error) {
            console.error('Failed to set hourglass duration:', error);
            alert(`Failed to set duration: ${error.message}`);
        }
    }

    async resetHourglass() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }
        try {
            await api.resetHourglass();
            this.updateHourglassProgress(0);
            alert('Hourglass reset!');
        } catch (error) {
            console.error('Failed to reset hourglass:', error);
            alert(`Failed to reset hourglass: ${error.message}`);
        }
    }

    async rollDice() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }
        const rollBtn = document.getElementById('btn-roll-dice');
        if (rollBtn) {
            rollBtn.disabled = true;
            rollBtn.textContent = 'Rolling...';
        }
        try {
            const result = await api.rollDice();
            if (result.diceA !== undefined && result.diceB !== undefined) {
                this.displayDualDice(result.diceA, result.diceB);
            } else {
                const value = result.value || result.diceValue;
                if (value === undefined || value === null) {
                    throw new Error('Invalid dice response: missing value field');
                }
                this.displayDiceValue(value);
                this.addDiceHistory(value);
            }
        } catch (error) {
            console.error('Failed to roll dice:', error);
        } finally {
            if (rollBtn) {
                rollBtn.disabled = !api.isConnected();
                rollBtn.textContent = 'Roll Dice';
            }
        }
    }

    displayDiceValue(value) {
        const diceValueEl = document.getElementById('dice-value');
        const dicePatternEl = document.getElementById('dice-pattern');
        if (diceValueEl) diceValueEl.textContent = value;
        if (dicePatternEl) {
            dicePatternEl.innerHTML = '';
            const patterns = {
                1: [[0, 0, 0], [0, 1, 0], [0, 0, 0]],
                2: [[1, 0, 0], [0, 0, 0], [0, 0, 1]],
                3: [[1, 0, 0], [0, 1, 0], [0, 0, 1]],
                4: [[1, 0, 1], [0, 0, 0], [1, 0, 1]],
                5: [[1, 0, 1], [0, 1, 0], [1, 0, 1]],
                6: [[1, 0, 1], [1, 0, 1], [1, 0, 1]]
            };
            const pattern = patterns[value] || patterns[1];
            this._renderDicePattern(dicePatternEl, pattern);
        }
    }

    displayDualDice(valueA, valueB) {
        const diceValueEl = document.getElementById('dice-value');
        const dicePatternA = document.getElementById('dice-pattern-a');
        const dicePatternB = document.getElementById('dice-pattern-b');
        const sum = valueA + valueB;
        
        if (diceValueEl) diceValueEl.textContent = `${valueA} + ${valueB} = ${sum}`;
        
        const patterns = {
            1: [[0, 0, 0], [0, 1, 0], [0, 0, 0]],
            2: [[1, 0, 0], [0, 0, 0], [0, 0, 1]],
            3: [[1, 0, 0], [0, 1, 0], [0, 0, 1]],
            4: [[1, 0, 1], [0, 0, 0], [1, 0, 1]],
            5: [[1, 0, 1], [0, 1, 0], [1, 0, 1]],
            6: [[1, 0, 1], [1, 0, 1], [1, 0, 1]]
        };
        
        if (dicePatternA) {
            dicePatternA.innerHTML = '';
            this._renderDicePattern(dicePatternA, patterns[valueA] || patterns[1]);
        }
        
        if (dicePatternB) {
            dicePatternB.innerHTML = '';
            this._renderDicePattern(dicePatternB, patterns[valueB] || patterns[1]);
        }
        
        this.addDiceHistory(`${valueA}+${valueB}=${sum}`);
    }

    _renderDicePattern(container, pattern) {
        pattern.forEach(row => {
            row.forEach(cell => {
                const dot = document.createElement('div');
                dot.className = 'dice-dot' + (cell ? ' active' : '');
                container.appendChild(dot);
            });
        });
    }

    addDiceHistory(value) {
        const historyList = document.getElementById('dice-history-list');
        if (!historyList) return;
        const item = document.createElement('div');
        item.className = 'history-item';
        item.textContent = value;
        historyList.insertBefore(item, historyList.firstChild);
        while (historyList.children.length > 10) {
            historyList.removeChild(historyList.lastChild);
        }
    }

    async resetFlipCounter() {
        if (!api.isConnected()) return;
        try {
            await api.resetFlipCounter();
            this.updateFlipCount(0);
        } catch (error) {
            console.error('Failed to reset flip counter:', error);
        }
    }

    updateHourglassProgress(percent) {
        const progressBar = document.getElementById('hourglass-progress');
        const progressText = document.getElementById('hourglass-progress-text');
        const clamped = Math.min(100, Math.max(0, percent));
        if (progressBar) progressBar.style.width = `${clamped}%`;
        if (progressText) progressText.textContent = `${Math.round(clamped)}%`;
    }

    updateFlipCount(count) {
        const countEl = document.getElementById('flip-count');
        if (countEl) countEl.textContent = count || 0;
    }

    getCurrentMode() {
        return this.currentMode;
    }
}

const modeManager = new ModeManager();
