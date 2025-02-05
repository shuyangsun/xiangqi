import Foundation
import SpriteKit
import XiangqiLib

class GameScene: SKScene {
    
    var game = xq.Game()
    
    // Board dimensions (number of columns and rows in our data structure)
    let totalRows = 10
    let totalCols = 9
    
    let boardColor = SKColor(red: 0.91, green: 0.82, blue: 0.64, alpha: 1.0)
    let lineColor = SKColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
    let pieceColor = SKColor(red: 0.80, green: 0.65, blue: 0.57, alpha: 1.0)
    let pieceStrokeColor = SKColor(red: 0.64, green: 0.5, blue: 0.38, alpha: 1.0)
    let pieceStrokeSelectedColor = SKColor(red: 0.35, green: 0.22, blue: 0.09, alpha: 1.0)
    let redTextColor = SKColor(red: 0.5, green: 0.05, blue: 0.05, alpha: 1.0)
    let blackTextColor = SKColor(red: 0.15, green: 0.12, blue: 0.1, alpha: 1.0)
    let possibleMoveColor = SKColor(red: 0.2, green: 0.2, blue: 0.9, alpha: 1.0)

    // tileSize will be computed from a chosen board height.
    var tileSize: CGFloat = 0.0
    
    // boardOrigin is the bottom-left of the board (in scene coordinates).
    // We will align the board so that its right edge is flush with the scene’s right edge.
    var boardOrigin: CGPoint = .zero
    
    var humanVsHuman: Bool = true
    var selectedPiece: SKShapeNode?
    var selectedPieceValue: xq.Piece?
    var selectedRow: Int?
    var selectedCol: Int?
    var possibleMovesMark: [SKShapeNode] = []
    var pieceToNode: [xq.Piece: SKShapeNode] = [:]
    var winnerLabel: SKLabelNode = SKLabelNode(text: "")
    
    // MARK: - Scene Setup
    
    override func didMove(to view: SKView) {
        backgroundColor = boardColor
        
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
        addButtons()
        addStatusLabels()
    }
    
    // MARK: - Add Undo Button
    
    func addButtons() {
        // Create an SKLabelNode to serve as our button.
        let undoButton = SKLabelNode(text: "悔棋")
        undoButton.fontName = "AvenirNext-Bold"
        undoButton.fontSize = 36
        undoButton.fontColor = .black
        undoButton.name = "undoButton"
        undoButton.position = CGPoint(x: boardOrigin.x / 2, y: size.height / 2 + 50)
        addChild(undoButton)
        
        let resetButton = SKLabelNode(text: "重新开始")
        resetButton.fontName = "AvenirNext-Bold"
        resetButton.fontSize = 36
        resetButton.fontColor = .black
        resetButton.name = "resetButton"
        resetButton.position = CGPoint(x: boardOrigin.x / 2, y: size.height / 2 - 50)
        addChild(resetButton)
    }
    
    func addStatusLabels() {
        winnerLabel.fontName = "AvenirNext-Bold"
        winnerLabel.fontSize = 42
        winnerLabel.fontColor = .black
        winnerLabel.name = "winnerLabel"
        winnerLabel.position = CGPoint(x: boardOrigin.x / 2, y: size.height / 2 + 150)
        addChild(winnerLabel)
    }
    
