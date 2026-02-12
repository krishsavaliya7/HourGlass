class Display {
    constructor() {
        this.matrixAElement = null;
        this.matrixBElement = null;
        this.matrixCellsA = [];
        this.matrixCellsB = [];
        this.orientationArrow = null;
        this.orientationAngle = null;
        this.orientationLabel = null;
        
        // Orientation smoothing
        this.lastDisplayedAngle = null;
        this.angleHistory = [];
        this.lastAngleUpdateTime = 0;
        
        // Matrix state caching
        this.lastMatrixAState = null;
        this.lastMatrixBState = null;
        
        // Clock mode state
        this.currentMode = null;
        this.clockTime = null;
        
        // Small 3x5 digit font for matrix display
        this.digitFont = [
            // 0
            [0b111, 0b101, 0b101, 0b101, 0b111],
            // 1
            [0b010, 0b110, 0b010, 0b010, 0b111],
            // 2
            [0b111, 0b001, 0b111, 0b100, 0b111],
            // 3
            [0b111, 0b001, 0b111, 0b001, 0b111],
            // 4
            [0b101, 0b101, 0b111, 0b001, 0b001],
            // 5
            [0b111, 0b100, 0b111, 0b001, 0b111],
            // 6
            [0b111, 0b100, 0b111, 0b101, 0b111],
            // 7
            [0b111, 0b001, 0b001, 0b010, 0b010],
            // 8
            [0b111, 0b101, 0b111, 0b101, 0b111],
            // 9
            [0b111, 0b101, 0b111, 0b001, 0b001]
        ];
        
        const init = () => this.initDisplay();
        document.readyState === 'loading' 
            ? document.addEventListener('DOMContentLoaded', init) 
            : init();
    }
    
    initDisplay() {
        this.matrixAElement = document.getElementById('matrix-a');
        this.matrixBElement = document.getElementById('matrix-b');
        this.orientationArrow = document.querySelector('.orientation-arrow');
        this.orientationAngle = document.getElementById('orientation-angle');
        this.orientationLabel = document.getElementById('orientation-label');
        
        if (this.matrixAElement && this.matrixBElement) {
            this.create8x8Grid(this.matrixAElement, this.matrixCellsA);
            this.create8x8Grid(this.matrixBElement, this.matrixCellsB);
        }
    }

    create8x8Grid(container, cellStore) {
        if (!container) return;
        container.innerHTML = '';
        cellStore.length = 0;
        for (let i = 0; i < 64; i++) {
            const cell = document.createElement('div');
            cell.className = 'led-cell';
            cellStore.push(cell);
            container.appendChild(cell);
        }
    }

    updateMatrix(matrixA, matrixB) {
        this._updateMatrixCells(this.matrixCellsA, matrixA, 'A');
        this._updateMatrixCells(this.matrixCellsB, matrixB, 'B');
    }

    _updateMatrixCells(cellStore, matrix, matrixId) {
        if (!cellStore || cellStore.length !== 64) return;
        
        const lastState = matrixId === 'A' ? this.lastMatrixAState : this.lastMatrixBState;
        const currentState = lastState || new Array(64);
        
        for (let row = 0; row < 8; row++) {
            for (let col = 0; col < 8; col++) {
                const idx = row * 8 + col;
                const isActive = Boolean(matrix?.[row]?.[col]);
                
                if (currentState[idx] !== isActive) {
                    cellStore[idx].classList.toggle('active', isActive);
                    currentState[idx] = isActive;
                }
            }
        }
        
        if (matrixId === 'A') this.lastMatrixAState = currentState;
        else this.lastMatrixBState = currentState;
    }

    updateFromAPI(displayData, mode, time, orientation, flipCount) {
        const angle = orientation || 0;
        
        // For clock mode at 90° or 270°: show digits
        if (mode === 'clock' && time) {
            if (angle === 90 || angle === 270) {
                const [hours, minutes] = time.split(':').map(Number);
                this.updateClockDisplay(hours, minutes);
                return;
            }
        }
        
        // For flip counter mode: always show digits (not dots)
        if (mode === 'flipcounter' && flipCount !== undefined) {
            this.updateFlipCounterDisplay(flipCount, angle);
            return;
        }
        
        // Default: show raw LED patterns
        if (!displayData) return;
        this.updateMatrix(
            this.normalizeMatrix(displayData.matrixA),
            this.normalizeMatrix(displayData.matrixB)
        );
    }

    normalizeMatrix(data) {
        const empty = Array.from({ length: 8 }, () => Array(8).fill(false));
        if (!data) return empty;

        if (typeof data === 'string' && data.length === 16) {
            const matrix = [];
            for (let row = 0; row < 8; row++) {
                const byteVal = parseInt(data.substring(row * 2, row * 2 + 2), 16);
                if (isNaN(byteVal)) return empty;
                matrix[row] = [];
                for (let col = 0; col < 8; col++) {
                    matrix[row][col] = Boolean((byteVal >> (7 - col)) & 1);
                }
            }
            return matrix;
        }

        if (!Array.isArray(data) || data.length === 0) return empty;
        if (Array.isArray(data[0])) {
            return Array.from({ length: 8 }, (_, row) =>
                Array.from({ length: 8 }, (_, col) => Boolean(data?.[row]?.[col]))
            );
        }

        const matrix = [];
        for (let row = 0; row < 8; row++) {
            matrix[row] = [];
            for (let col = 0; col < 8; col++) {
                matrix[row][col] = Boolean(data[row * 8 + col]);
            }
        }
        return matrix;
    }

    smoothAngle(rawAngle) {
        this.angleHistory.push(rawAngle);
        if (this.angleHistory.length > 5) this.angleHistory.shift();
        
        if (this.angleHistory.length < 2) return rawAngle;
        
        const hasWrapAround = this.angleHistory.some(a => a < 45) && 
                              this.angleHistory.some(a => a > 315);
        
        if (hasWrapAround) {
            const adjusted = this.angleHistory.map(a => a < 180 ? a + 360 : a);
            return (adjusted.reduce((sum, a) => sum + a, 0) / adjusted.length) % 360;
        }
        
        return this.angleHistory.reduce((sum, a) => sum + a, 0) / this.angleHistory.length;
    }

    snapToCardinal(angle, threshold = 10) {
        for (const cardinal of [0, 90, 180, 270]) {
            let diff = Math.abs(angle - cardinal);
            if (diff > 180) diff = 360 - diff;
            if (diff < threshold) return cardinal;
        }
        return angle;
    }

    updateOrientation(data) {
        if (!data) return;
        
        const now = Date.now();
        if (now - this.lastAngleUpdateTime < 50) return;
        
        let rawAngle = data.angle || 0;
        if (isNaN(rawAngle) || !isFinite(rawAngle)) rawAngle = 0;
        
        rawAngle = ((rawAngle % 360) + 360) % 360;
        let smoothedAngle = this.smoothAngle(rawAngle);
        smoothedAngle = this.snapToCardinal(smoothedAngle, 8);
        
        const displayAngle = Math.round(smoothedAngle);
        
        if (this.lastDisplayedAngle === null || 
            Math.abs(displayAngle - this.lastDisplayedAngle) >= 1) {
            
            if (this.orientationArrow) {
                this.orientationArrow.style.transition ||= 'transform 0.15s ease-out';
                const gravityAngle = (displayAngle + 180) % 360;
                this.orientationArrow.style.transform = `translate(-50%, 0) rotate(${gravityAngle}deg)`;
            }
            
            if (this.orientationAngle) {
                this.orientationAngle.textContent = `${displayAngle}°`;
            }
            
            if (this.orientationLabel) {
                const labels = [
                    { max: 45, label: 'Upright' },
                    { max: 135, label: 'Right' },
                    { max: 225, label: 'Upside Down' },
                    { max: 315, label: 'Left' }
                ];
                const label = labels.find(l => displayAngle < l.max)?.label || 'Upright';
                this.orientationLabel.textContent = label;
            }
            
            this.lastDisplayedAngle = displayAngle;
            this.lastAngleUpdateTime = now;
        }
    }

    showTestPattern() {
        const matrixA = [];
        const matrixB = [];
        for (let row = 0; row < 8; row++) {
            const rowA = [];
            const rowB = [];
            for (let col = 0; col < 8; col++) {
                rowA.push((row + col) % 2 === 0);
                rowB.push(row % 2 === col % 2);
            }
            matrixA.push(rowA);
            matrixB.push(rowB);
        }
        this.updateMatrix(matrixA, matrixB);
    }
    
    // Set current mode for special rendering
    setMode(mode) {
        this.currentMode = mode;
    }
    
    // Set clock time for digit rendering
    setClockTime(time) {
        this.clockTime = time;
    }
    
    // Draw a single digit on a matrix (3x5 font, positioned at top-left)
    drawDigit(matrix, digit, offsetX, offsetY) {
        const font = this.digitFont[digit] || this.digitFont[0];
        for (let row = 0; row < 5; row++) {
            for (let col = 0; col < 3; col++) {
                const x = offsetX + col;
                const y = offsetY + row;
                if (x < 8 && y < 8) {
                    // Reverse the bit order to fix mirroring
                    // col 0 (left) gets bit 0, col 2 (right) gets bit 2
                    matrix[y][x] = (font[row] >> col) & 1;
                }
            }
        }
    }
    
    // Draw two digits (tens and ones) on a single 8x8 matrix
    // Ones on left, tens on right to fix "04" showing as "40"
    drawTwoDigits(matrix, number) {
        const num = Math.max(0, Math.min(99, number));
        const tens = Math.floor(num / 10);
        const ones = num % 10;
        
        // Clear matrix
        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                matrix[y][x] = false;
            }
        }
        
        // Draw ones digit on LEFT side (offsetX=1) - this will appear on the left
        this.drawDigit(matrix, ones, 1, 1);
        // Draw tens digit on RIGHT side (offsetX=5) - this will appear on the right
        this.drawDigit(matrix, tens, 5, 1);
    }
    
    // Create a matrix showing clock digits instead of raw LED data
    createClockMatrix(hours, minutes) {
        const topMatrix = Array(8).fill(null).map(() => Array(8).fill(false));
        const bottomMatrix = Array(8).fill(null).map(() => Array(8).fill(false));
        
        // Hours on top matrix
        this.drawTwoDigits(topMatrix, hours);
        // Minutes on bottom matrix  
        this.drawTwoDigits(bottomMatrix, minutes);
        
        return { topMatrix, bottomMatrix };
    }
    
    // Update display with clock mode special rendering
    updateClockDisplay(hours, minutes) {
        const { topMatrix, bottomMatrix } = this.createClockMatrix(hours, minutes);
        // topMatrix (hours) goes to matrix B (top), bottomMatrix (minutes) goes to matrix A (bottom)
        // updateMatrix expects (matrixA, matrixB) where A=bottom, B=top
        this.updateMatrix(bottomMatrix, topMatrix);
    }
    
    // Update display for flip counter mode
    updateFlipCounterDisplay(count, orientation) {
        const topMatrix = Array(8).fill(null).map(() => Array(8).fill(false));
        const bottomMatrix = Array(8).fill(null).map(() => Array(8).fill(false));
        
        // At 0°: show on top matrix only
        // At 180°: show on bottom matrix only
        // At 90°/270°: show on both matrices
        if (orientation === 0) {
            this.drawTwoDigits(topMatrix, count);
        } else if (orientation === 180) {
            this.drawTwoDigits(bottomMatrix, count);
        } else {
            // 90° or 270° - show on both
            this.drawTwoDigits(topMatrix, count);
            this.drawTwoDigits(bottomMatrix, count);
        }
        
        // updateMatrix expects (matrixA, matrixB) where A=bottom, B=top
        this.updateMatrix(bottomMatrix, topMatrix);
    }
}

const display = new Display();
