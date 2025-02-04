import SpriteKit

class GameScene: SKScene {
    
    // Constants for the board dimensions.
    let tileSize: CGFloat = 40.0
    let totalCols = 9    // Number of columns (files)
    let totalRows = 10   // Number of rows (ranks)
    
    // The boardOrigin is the bottom-left point of the board (in scene coordinates).
    // (Even though our board data structure uses (0, 0) as top-left.)
    var boardOrigin: CGPoint = .zero
    
    override func didMove(to view: SKView) {
        backgroundColor = SKColor(red: 0.95, green: 0.87, blue: 0.70, alpha: 1.0)  // light wooden color
        
        // The board’s physical size (in points). (There are totalCols-1 intervals horizontally and totalRows-1 intervals vertically.)
        let boardWidth = CGFloat(totalCols - 1) * tileSize
        let boardHeight = CGFloat(totalRows - 1) * tileSize
        
        // Center the board in the scene.
        boardOrigin = CGPoint(x: (size.width - boardWidth) / 2,
                              y: (size.height - boardHeight) / 2)
        
        drawBoard()
        drawPieces()
    }
    
    // MARK: - Coordinate Transformation
    
    /// Converts board coordinates (col, row) into scene coordinates.
    /// - Parameters:
    ///   - col: The column index (0 is leftmost).
    ///   - row: The row index (0 is the top row, totalRows-1 is the bottom).
    /// - Returns: The corresponding CGPoint in the scene.
    func pointForBoardCoordinate(col: Int, row: Int) -> CGPoint {
        // In the scene, the boardOrigin is at the bottom-left.
        // Since our board indexing starts at the top, we “flip” the row.
        let x = boardOrigin.x + CGFloat(col) * tileSize
        let y = boardOrigin.y + CGFloat((totalRows - 1) - row) * tileSize
        return CGPoint(x: x, y: y)
    }
    
    // MARK: - Board Drawing
    
