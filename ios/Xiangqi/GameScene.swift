import SpriteKit
import XiangqiLib

class GameScene: SKScene {
    
    // Board dimensions (number of columns and rows in our data structure)
    let totalRows = 10   // Rows (ranks)
    let totalCols = 9    // Columns (files)
    
    // tileSize will be computed from a chosen board height.
    var tileSize: CGFloat = 0.0
    
    // boardOrigin is the bottom-left of the board (in scene coordinates).
    // We will align the board so that its right edge is flush with the scene’s right edge.
    var boardOrigin: CGPoint = .zero
    
    // Keep track of a selected piece.
    var selectedPiece: SKShapeNode?
    
    // MARK: - Scene Setup
    
    override func didMove(to view: SKView) {
        backgroundColor = SKColor(red: 0.95, green: 0.87, blue: 0.70, alpha: 1.0)  // light wooden color
        
        // Instead of using the full scene height, reserve a margin.
        let boardHeight = size.height * 0.85
        tileSize = boardHeight / CGFloat(totalRows - 1)
        let boardWidth = CGFloat(totalCols - 1) * tileSize
        
        // Position the board:
        // - Horizontally: right edge aligned with scene’s right edge.
        // - Vertically: center the board.
        let boardY = (size.height - boardHeight) / 2
        boardOrigin = CGPoint(x: size.width - boardWidth - tileSize, y: boardY)
        
        drawBoard()
        drawPieces()
    }
    
    // MARK: - Coordinate Transformation
    
    /// Given board coordinates (with (0,0) at the top‑left), return the scene coordinate.
    func pointForBoardCoordinate(col: Int, row: Int) -> CGPoint {
        // Our board data is in row‑major order with (0,0) at the top.
        // In the scene, boardOrigin is the bottom‑left.
        let x = boardOrigin.x + CGFloat(col) * tileSize
        let y = boardOrigin.y + CGFloat((totalRows - 1) - row) * tileSize
        return CGPoint(x: x, y: y)
    }
    
    /// Converts a scene point to a board coordinate (col, row) by snapping to the grid.
    /// Returns nil if the point is outside the board’s bounds.
    func boardCoordinateForPoint(_ point: CGPoint) -> (col: Int, row: Int)? {
        let boardWidth = CGFloat(totalCols - 1) * tileSize
        let boardHeight = CGFloat(totalRows - 1) * tileSize
        
        let relativeX = point.x - boardOrigin.x
        let relativeY = point.y - boardOrigin.y
        
        // Check that the point is within the board’s rectangle.
        if relativeX < 0 || relativeX > boardWidth || relativeY < 0 || relativeY > boardHeight {
            return nil
        }
        
        // Snap by rounding to the nearest grid index.
        let col = Int(round(relativeX / tileSize))
        // Our board rows are counted from the top. Since our origin is at the bottom,
        // compute row by flipping the relative Y.
        let row = (totalRows - 1) - Int(round(relativeY / tileSize))
        return (col, row)
    }
    
    /// Given a scene point, return the nearest grid point on the board.
    func snappedPosition(from point: CGPoint) -> CGPoint? {
        guard let (col, row) = boardCoordinateForPoint(point) else {
            return nil
        }
        return pointForBoardCoordinate(col: col, row: row)
    }
    
    // MARK: - Board Drawing
    
