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
}

const display = new Display();
