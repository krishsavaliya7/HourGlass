/**
 * Mode-Specific UI Logic
 * Handles mode switching and mode-specific controls
 */

class ModeManager {
    constructor() {
        this.currentMode = 'clock';
        this.modeControls = {};
        this.modeButtons = [];
        
        // Wait for DOM to be ready
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', () => this.initDOM());
        } else {
            this.initDOM();
        }
    }
    
    /**
     * Initialize DOM elements
     */
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

    /**
     * Initialize event listeners
     */
    initEventListeners() {
        // Mode button clicks
        this.modeButtons.forEach(btn => {
            btn.addEventListener('click', () => {
                const mode = btn.dataset.mode;
                this.setMode(mode);
            });
        });

        // Clock controls
        const setClockBtn = document.getElementById('btn-set-clock');
        if (setClockBtn) {
            setClockBtn.addEventListener('click', () => this.setClockTime());
        }

        // Hourglass controls
        const setHourglassBtn = document.getElementById('btn-set-hourglass');
        if (setHourglassBtn) {
            setHourglassBtn.addEventListener('click', () => this.setHourglassDuration());
        }

        const resetHourglassBtn = document.getElementById('btn-reset-hourglass');
        if (resetHourglassBtn) {
            resetHourglassBtn.addEventListener('click', () => this.resetHourglass());
        }

        // Dice controls
        const rollDiceBtn = document.getElementById('btn-roll-dice');
        if (rollDiceBtn) {
            rollDiceBtn.addEventListener('click', () => this.rollDice());
        }

        // Flip counter controls
        const resetCounterBtn = document.getElementById('btn-reset-counter');
        if (resetCounterBtn) {
            resetCounterBtn.addEventListener('click', () => this.resetFlipCounter());
        }
    }

    /**
     * Set current mode
     */
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

    /**
     * Update UI for current mode
     */
    updateUI() {
        // Update mode buttons
        this.modeButtons.forEach(btn => {
            if (btn.dataset.mode === this.currentMode) {
                btn.classList.add('active');
            } else {
                btn.classList.remove('active');
            }
        });

        // Update mode controls visibility
        Object.keys(this.modeControls).forEach(mode => {
            const control = this.modeControls[mode];
            // Guard against null/undefined DOM nodes
            if (!control) {
                console.warn(`Missing mode control for mode: ${mode}`);
                return;
            }
            if (mode === this.currentMode) {
                control.classList.add('active');
            } else {
                control.classList.remove('active');
            }
        });

        // Update mode display
        const modeDisplay = document.getElementById('current-mode');
        if (modeDisplay) {
            modeDisplay.textContent = this.currentMode.charAt(0).toUpperCase() + 
                                    this.currentMode.slice(1);
        }
    }

    /**
     * Set clock time
     */
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

    /**
     * Set hourglass duration
     */
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

    /**
     * Reset hourglass
     */
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

    /**
     * Roll dice
     */
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
            const value = result.value || result.diceValue;
            
            // Validate that we got a proper value
            if (value === undefined || value === null) {
                console.warn('api.rollDice() returned invalid response:', result);
                console.error('Missing both "value" and "diceValue" fields in rollDice response');
                throw new Error('Invalid dice response: missing value field');
            }
            
            this.displayDiceValue(value);
            this.addDiceHistory(value);
        } catch (error) {
            console.error('Failed to roll dice:', error);
            alert(`Failed to roll dice: ${error.message}`);
        } finally {
            if (rollBtn) {
                rollBtn.disabled = !api.isConnected();
                rollBtn.textContent = 'Roll Dice';
            }
        }
    }

    /**
     * Display dice value
     */
    displayDiceValue(value) {
        const diceValueEl = document.getElementById('dice-value');
        const dicePatternEl = document.getElementById('dice-pattern');

        if (diceValueEl) {
            diceValueEl.textContent = value;
        }

        if (dicePatternEl) {
            // Clear previous pattern
            dicePatternEl.innerHTML = '';
            
            // Create dice pattern based on value
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
                    dot.className = 'dice-dot';
                    if (cell === 1) {
                        dot.classList.add('active');
                    }
                    dicePatternEl.appendChild(dot);
                });
            });
        }
    }

    /**
     * Add dice roll to history
     */
    addDiceHistory(value) {
        const historyList = document.getElementById('dice-history-list');
        if (!historyList) return;

        const item = document.createElement('div');
        item.className = 'history-item';
        item.textContent = value;
        historyList.insertBefore(item, historyList.firstChild);

        // Keep only last 10 items
        while (historyList.children.length > 10) {
            historyList.removeChild(historyList.lastChild);
        }
    }

    /**
     * Reset flip counter
     */
    async resetFlipCounter() {
        if (!api.isConnected()) {
            alert('Please connect device first.');
            return;
        }

        if (!confirm('Reset flip counter to zero?')) return;

        try {
            await api.resetFlipCounter();
            this.updateFlipCount(0);
            alert('Flip counter reset!');
        } catch (error) {
            console.error('Failed to reset flip counter:', error);
            alert(`Failed to reset counter: ${error.message}`);
        }
    }

    /**
     * Update hourglass progress
     */
    updateHourglassProgress(percent) {
        const progressBar = document.getElementById('hourglass-progress');
        const progressText = document.getElementById('hourglass-progress-text');

        if (progressBar) {
            progressBar.style.width = `${Math.min(100, Math.max(0, percent))}%`;
        }

        if (progressText) {
            progressText.textContent = `${Math.round(percent)}%`;
        }
    }

    /**
     * Update flip count display
     */
    updateFlipCount(count) {
        const countEl = document.getElementById('flip-count');
        if (countEl) {
            countEl.textContent = count || 0;
        }
    }

    /**
     * Get current mode
     */
    getCurrentMode() {
        return this.currentMode;
    }
}

// Export singleton instance
const modeManager = new ModeManager();