    func drawBoard() {
        let boardWidth = CGFloat(totalCols - 1) * tileSize
        let boardHeight = CGFloat(totalRows - 1) * tileSize
        
        // 1. Draw horizontal lines.
        // There are totalRows horizontal lines. (Row 0 is at the top.)
        for r in 0..<totalRows {
            let path = CGMutablePath()
            // Compute the y-coordinate for this board row.
            let y = boardOrigin.y + CGFloat((totalRows - 1) - r) * tileSize
            let start = CGPoint(x: boardOrigin.x, y: y)
            let end = CGPoint(x: boardOrigin.x + boardWidth, y: y)
            path.move(to: start)
            path.addLine(to: end)
            
            let line = SKShapeNode(path: path)
            line.strokeColor = .black
            line.lineWidth = 2
            addChild(line)
        }
        
        // 2. Draw vertical lines.
        // There are totalCols vertical lines.
        for c in 0..<totalCols {
            // For the border columns, draw a full vertical line (from top (row 0) to bottom (row totalRows-1)).
            if c == 0 || c == totalCols - 1 {
                let path = CGMutablePath()
                let start = pointForBoardCoordinate(col: c, row: 0)           // top
                let end   = pointForBoardCoordinate(col: c, row: totalRows - 1) // bottom
                path.move(to: start)
                path.addLine(to: end)
                
                let line = SKShapeNode(path: path)
                line.strokeColor = .black
                line.lineWidth = 2
                addChild(line)
            } else {
                // For inner columns, draw two segments with a gap for the river.
                // The gap is between board rows 4 and 5.
                
                // Top segment: from row 0 (top) to row 4.
                let topPath = CGMutablePath()
                let startTop = pointForBoardCoordinate(col: c, row: 0)
                let endTop = pointForBoardCoordinate(col: c, row: 4)
                topPath.move(to: startTop)
                topPath.addLine(to: endTop)
                
                let topLine = SKShapeNode(path: topPath)
                topLine.strokeColor = .black
                topLine.lineWidth = 2
                addChild(topLine)
                
                // Bottom segment: from row 5 to row totalRows-1 (bottom).
                let bottomPath = CGMutablePath()
                let startBottom = pointForBoardCoordinate(col: c, row: 5)
                let endBottom = pointForBoardCoordinate(col: c, row: totalRows - 1)
                bottomPath.move(to: startBottom)
                bottomPath.addLine(to: endBottom)
                
                let bottomLine = SKShapeNode(path: bottomPath)
                bottomLine.strokeColor = .black
                bottomLine.lineWidth = 2
                addChild(bottomLine)
            }
        }
        
        // 3. Draw the palace diagonals.
        // The black palace is at the top and occupies columns 3-5 and rows 0-2.
        let blackPalaceTopLeft     = pointForBoardCoordinate(col: 3, row: 0)
        let blackPalaceTopRight    = pointForBoardCoordinate(col: 5, row: 0)
        let blackPalaceBottomLeft  = pointForBoardCoordinate(col: 3, row: 2)
        let blackPalaceBottomRight = pointForBoardCoordinate(col: 5, row: 2)
        
        let blackDiagonal1Path = CGMutablePath()
        blackDiagonal1Path.move(to: blackPalaceTopLeft)
        blackDiagonal1Path.addLine(to: blackPalaceBottomRight)
        let blackDiagonal1 = SKShapeNode(path: blackDiagonal1Path)
        blackDiagonal1.strokeColor = .black
        blackDiagonal1.lineWidth = 2
        addChild(blackDiagonal1)
        
        let blackDiagonal2Path = CGMutablePath()
        blackDiagonal2Path.move(to: blackPalaceTopRight)
        blackDiagonal2Path.addLine(to: blackPalaceBottomLeft)
        let blackDiagonal2 = SKShapeNode(path: blackDiagonal2Path)
        blackDiagonal2.strokeColor = .black
        blackDiagonal2.lineWidth = 2
        addChild(blackDiagonal2)
        
        // The red palace is at the bottom and occupies columns 3-5 and rows 7-9.
        let redPalaceTopLeft     = pointForBoardCoordinate(col: 3, row: 7)
        let redPalaceTopRight    = pointForBoardCoordinate(col: 5, row: 7)
        let redPalaceBottomLeft  = pointForBoardCoordinate(col: 3, row: 9)
        let redPalaceBottomRight = pointForBoardCoordinate(col: 5, row: 9)
        
        let redDiagonal1Path = CGMutablePath()
        redDiagonal1Path.move(to: redPalaceTopLeft)
        redDiagonal1Path.addLine(to: redPalaceBottomRight)
        let redDiagonal1 = SKShapeNode(path: redDiagonal1Path)
        redDiagonal1.strokeColor = .black
        redDiagonal1.lineWidth = 2
        addChild(redDiagonal1)
        
        let redDiagonal2Path = CGMutablePath()
        redDiagonal2Path.move(to: redPalaceTopRight)
        redDiagonal2Path.addLine(to: redPalaceBottomLeft)
        let redDiagonal2 = SKShapeNode(path: redDiagonal2Path)
        redDiagonal2.strokeColor = .black
        redDiagonal2.lineWidth = 2
        addChild(redDiagonal2)
        
        // 4. Draw the river labels ("楚河" and "漢界").
        // The gap is between board rows 4 and 5.
        let chuLabel = SKLabelNode(text: "楚河")
        chuLabel.fontSize = 30
        chuLabel.fontColor = .black
        chuLabel.position = CGPoint(x: boardOrigin.x + boardWidth * 0.25,
                                    y: boardOrigin.y + 4.5 * tileSize)
        chuLabel.verticalAlignmentMode = .center
        addChild(chuLabel)
        
        let hanLabel = SKLabelNode(text: "漢界")
        hanLabel.fontSize = 30
        hanLabel.fontColor = .black
        hanLabel.position = CGPoint(x: boardOrigin.x + boardWidth * 0.75,
                                    y: boardOrigin.y + 4.5 * tileSize)
        hanLabel.verticalAlignmentMode = .center
        addChild(hanLabel)
    }
    
    // MARK: - Piece Setup
    