    func drawBoard() {
        let boardWidth = CGFloat(totalCols - 1) * tileSize
        
        // 1. Draw horizontal lines.
        for r in 0..<totalRows {
            let path = CGMutablePath()
            // Calculate y by "flipping" the row.
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
        for c in 0..<totalCols {
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
                // For inner columns, draw two segments (with a gap for the river between rows 4 and 5).
                let topPath = CGMutablePath()
                let startTop = pointForBoardCoordinate(col: c, row: 0)
                let endTop = pointForBoardCoordinate(col: c, row: 4)
                topPath.move(to: startTop)
                topPath.addLine(to: endTop)
                
                let topLine = SKShapeNode(path: topPath)
                topLine.strokeColor = .black
                topLine.lineWidth = 2
                addChild(topLine)
                
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
        // Black palace (top): occupies columns 3-5 and rows 0-2.
        let blackPalaceTopLeft     = pointForBoardCoordinate(col: 3, row: 0)
        let blackPalaceTopRight    = pointForBoardCoordinate(col: 5, row: 0)
        let blackPalaceBottomLeft  = pointForBoardCoordinate(col: 3, row: 2)
        let blackPalaceBottomRight = pointForBoardCoordinate(col: 5, row: 2)
        
        let blackDiagonal1 = SKShapeNode(path: {
            let path = CGMutablePath()
            path.move(to: blackPalaceTopLeft)
            path.addLine(to: blackPalaceBottomRight)
            return path
        }())
        blackDiagonal1.strokeColor = .black
        blackDiagonal1.lineWidth = 2
        addChild(blackDiagonal1)
        
        let blackDiagonal2 = SKShapeNode(path: {
            let path = CGMutablePath()
            path.move(to: blackPalaceTopRight)
            path.addLine(to: blackPalaceBottomLeft)
            return path
        }())
        blackDiagonal2.strokeColor = .black
        blackDiagonal2.lineWidth = 2
        addChild(blackDiagonal2)
        
        // Red palace (bottom): occupies columns 3-5 and rows 7-9.
        let redPalaceTopLeft     = pointForBoardCoordinate(col: 3, row: 7)
        let redPalaceTopRight    = pointForBoardCoordinate(col: 5, row: 7)
        let redPalaceBottomLeft  = pointForBoardCoordinate(col: 3, row: 9)
        let redPalaceBottomRight = pointForBoardCoordinate(col: 5, row: 9)
        
        let redDiagonal1 = SKShapeNode(path: {
            let path = CGMutablePath()
            path.move(to: redPalaceTopLeft)
            path.addLine(to: redPalaceBottomRight)
            return path
        }())
        redDiagonal1.strokeColor = .black
        redDiagonal1.lineWidth = 2
        addChild(redDiagonal1)
        
        let redDiagonal2 = SKShapeNode(path: {
            let path = CGMutablePath()
            path.move(to: redPalaceTopRight)
            path.addLine(to: redPalaceBottomLeft)
            return path
        }())
        redDiagonal2.strokeColor = .black
        redDiagonal2.lineWidth = 2
        addChild(redDiagonal2)
        
        // 4. Draw the river labels ("楚河" and "漢界").
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
        // Use a circle to represent each piece.
        let pieceRadius = tileSize * 0.4
        
        /// Helper function to create a piece node.
        func createPieceNode(col: Int, row: Int, text: String, isRed: Bool) {
            let piece = SKShapeNode(circleOfRadius: pieceRadius)
            piece.position = pointForBoardCoordinate(col: col, row: row)
            piece.fillColor = isRed ? .red : .black
            piece.strokeColor = .white
            piece.lineWidth = 2
            piece.name = "piece"  // Helps us identify it in touch events.
            
            let label = SKLabelNode(fontNamed: "AvenirNext-Bold")
            label.text = text
            label.fontSize = pieceRadius
            label.fontColor = .white
            label.verticalAlignmentMode = .center
            label.horizontalAlignmentMode = .center
            piece.addChild(label)
            
            addChild(piece)
        }
        
        // --- Black Pieces (opponent) ---
        createPieceNode(col: 0, row: 0, text: "車", isRed: false)
        createPieceNode(col: 8, row: 0, text: "車", isRed: false)
        createPieceNode(col: 1, row: 0, text: "馬", isRed: false)
        createPieceNode(col: 7, row: 0, text: "馬", isRed: false)
        createPieceNode(col: 2, row: 0, text: "象", isRed: false)
        createPieceNode(col: 6, row: 0, text: "象", isRed: false)
        createPieceNode(col: 3, row: 0, text: "士", isRed: false)
        createPieceNode(col: 5, row: 0, text: "士", isRed: false)
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
        createPieceNode(col: 0, row: 9, text: "車", isRed: true)
        createPieceNode(col: 8, row: 9, text: "車", isRed: true)
        createPieceNode(col: 1, row: 9, text: "馬", isRed: true)
        createPieceNode(col: 7, row: 9, text: "馬", isRed: true)
        createPieceNode(col: 2, row: 9, text: "相", isRed: true)
        createPieceNode(col: 6, row: 9, text: "相", isRed: true)
        createPieceNode(col: 3, row: 9, text: "仕", isRed: true)
        createPieceNode(col: 5, row: 9, text: "仕", isRed: true)
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
    
    // MARK: - Touch Handling for Moving Pieces
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        
        // Identify the node at the touch location.
        let tappedNode = atPoint(location)
        
        // Check if the tapped node (or its parent) is a piece.
        var pieceNode: SKShapeNode?
        if let name = tappedNode.name, name == "piece", let shape = tappedNode as? SKShapeNode {
            pieceNode = shape
        } else if let parent = tappedNode.parent as? SKShapeNode, parent.name == "piece" {
            pieceNode = parent
        }
        
        if let tappedPiece = pieceNode {
            // If no piece is selected, select this piece.
            if selectedPiece == nil {
                selectedPiece = tappedPiece
                tappedPiece.strokeColor = .yellow
            } else {
                // If the same piece is tapped, deselect it.
                if tappedPiece == selectedPiece {
                    tappedPiece.strokeColor = .white
                    selectedPiece = nil
                } else {
                    // Otherwise, move the selected piece to the tapped piece's grid position.
                    if let destination = snappedPosition(from: tappedPiece.position) {
                        let moveAction = SKAction.move(to: destination, duration: 0.3)
                        selectedPiece?.run(moveAction)
                    }
                    selectedPiece?.strokeColor = .white
                    selectedPiece = nil
                }
            }
        } else {
            // If an empty spot is tapped while a piece is selected,
            // snap the destination to the grid.
            if let selected = selectedPiece, let destination = snappedPosition(from: location) {
                let moveAction = SKAction.move(to: destination, duration: 0.3)
                selected.run(moveAction)
                selected.strokeColor = .white
                selectedPiece = nil
            }
        }
    }
}

