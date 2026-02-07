class Display {
    constructor() {
        this.matrixStackElement = null;
        this.matrixAElement = null;
        this.matrixBElement = null;
        this.matrixCellsA = [];
        this.matrixCellsB = [];
        this.orientationArrow = null;
        this.orientationAngle = null;
        this.orientationLabel = null;
        
<<<<<<< HEAD
        // Orientation smoothing variables
        this.lastDisplayedAngle = null;
        this.smoothedAngle = 0;
        this.angleHistory = [];
        this.maxHistoryLength = 5; // Average over 5 readings
        this.lastAngleUpdateTime = 0;
        this.angleUpdateThrottle = 50; // Only update display every 50ms max
        
        // Matrix state caching to prevent unnecessary DOM updates
        this.lastMatrixAState = null;
        this.lastMatrixBState = null;
        
=======
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
        const init = () => this.initDisplay();
        document.readyState === 'loading' 
            ? document.addEventListener('DOMContentLoaded', init) 
            : init();
    }
    
    initDisplay() {
        this.matrixStackElement = document.getElementById('matrix-display');
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

<<<<<<< HEAD
    // Optimized matrix update - only update changed cells
    updateMatrix(matrixA, matrixB) {
        this.updateMatrixOptimized(this.matrixCellsA, matrixA, 'A');
        this.updateMatrixOptimized(this.matrixCellsB, matrixB, 'B');
    }

    updateMatrixOptimized(cellStore, matrix, matrixId) {
        if (!cellStore || cellStore.length !== 64) return;
        
        // Get the last state for this matrix
        const lastState = matrixId === 'A' ? this.lastMatrixAState : this.lastMatrixBState;
        
        // Update cells only if they changed
=======
    updateMatrix(matrixA, matrixB) {
        this.update8x8Cells(this.matrixCellsA, matrixA);
        this.update8x8Cells(this.matrixCellsB, matrixB);
    }

    update8x8Cells(cellStore, matrix) {
        if (!cellStore || cellStore.length !== 64) return;
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
        for (let row = 0; row < 8; row++) {
            for (let col = 0; col < 8; col++) {
                const idx = row * 8 + col;
                const isActive = Boolean(matrix?.[row]?.[col]);
<<<<<<< HEAD
                
                // Only update DOM if state changed
                if (!lastState || lastState[idx] !== isActive) {
                    cellStore[idx].classList.toggle('active', isActive);
                }
                
                // Store current state
                if (!lastState) {
                    if (matrixId === 'A') this.lastMatrixAState = new Array(64);
                    else this.lastMatrixBState = new Array(64);
                }
                
                if (matrixId === 'A') {
                    this.lastMatrixAState[idx] = isActive;
                } else {
                    this.lastMatrixBState[idx] = isActive;
                }
=======
                cellStore[idx].classList.toggle('active', isActive);
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
            }
        }
    }

    updateFromAPI(displayData) {
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
                const hexByte = data.substring(row * 2, row * 2 + 2);
                const byteVal = parseInt(hexByte, 16);
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

<<<<<<< HEAD
    // Smooth angle transition with moving average and intelligent snapping
    smoothAngle(rawAngle) {
        // Add to history
        this.angleHistory.push(rawAngle);
        if (this.angleHistory.length > this.maxHistoryLength) {
            this.angleHistory.shift();
        }
        
        // Calculate average, handling wraparound at 0/360
        if (this.angleHistory.length < 2) {
            return rawAngle;
        }
        
        // Check if we're crossing the 0/360 boundary
        const hasWrapAround = this.angleHistory.some(a => a < 45) && 
                              this.angleHistory.some(a => a > 315);
        
        let avg;
        if (hasWrapAround) {
            // Adjust angles for wraparound calculation
            const adjusted = this.angleHistory.map(a => a < 180 ? a + 360 : a);
            avg = adjusted.reduce((sum, a) => sum + a, 0) / adjusted.length;
            avg = avg % 360;
        } else {
            avg = this.angleHistory.reduce((sum, a) => sum + a, 0) / this.angleHistory.length;
        }
        
        return avg;
    }

    // Snap angle to nearest cardinal direction if close enough
    snapToCardinal(angle, threshold = 10) {
        const cardinals = [0, 90, 180, 270];
        
        for (let cardinal of cardinals) {
            let diff = Math.abs(angle - cardinal);
            // Handle wraparound for 0/360
            if (diff > 180) diff = 360 - diff;
            
            if (diff < threshold) {
                return cardinal;
            }
        }
        
        return angle;
    }

    updateOrientation(data) {
        if (!data) return;
        
        const now = Date.now();
        
        // Throttle updates to prevent excessive reflows
        if (now - this.lastAngleUpdateTime < this.angleUpdateThrottle) {
            return;
        }
        
        let rawAngle = data.angle || 0;
        
        // Validate angle is a number and in valid range
        if (isNaN(rawAngle) || !isFinite(rawAngle)) {
            rawAngle = 0;
        }
        
        // Normalize angle to 0-359 range
        rawAngle = ((rawAngle % 360) + 360) % 360;
        
        // Apply smoothing
        let smoothedAngle = this.smoothAngle(rawAngle);
        
        // Snap to cardinal directions if close (prevents jitter near 0/90/180/270)
        smoothedAngle = this.snapToCardinal(smoothedAngle, 8);
        
        // Round to integer
        const displayAngle = Math.round(smoothedAngle);
        
        // Only update DOM if angle actually changed significantly
        if (this.lastDisplayedAngle === null || 
            Math.abs(displayAngle - this.lastDisplayedAngle) >= 1) {
            
            if (this.orientationArrow) {
                // Use CSS transition for smooth rotation
                if (!this.orientationArrow.style.transition) {
                    this.orientationArrow.style.transition = 'transform 0.15s ease-out';
                }
                // Arrow shows gravity direction (add 180° to device tilt angle)
                // At 0° upright, arrow points down (gravity direction)
                const gravityAngle = (displayAngle + 180) % 360;
                this.orientationArrow.style.transform = `translate(-50%, 0) rotate(${gravityAngle}deg)`;
            }
            
            if (this.orientationAngle) {
                this.orientationAngle.textContent = `${displayAngle}°`;
            }
            
            if (this.orientationLabel) {
                let label = 'Unknown';
                if (displayAngle >= 315 || displayAngle < 45) label = 'Upright';
                else if (displayAngle >= 45 && displayAngle < 135) label = 'Right';
                else if (displayAngle >= 135 && displayAngle < 225) label = 'Upside Down';
                else if (displayAngle >= 225 && displayAngle < 315) label = 'Left';
                this.orientationLabel.textContent = label;
            }
            
            this.lastDisplayedAngle = displayAngle;
            this.lastAngleUpdateTime = now;
=======
    updateOrientation(data) {
        if (!data) return;
        const angle = data.angle || 0;
        if (this.orientationArrow) {
            this.orientationArrow.style.transform = `translate(-50%, -50%) rotate(${angle}deg)`;
        }
        if (this.orientationAngle) {
            this.orientationAngle.textContent = `${Math.round(angle)}°`;
        }
        if (this.orientationLabel) {
            let label = 'Unknown';
            if (angle >= 315 || angle < 45) label = 'Upright';
            else if (angle >= 45 && angle < 135) label = 'Right';
            else if (angle >= 135 && angle < 225) label = 'Upside Down';
            else if (angle >= 225 && angle < 315) label = 'Left';
            this.orientationLabel.textContent = label;
>>>>>>> a290e832b0766f3ef1f7a8fe802fa37b0ec08a9e
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
}

const display = new Display();