    func updateStatusLabels() {
        if !game.IsGameOver() {
            winnerLabel.text = ""
            return
        }
        let winner = game.GetWinner()
        if winner == .DRAW {
            winnerLabel.fontColor = .black
            winnerLabel.text = "和棋！"
        } else if winner == .RED {
            winnerLabel.fontColor = .red
            winnerLabel.text = "红方胜！"
        } else if winner == .BLACK {
            winnerLabel.fontColor = .black
            winnerLabel.text = "黑方胜！"
        }
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
        let tsizeHalf = tileSize / 2
        if relativeX < -tsizeHalf || relativeX > boardWidth + tsizeHalf || relativeY < -tsizeHalf || relativeY > boardHeight + tsizeHalf {
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
            line.strokeColor = lineColor
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
                line.strokeColor = lineColor
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
        blackDiagonal1.strokeColor = lineColor
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
        redDiagonal1.strokeColor = lineColor
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
        chuLabel.fontColor = lineColor
        chuLabel.position = CGPoint(x: boardOrigin.x + boardWidth * 0.25,
                                    y: boardOrigin.y + 4.5 * tileSize)
        chuLabel.verticalAlignmentMode = .center
        addChild(chuLabel)
        
        let hanLabel = SKLabelNode(text: "漢界")
        hanLabel.fontSize = 30
        hanLabel.fontColor = lineColor
        hanLabel.position = CGPoint(x: boardOrigin.x + boardWidth * 0.75,
                                    y: boardOrigin.y + 4.5 * tileSize)
        hanLabel.verticalAlignmentMode = .center
        addChild(hanLabel)
    }
    
    // MARK: - Piece Setup
    
    func drawPieces() {
        func pieceText(piece: xq.Piece) -> String {
            switch (piece) {
            case .B_CHARIOT_L, .B_CHARIOT_R, .R_CHARIOT_L, .R_CHARIOT_R:
                return "車"
            case .B_HORSE_L, .B_HORSE_R, .R_HORSE_L, .R_HORSE_R:
                return "馬"
            case .B_ELEPHANT_L, .B_ELEPHANT_R:
                return "象"
            case .R_ELEPHANT_L, .R_ELEPHANT_R:
                return "象"
            case .B_ADVISOR_L, .B_ADVISOR_R:
                return "士"
            case .R_ADVISOR_L, .R_ADVISOR_R:
                return "仕"
            case .B_GENERAL:
                return "将"
            case .R_GENERAL:
                return "帥"
            case .B_CANNON_L, .B_CANNON_R:
                return "砲"
            case .R_CANNON_L, .R_CANNON_R:
                return "炮"
            case .B_SOLDIER_1, .B_SOLDIER_2, .B_SOLDIER_3, .B_SOLDIER_4, .B_SOLDIER_5:
                return "卒"
            case .R_SOLDIER_1, .R_SOLDIER_2, .R_SOLDIER_3, .R_SOLDIER_4, .R_SOLDIER_5:
                return "兵"
            default:
                return "?"
            }
        }
        
        let pieceRadius = tileSize * 0.4
        func createPieceNode(row: Int, col: Int, piece: xq.Piece) {
            let node = SKShapeNode(circleOfRadius: pieceRadius)
            node.position = pointForBoardCoordinate(col: col, row: row)
            node.fillColor = pieceColor
            node.strokeColor = pieceStrokeColor
            node.lineWidth = 5
            node.name = "piece_\(piece.rawValue)"
            node.zPosition = 1
            
            let label = SKLabelNode(fontNamed: "AvenirNext-Bold")
            label.text = pieceText(piece: piece)
            label.fontSize = pieceRadius * 1.2
            label.fontColor = piece.rawValue > 0 ? redTextColor : blackTextColor
            label.verticalAlignmentMode = .center
            label.horizontalAlignmentMode = .center
            node.addChild(label)
            
            addChild(node)
            pieceToNode[piece] = node
        }
        
        for row in 0..<totalRows {
            for col in 0..<totalCols {
                let piece = game.PieceAt(xq.Position(row: UInt8(row), col: UInt8(col)))
                if piece != .EMPTY {
                    createPieceNode(row: row, col: col, piece: piece)
                }
            }
        }
    }
    
    // MARK: - Touch Handling for Moving Pieces
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        
        // Identify the node at the touch location.
        let tappedNode = atPoint(location)
        
        // Check if the undo button was tapped.
        if let name = tappedNode.name {
            if name == "undoButton" ||
                tappedNode.parent?.name == "undoButton" {
                undo()
                return
            } else if name == "resetButton" || tappedNode.parent?.name == "resetButton" {
                reset()
                return
            }
        }

        guard let boardPointMaybe = boardCoordinateForPoint(location) else {
            return // Touch is outside board.
        }
        
        // Touching board:
        if !humanVsHuman && game.Turn() != .RED { return }
        clearPossibleMovesMark()
        
        let tappedRow = boardPointMaybe.row
        let tappedCol = boardPointMaybe.col
        let xqPiece = game.PieceAt(xq.Position(row: UInt8(tappedRow), col: UInt8(tappedCol)))
        let tappedPiece = pieceToNode[xqPiece]

        if tappedPiece != nil {
            if selectedPieceValue == xqPiece {
                // Selecting the same piece, deselect.
                clearSelection()
            } else if (game.Turn() == .RED && xqPiece.rawValue > 0) || (game.Turn() == .BLACK && xqPiece.rawValue < 0) {
                // Selecting a different self piece, change selection.
                selectedPiece?.strokeColor = pieceStrokeColor
                tappedPiece!.strokeColor = pieceStrokeSelectedColor
                selectedRow = tappedRow
                selectedCol = tappedCol
                selectedPiece = tappedPiece
                selectedPieceValue = xqPiece
                self.drawPossibleMoves(row: UInt8(tappedRow), col: UInt8(tappedCol))
            } else if selectedPiece != nil && ((game.Turn() == .RED && xqPiece.rawValue < 0) || (game.Turn() == .BLACK && xqPiece.rawValue > 0)) {
                // Selecting an opponent piece, check possible moves.
                let possibleMoves = game.PossibleMoves(xq.Position(row: UInt8(selectedRow!), col: UInt8(selectedCol!)))
                if possibleMoves[tappedRow][tappedCol], let destination = snappedPosition(from: tappedPiece!.position) {
                    let captured = game.Move(
                        xq.Position(row: UInt8(selectedRow!), col: UInt8(selectedCol!)),
                        xq.Position(row: UInt8(tappedRow), col: UInt8(tappedCol))
                    )
                    if captured != .EMPTY {
                        pieceToNode[captured]?.isHidden = true
                    }
                    let moveAction = SKAction.move(to: destination, duration: 0.3)
                    selectedPiece?.run(moveAction)
                }
                clearSelection()
                updateStatusLabels()
            }
        } else if selectedPiece != nil {
            // Move to empty space.
            let possibleMoves = game.PossibleMoves(xq.Position(row: UInt8(selectedRow!), col: UInt8(selectedCol!)))
            if possibleMoves[tappedRow][tappedCol],  let destination = snappedPosition(from: location) {
                let moveAction = SKAction.move(to: destination, duration: 0.3)
                selectedPiece?.run(moveAction)
                game.Move(
                    xq.Position(row: UInt8(selectedRow!), col: UInt8(selectedCol!)),
                    xq.Position(row: UInt8(tappedRow), col: UInt8(tappedCol))
                )
            }
            clearSelection()
            updateStatusLabels()
        }
    }
    
