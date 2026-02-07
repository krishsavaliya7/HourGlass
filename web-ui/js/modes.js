class ModeManager {
    constructor() {
        this.currentMode = 'clock';
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

        const setClock = document.getElementById('btn-set-clock');
        if (setClock) setClock.addEventListener('click', () => this.setClockTime());

        const setHourglass = document.getElementById('btn-set-hourglass');
        if (setHourglass) setHourglass.addEventListener('click', () => this.setHourglassDuration());

        const resetHourglass = document.getElementById('btn-reset-hourglass');
        if (resetHourglass) resetHourglass.addEventListener('click', () => this.resetHourglass());

        const rollDice = document.getElementById('btn-roll-dice');
        if (rollDice) rollDice.addEventListener('click', () => this.rollDice());

        const resetCounter = document.getElementById('btn-reset-counter');
        if (resetCounter) resetCounter.addEventListener('click', () => this.resetFlipCounter());
    }

    async setMode(mode) {
        if (this.currentMode === mode) return;
        if (!api.isConnected()) {
            alert('Please connect device first using the "Connect Device" button.');
            return;
        }
        try {
            await api.setMode(mode);
            this.currentMode = mode;
            this.updateUI();
        } catch (error) {
            console.error('Failed to set mode:', error);
            alert(`Failed to change mode: ${error.message}`);
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
        const hours = parseInt(document.getElementById('clock-hours').value) || 12;
        const minutes = parseInt(document.getElementById('clock-minutes').value) || 0;
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

    async setHourglassDuration() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }
        const hours = parseInt(document.getElementById('hourglass-hours').value) || 0;
        const minutes = parseInt(document.getElementById('hourglass-minutes').value) || 1;
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
            // Check if we got dual dice response
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
            // Silently ignore error - no popup
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
            pattern.forEach(row => {
                row.forEach(cell => {
                    const dot = document.createElement('div');
                    dot.className = 'dice-dot' + (cell ? ' active' : '');
                    dicePatternEl.appendChild(dot);
                });
            });
        }
    }

    displayDualDice(valueA, valueB) {
        // Display two dice values (for shake-to-roll feature)
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
        
        // Update Dice A (Matrix A)
        if (dicePatternA) {
            dicePatternA.innerHTML = '';
            const patternA = patterns[valueA] || patterns[1];
            patternA.forEach(row => {
                row.forEach(cell => {
                    const dot = document.createElement('div');
                    dot.className = 'dice-dot' + (cell ? ' active' : '');
                    dicePatternA.appendChild(dot);
                });
            });
        }
        
        // Update Dice B (Matrix B)
        if (dicePatternB) {
            dicePatternB.innerHTML = '';
            const patternB = patterns[valueB] || patterns[1];
            patternB.forEach(row => {
                row.forEach(cell => {
                    const dot = document.createElement('div');
                    dot.className = 'dice-dot' + (cell ? ' active' : '');
                    dicePatternB.appendChild(dot);
                });
            });
        }
        
        this.addDiceHistory(`${valueA}+${valueB}=${sum}`);
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
        if (progressBar) progressBar.style.width = `${Math.min(100, Math.max(0, percent))}%`;
        if (progressText) progressText.textContent = `${Math.round(percent)}%`;
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