    func drawPieces() {
        // Use a circle to represent a piece.
        let pieceRadius = tileSize * 0.4
        
        /// A helper to create and add a piece node.
        /// - Parameters:
        ///   - col: The column index.
        ///   - row: The row index (remember: 0 is top, totalRows-1 is bottom).
        ///   - text: The Xiangqi character.
        ///   - isRed: Whether the piece is red (self) or black (opponent).
        func createPieceNode(col: Int, row: Int, text: String, isRed: Bool) {
            let piece = SKShapeNode(circleOfRadius: pieceRadius)
            piece.position = pointForBoardCoordinate(col: col, row: row)
            piece.fillColor = isRed ? .red : .black
            piece.strokeColor = .white
            piece.lineWidth = 2
            
            let label = SKLabelNode(text: text)
            label.fontSize = pieceRadius
            label.fontColor = .white
            label.verticalAlignmentMode = .center
            label.horizontalAlignmentMode = .center
            piece.addChild(label)
            
            addChild(piece)
        }
        
        // --- Black Pieces (opponent) ---
        // With row-major indexing (top-left is (0,0)), black's back row is row 0.
        // Back row (row 0)
        createPieceNode(col: 0, row: 0, text: "車", isRed: false)
        createPieceNode(col: 8, row: 0, text: "車", isRed: false)
        // Horses
        createPieceNode(col: 1, row: 0, text: "馬", isRed: false)
        createPieceNode(col: 7, row: 0, text: "馬", isRed: false)
        // Elephants
        createPieceNode(col: 2, row: 0, text: "象", isRed: false)
        createPieceNode(col: 6, row: 0, text: "象", isRed: false)
        // Advisors
        createPieceNode(col: 3, row: 0, text: "士", isRed: false)
        createPieceNode(col: 5, row: 0, text: "士", isRed: false)
        // General
        createPieceNode(col: 4, row: 0, text: "將", isRed: false)
        // Cannons (row 2)
        createPieceNode(col: 1, row: 2, text: "砲", isRed: false)
        createPieceNode(col: 7, row: 2, text: "砲", isRed: false)
        // Soldiers (row 3)
        createPieceNode(col: 0, row: 3, text: "卒", isRed: false)
        createPieceNode(col: 2, row: 3, text: "卒", isRed: false)
        createPieceNode(col: 4, row: 3, text: "卒", isRed: false)
        createPieceNode(col: 6, row: 3, text: "卒", isRed: false)
        createPieceNode(col: 8, row: 3, text: "卒", isRed: false)
        
        // --- Red Pieces (self) ---
        // With the new indexing, red's back row is at the bottom (row 9).
        // Back row (row 9)
        createPieceNode(col: 0, row: 9, text: "車", isRed: true)
        createPieceNode(col: 8, row: 9, text: "車", isRed: true)
        // Horses
        createPieceNode(col: 1, row: 9, text: "馬", isRed: true)
        createPieceNode(col: 7, row: 9, text: "馬", isRed: true)
        // Elephants
        createPieceNode(col: 2, row: 9, text: "相", isRed: true)
        createPieceNode(col: 6, row: 9, text: "相", isRed: true)
        // Advisors
        createPieceNode(col: 3, row: 9, text: "仕", isRed: true)
        createPieceNode(col: 5, row: 9, text: "仕", isRed: true)
        // General
        createPieceNode(col: 4, row: 9, text: "帥", isRed: true)
        // Cannons (row 7)
        createPieceNode(col: 1, row: 7, text: "炮", isRed: true)
        createPieceNode(col: 7, row: 7, text: "炮", isRed: true)
        // Soldiers (row 6)
        createPieceNode(col: 0, row: 6, text: "兵", isRed: true)
        createPieceNode(col: 2, row: 6, text: "兵", isRed: true)
        createPieceNode(col: 4, row: 6, text: "兵", isRed: true)
        createPieceNode(col: 6, row: 6, text: "兵", isRed: true)
        createPieceNode(col: 8, row: 6, text: "兵", isRed: true)
    }
}

