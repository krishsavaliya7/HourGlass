/**
 * Display Visualization Module
 * Handles LED matrix and orientation visualization
 */

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
        
        // Wait for DOM to be ready
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', () => this.initDisplay());
        } else {
            this.initDisplay();
        }
    }
    
    /**
     * Initialize display components from DOM
     */
    initDisplay() {
        this.matrixStackElement = document.getElementById('matrix-display');
        this.matrixAElement = document.getElementById('matrix-a');
        this.matrixBElement = document.getElementById('matrix-b');
        this.orientationArrow = document.querySelector('.orientation-arrow');
        this.orientationAngle = document.getElementById('orientation-angle');
        this.orientationLabel = document.getElementById('orientation-label');
        
        if (this.matrixStackElement && this.matrixAElement && this.matrixBElement) {
            this.initMatrix();
        } else {
            console.warn('Matrix display elements not found in DOM');
        }
    }

    /**
     * Initialize LED matrix display
     */
    initMatrix() {
        this.ensureMatrixElements();

        this.create8x8Grid(this.matrixAElement, this.matrixCellsA);
        this.create8x8Grid(this.matrixBElement, this.matrixCellsB);
    }

    /**
     * Ensure DOM elements exist (for resilience if markup changes)
     */
    ensureMatrixElements() {
        if (!this.matrixStackElement) return;

        // Ensure stack styling is applied even if HTML is older
        this.matrixStackElement.classList.add('led-matrix-stack');

        // If older markup exists (single grid), upgrade it in-place.
        if (!this.matrixAElement) {
            this.matrixAElement = document.createElement('div');
            this.matrixAElement.id = 'matrix-a';
            this.matrixAElement.className = 'led-matrix';
            this.matrixAElement.setAttribute('role', 'img');
            this.matrixAElement.setAttribute('aria-label', 'Top 8x8 matrix (Matrix A)');
            this.matrixStackElement.appendChild(this.matrixAElement);
        }

        // Ensure connector exists between matrices
        const hasConnector = this.matrixStackElement.querySelector('.matrix-connector');
        if (!hasConnector) {
            const connector = document.createElement('div');
            connector.className = 'matrix-connector';
            connector.setAttribute('aria-hidden', 'true');

            // Place it right after matrix A if possible
            if (this.matrixAElement && this.matrixAElement.nextSibling) {
                this.matrixStackElement.insertBefore(connector, this.matrixAElement.nextSibling);
            } else if (this.matrixAElement) {
                this.matrixStackElement.appendChild(connector);
            } else {
                this.matrixStackElement.prepend(connector);
            }
        }

        if (!this.matrixBElement) {
            this.matrixBElement = document.createElement('div');
            this.matrixBElement.id = 'matrix-b';
            this.matrixBElement.className = 'led-matrix';
            this.matrixBElement.setAttribute('role', 'img');
            this.matrixBElement.setAttribute('aria-label', 'Bottom 8x8 matrix (Matrix B)');
            this.matrixStackElement.appendChild(this.matrixBElement);
        }
    }

    /**
     * Create an 8x8 LED grid inside a container
     * @param {HTMLElement} container
     * @param {HTMLElement[]} cellStore Row-major list of cells
     */
    create8x8Grid(container, cellStore) {
        if (!container) return;
        container.innerHTML = '';
        cellStore.length = 0;

        for (let row = 0; row < 8; row++) {
            for (let col = 0; col < 8; col++) {
                const cell = document.createElement('div');
                cell.className = 'led-cell';
                cell.dataset.row = row;
                cell.dataset.col = col;
                cellStore.push(cell);
                container.appendChild(cell);
            }
        }
        
        // Verify grid was created
        if (cellStore.length !== 64) {
            console.error(`Grid creation failed: expected 64 cells, got ${cellStore.length}`);
        }
    }

    /**
     * Update LED matrix display
     * @param {Array} matrixA - 8x8 matrix for first display (Top)
     * @param {Array} matrixB - 8x8 matrix for second display (Bottom)
     */
    updateMatrix(matrixA, matrixB) {
        this.update8x8Cells(this.matrixCellsA, matrixA);
        this.update8x8Cells(this.matrixCellsB, matrixB);
    }

    /**
     * Update a cached 8x8 grid cell list from a 2D matrix
     * @param {HTMLElement[]} cellStore
     * @param {boolean[][]} matrix
     */
    update8x8Cells(cellStore, matrix) {
        if (!cellStore || cellStore.length !== 64) return;
        for (let row = 0; row < 8; row++) {
            for (let col = 0; col < 8; col++) {
                const idx = row * 8 + col;
                const cell = cellStore[idx];
                const isActive = Boolean(matrix && matrix[row] && matrix[row][col]);
                cell.classList.toggle('active', isActive);
            }
        }
    }

    /**
     * Update matrix from API display data
     */
    updateFromAPI(displayData) {
        if (!displayData) return;

        // Convert flat array to 2D if needed (defaults to all-off if missing)
        const matrixA = this.normalizeMatrix(displayData.matrixA);
        const matrixB = this.normalizeMatrix(displayData.matrixB);
        this.updateMatrix(matrixA, matrixB);
    }

    /**
     * Normalize matrix data to 8x8 format
     */
    normalizeMatrix(data) {
        const empty = Array.from({ length: 8 }, () => Array(8).fill(false));
        if (!Array.isArray(data) || data.length === 0) {
            return empty;
        }

        // Already 2D-ish
        if (Array.isArray(data[0])) {
            const matrix = Array.from({ length: 8 }, (_, row) =>
                Array.from({ length: 8 }, (_, col) => Boolean(data?.[row]?.[col]))
            );
            return matrix;
        }

        // Flat array to 2D
        const matrix = [];
        for (let row = 0; row < 8; row++) {
            matrix[row] = [];
            for (let col = 0; col < 8; col++) {
                const index = row * 8 + col;
                matrix[row][col] = Boolean(data[index]);
            }
        }
        return matrix;
    }

    /**
     * Update orientation display
     */
    updateOrientation(orientationData) {
        if (!orientationData) return;

        const angle = orientationData.angle || 0;
        const x = orientationData.x || 0;
        const y = orientationData.y || 0;

        // Update arrow rotation
        if (this.orientationArrow) {
            this.orientationArrow.style.transform = 
                `translate(-50%, -50%) rotate(${angle}deg)`;
        }

        // Update angle display
        if (this.orientationAngle) {
            this.orientationAngle.textContent = `${Math.round(angle)}°`;
        }

        // Update label (90° is default upright position)
        if (this.orientationLabel) {
            let label = 'Unknown';
            if (angle >= 45 && angle < 135) {
                label = 'Upright';      // 90° default position
            } else if (angle >= 135 && angle < 225) {
                label = 'Right';        // 180°
            } else if (angle >= 225 && angle < 315) {
                label = 'Upside Down';  // 270° flipped
            } else {
                label = 'Left';         // 0° / 360°
            }
            this.orientationLabel.textContent = label;
        }
    }

    /**
     * Clear display
     */
    clear() {
        [...this.matrixCellsA, ...this.matrixCellsB].forEach(cell => cell.classList.remove('active'));
    }

    /**
     * Show loading animation
     */
    showLoading() {
        if (this.matrixStackElement) {
            this.matrixStackElement.classList.add('loading');
        }
    }

    /**
     * Hide loading animation
     */
    hideLoading() {
        if (this.matrixStackElement) {
            this.matrixStackElement.classList.remove('loading');
        }
    }
    
    /**
     * Show test pattern (for debugging/demo)
     */
    showTestPattern() {
        // Create a test pattern: alternating rows
        const matrixA = [];
        const matrixB = [];
        
        for (let row = 0; row < 8; row++) {
            const rowData = [];
            for (let col = 0; col < 8; col++) {
                // Alternating pattern for test
                rowData.push((row + col) % 2 === 0);
            }
            matrixA.push(rowData);
        }
        
        // Bottom matrix shows a different pattern
        for (let row = 0; row < 8; row++) {
            const rowData = [];
            for (let col = 0; col < 8; col++) {
                // Checkerboard pattern
                rowData.push((row % 2 === 0 && col % 2 === 0) || (row % 2 === 1 && col % 2 === 1));
            }
            matrixB.push(rowData);
        }
        
        this.updateMatrix(matrixA, matrixB);
    }

// Export singleton instance
const display = new Display();