    func undo() {
        if !game.CanUndo() { return }
        
        func undoSingleMove() {
            let undoneMove = game.Undo()
            if let original = snappedPosition(from: pointForBoardCoordinate(col: Int(undoneMove.from.col), row: Int(undoneMove.from.row))) {
                let moveAction = SKAction.move(to: original, duration: 0.3)
                pieceToNode[undoneMove.piece]?.run(moveAction)
                if undoneMove.captured != .EMPTY {
                    pieceToNode[undoneMove.captured]?.isHidden = false
                }
            }
        }
        if humanVsHuman {
            undoSingleMove()
        } else {
            undoSingleMove()
            if game.Turn() != .RED && game.CanUndo() {
                undoSingleMove()
            }
        }
        updateStatusLabels()
    }
    
    func reset() {
        game.Reset()

        clearSelection()
        clearPossibleMovesMark()

        for row in 0..<totalRows {
            for col in 0..<totalCols {
                let piece = game.PieceAt(xq.Position(row: UInt8(row), col: UInt8(col)))
                if piece != .EMPTY {
                    let node = pieceToNode[piece]
                    node?.position = pointForBoardCoordinate(col: col, row: row)
                    node?.isHidden = false
                }
            }
        }
        updateStatusLabels()
    }
    
    func drawPossibleMoves(row: UInt8, col: UInt8) {
        func createPossibleMovesMark(row: Int, col: Int) {
            let mark = SKShapeNode(circleOfRadius: 10)
            mark.position = pointForBoardCoordinate(col: col, row: row)
            mark.fillColor = possibleMoveColor
            mark.zPosition = 5
            possibleMovesMark.append(mark)
            addChild(mark)
        }
        
        let possibleMoves = self.game.PossibleMoves(xq.Position(row: row, col: col))
        for row in 0...9 {
            for col in 0...8 {
                if (!possibleMoves[row][col]) {
                    continue;
                }
                createPossibleMovesMark(row: row, col: col)
            }
        }
    }
    
    func clearPossibleMovesMark() {
        for mark in self.possibleMovesMark {
            mark.removeFromParent()
        }
        self.possibleMovesMark.removeAll()
    }
    
    func clearSelection() {
        self.selectedPiece?.strokeColor = pieceStrokeColor
        self.selectedRow = nil
        self.selectedCol = nil
        self.selectedPiece = nil
        self.selectedPieceValue = nil
    }
}

