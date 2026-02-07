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

    updateMatrix(matrixA, matrixB) {
        this.update8x8Cells(this.matrixCellsA, matrixA);
        this.update8x8Cells(this.matrixCellsB, matrixB);
    }

    update8x8Cells(cellStore, matrix) {
        if (!cellStore || cellStore.length !== 64) return;
        for (let row = 0; row < 8; row++) {
            for (let col = 0; col < 8; col++) {
                const idx = row * 8 + col;
                const isActive = Boolean(matrix?.[row]?.[col]);
                cellStore[idx].classList.toggle('active', isActive);
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
