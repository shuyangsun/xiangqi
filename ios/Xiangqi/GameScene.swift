import Foundation
import SpriteKit
import GameKit
import XiangqiLib
import UIKit  // Needed for UIAlertController

struct GameData: Codable {
    var board: [[Int8]]
    var moves: [UInt16]
    // We can encode Dates as ISO8601 strings or as timestamps.
    // For simplicity, we encode them as Doubles (time intervals).
    var movesTs: [TimeInterval]
    var isOver: Bool
    var isVsHuman: Bool
    var winner: Int8

    // A custom initializer to convert [Date] to [TimeInterval].
    init(board: [[Int8]], moves: [UInt16], movesTs: [Date], isOver: Bool, isVsHuman: Bool, winner: Int8) {
        self.board = board
        self.moves = moves
        self.movesTs = movesTs.map { $0.timeIntervalSince1970 }
        self.isOver = isOver
        self.isVsHuman = isVsHuman
        self.winner = winner
    }
}

enum UniquePiece {
    case empty
    case rGeneral
    case rAdvisor1
    case rAdvisor2
    case rElephant1
    case rElephant2
    case rHorse1
    case rHorse2
    case rChariot1
    case rChariot2
    case rCannon1
    case rCannon2
    case rSoldier1
    case rSoldier2
    case rSoldier3
    case rSoldier4
    case rSoldier5
    case bGeneral
    case bAdvisor1
    case bAdvisor2
    case bElephant1
    case bElephant2
    case bHorse1
    case bHorse2
    case bChariot1
    case bChariot2
    case bCannon1
    case bCannon2
    case bSoldier1
    case bSoldier2
    case bSoldier3
    case bSoldier4
    case bSoldier5
}

func pieceAndCountToUniquePiece(_ piece: xq.Piece, _ pieceCount: UInt8) -> UniquePiece {
    switch (piece, pieceCount) {
    case (.EMPTY, _):
        return .empty;
    case (.R_GENERAL, _):
        return .rGeneral;
    case (.R_ADVISOR, 0):
        return .rAdvisor1;
    case (.R_ADVISOR, 1):
        return .rAdvisor2;
    case (.R_ELEPHANT, 0):
        return .rElephant1;
    case (.R_ELEPHANT, 1):
        return .rElephant2;
    case (.R_HORSE, 0):
        return .rHorse1;
    case (.R_HORSE, 1):
        return .rHorse2;
    case (.R_CHARIOT, 0):
        return .rChariot1;
    case (.R_CHARIOT, 1):
        return .rChariot2;
    case (.R_CANNON, 0):
        return .rCannon1;
    case (.R_CANNON, 1):
        return .rCannon1;
    case (.R_SOLDIER, 0):
        return .rSoldier1;
    case (.R_SOLDIER, 1):
        return .rSoldier2;
    case (.R_SOLDIER, 2):
        return .rSoldier3;
    case (.R_SOLDIER, 3):
        return .rSoldier4;
    case (.R_SOLDIER, 4):
        return .rSoldier5;
    case (.B_GENERAL, _):
        return .bGeneral;
    case (.B_ADVISOR, 0):
        return .bAdvisor1;
    case (.B_ADVISOR, 1):
        return .bAdvisor2;
    case (.B_ELEPHANT, 0):
        return .bElephant1;
    case (.B_ELEPHANT, 1):
        return .bElephant2;
    case (.B_HORSE, 0):
        return .bHorse1;
    case (.B_HORSE, 1):
        return .bHorse2;
    case (.B_CHARIOT, 0):
        return .bChariot1;
    case (.B_CHARIOT, 1):
        return .bChariot2;
    case (.B_CANNON, 0):
        return .bCannon1;
    case (.B_CANNON, 1):
        return .bCannon1;
    case (.B_SOLDIER, 0):
        return .bSoldier1;
    case (.B_SOLDIER, 1):
        return .bSoldier2;
    case (.B_SOLDIER, 2):
        return .bSoldier3;
    case (.B_SOLDIER, 3):
        return .bSoldier4;
    case (.B_SOLDIER, 4):
        return .bSoldier5;
    default:
        return .empty;
    }
}

class GameScene: SKScene {

    var game = xq.Game()

    // Board dimensions (number of columns and rows in our data structure)
    let totalRows = UInt8(10)
    let totalCols = UInt8(9)

    let boardColor = SKColor(red: 0.91, green: 0.82, blue: 0.64, alpha: 1.0)
    let lineColor = SKColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
    let pieceColor = SKColor(red: 0.80, green: 0.65, blue: 0.57, alpha: 1.0)
    let pieceColorThreatened = SKColor(red: 0.8, green: 0.2, blue: 0.2, alpha: 0.9)
    let pieceStrokeColor = SKColor(red: 0.64, green: 0.5, blue: 0.38, alpha: 1.0)
    let pieceStrokeSelectedColor = SKColor(red: 0.35, green: 0.22, blue: 0.09, alpha: 1.0)
    let redTextColor = SKColor(red: 0.5, green: 0.05, blue: 0.05, alpha: 1.0)
    let blackTextColor = SKColor(red: 0.15, green: 0.12, blue: 0.1, alpha: 1.0)
    let possibleMoveColor = SKColor(red: 0.2, green: 0.2, blue: 0.9, alpha: 1.0)
    let buttonColor = SKColor(red: 0.35, green: 0.70, blue: 0.35, alpha: 1.0)

    // tileSize will be computed from a chosen board height.
    var tileSize: CGFloat = 0.0

    // boardOrigin is the bottom-left of the board (in scene coordinates).
    // We will align the board so that its right edge is flush with the scene’s right edge.
    var boardOrigin: CGPoint = .zero

    var selectedPiece: SKShapeNode?
    var selectedPieceValue: xq.Piece?
    var selectedRow: UInt8?
    var selectedCol: UInt8?
    var possibleMovesMark: [SKShapeNode] = []
    var pieceToNode: [UniquePiece: SKShapeNode] = [:]
    var winnerLabel: SKLabelNode = SKLabelNode(text: "")
    var humanVsHuman: Bool = true
    var movesTs: [Date] = []

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

    // MARK: - Add Buttons

    func addButtons() {
        // Create Undo button (背景 + label)
        let undoSize = CGSize(width: 120, height: 50)
        let undoButton = SKShapeNode(rectOf: undoSize, cornerRadius: 10)
        undoButton.fillColor = buttonColor
        undoButton.strokeColor = .black
        undoButton.lineWidth = 2
        undoButton.name = "undoButton"
        // Position the whole button container.
        undoButton.position = CGPoint(x: boardOrigin.x / 2, y: size.height / 2 + 50)
        addChild(undoButton)

        let undoLabel = SKLabelNode(text: "悔棋")
        undoLabel.fontName = "AvenirNext-Bold"
        undoLabel.fontSize = 36
        undoLabel.fontColor = .black
        undoLabel.verticalAlignmentMode = .center
        // Setting the name here as well ensures a tap on the text is also recognized.
        undoLabel.name = "undoButton"
        undoLabel.position = CGPoint.zero
        undoButton.addChild(undoLabel)

        // Create Reset button (背景 + label)
        let resetSize = CGSize(width: 160, height: 50)
        let resetButton = SKShapeNode(rectOf: resetSize, cornerRadius: 10)
        resetButton.fillColor = .red
        resetButton.strokeColor = .black
        resetButton.lineWidth = 2
        resetButton.name = "resetButton"
        resetButton.position = CGPoint(x: boardOrigin.x / 2, y: size.height / 2 - 50)
        addChild(resetButton)

        let resetLabel = SKLabelNode(text: "重新开始")
        resetLabel.fontName = "AvenirNext-Bold"
        resetLabel.fontSize = 36
        resetLabel.fontColor = .black
        resetLabel.verticalAlignmentMode = .center
        resetLabel.name = "resetButton"
        resetLabel.position = CGPoint.zero
        resetButton.addChild(resetLabel)
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

    // MARK: - Confirmation Alert for Reset

    func showResetConfirmation() {
        // Get the view controller from the scene's view.
        guard let viewController = self.view?.window?.rootViewController else { return }

        let alert = UIAlertController(title: "确认重置", message: "您确定要重新开始游戏吗？", preferredStyle: .alert)

        // Cancel action.
        alert.addAction(UIAlertAction(title: "取消", style: .cancel, handler: nil))

        // Confirm action.
        alert.addAction(UIAlertAction(title: "确认", style: .destructive, handler: { _ in
            self.reset()
        }))

        viewController.present(alert, animated: true, completion: nil)
    }

    // MARK: - Coordinate Transformation

    /// Given board coordinates (with (0,0) at the top‑left), return the scene coordinate.
    func pointForBoardCoordinate(col: UInt8, row: UInt8) -> CGPoint {
        // Our board data is in row‑major order with (0,0) at the top.
        // In the scene, boardOrigin is the bottom‑left.
        let x = boardOrigin.x + CGFloat(col) * tileSize
        let y = boardOrigin.y + CGFloat(totalRows - 1 - row) * tileSize
        return CGPoint(x: x, y: y)
    }

    /// Converts a scene point to a board coordinate (col, row) by snapping to the grid.
    /// Returns nil if the point is outside the board’s bounds.
    func boardCoordinateForPoint(_ point: CGPoint) -> (col: UInt8, row: UInt8)? {
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
        let col = UInt8(round(relativeX / tileSize))
        // Our board rows are counted from the top. Since our origin is at the bottom,
        // compute row by flipping the relative Y.
        let row = UInt8(Int(totalRows - 1) - Int(round(relativeY / tileSize)))
        return (col, row)
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
        for c in 0..<UInt8(totalCols) {
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
        chuLabel.fontName = "AvenirNext-Bold"
        chuLabel.fontSize = 30
        chuLabel.fontColor = lineColor
        chuLabel.position = CGPoint(x: boardOrigin.x + boardWidth * 0.25,
                                    y: boardOrigin.y + 4.5 * tileSize)
        chuLabel.verticalAlignmentMode = .center
        addChild(chuLabel)

        let hanLabel = SKLabelNode(text: "漢界")
        hanLabel.fontName = "AvenirNext-Bold"
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
            case .B_CHARIOT, .R_CHARIOT:
                return "車"
            case .B_HORSE, .R_HORSE:
                return "馬"
            case .B_ELEPHANT:
                return "象"
            case .R_ELEPHANT:
                return "象"
            case .B_ADVISOR:
                return "士"
            case .R_ADVISOR:
                return "仕"
            case .B_GENERAL:
                return "将"
            case .R_GENERAL:
                return "帥"
            case .B_CANNON:
                return "砲"
            case .R_CANNON:
                return "炮"
            case .B_SOLDIER:
                return "卒"
            case .R_SOLDIER:
                return "兵"
            default:
                return "?"
            }
        }

        let pieceRadius = tileSize * 0.4
        func createPieceNode(row: UInt8, col: UInt8, piece: xq.Piece, pieceCount: UInt8) {
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

            if humanVsHuman && piece.rawValue < 0 {
                node.zRotation = CGFloat(Double.pi)
            }

            addChild(node)
            pieceToNode[pieceAndCountToUniquePiece(piece, pieceCount)] = node
        }

        var pieceCount: [xq.Piece : UInt8] = [:]
        for row in 0..<UInt8(totalRows) {
            for col in 0..<UInt8(totalCols) {
                let piece = game.PieceAt(xq.Pos(row, col))
                if piece != .EMPTY {
                    createPieceNode(row: row, col: col, piece: piece, pieceCount: pieceCount[piece] ?? 0)
                    if pieceCount[piece] == nil {
                        pieceCount[piece] = UInt8(1)
                    } else {
                        pieceCount[piece] = pieceCount[piece]! + 1
                    }
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

        // Check if the undo or reset buttons were tapped.
        // (This covers taps on either the button background or its child label.)
        if let nodeName = tappedNode.name, nodeName == "undoButton" || tappedNode.parent?.name == "undoButton" {
            // Get the button container node.
            let buttonNode = tappedNode.name == "undoButton" ? tappedNode : tappedNode.parent!
            // Run a quick scale animation to indicate the tap.
            let scaleAction = SKAction.sequence([
                SKAction.scale(to: 0.9, duration: 0.1),
                SKAction.scale(to: 1.0, duration: 0.1)
            ])
            buttonNode.run(scaleAction) {
                self.undo()
            }
            return
        } else if let nodeName = tappedNode.name, nodeName == "resetButton" || tappedNode.parent?.name == "resetButton" {
            let buttonNode = tappedNode.name == "resetButton" ? tappedNode : tappedNode.parent!
            let scaleAction = SKAction.sequence([
                SKAction.scale(to: 0.9, duration: 0.1),
                SKAction.scale(to: 1.0, duration: 0.1)
            ])
            buttonNode.run(scaleAction) {
                // Only prompt for confirmation if moves have been made.
                if self.game.MovesCount() > 0 {
                    self.showResetConfirmation()
                }
            }
            return
        }

        guard let boardPointMaybe = boardCoordinateForPoint(location) else {
            return // Touch is outside board.
        }

        // Touching board:
        if !humanVsHuman && game.Turn() != .RED { return }
        clearPossibleMovesMark()

        let tappedRow = boardPointMaybe.row
        let tappedCol = boardPointMaybe.col
        let xqPiece = game.PieceAt(xq.Pos(tappedRow, tappedCol))
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
                let possibleMoves = game.PossibleMoves(xq.Pos(selectedRow!, selectedCol!))
                if possibleMoves[Int(tappedRow)][Int(tappedCol)] {
                    move(fromRow: selectedRow!, fromCol: selectedCol!, toRow: tappedRow, toCol: tappedCol)
                    movesTs.append(Date())
                }
                clearSelection()
                updateStatusLabels()
            }
        } else if selectedPiece != nil {
            // Move to empty space.
            let possibleMoves = game.PossibleMoves(xq.Pos(UInt8(selectedRow!), UInt8(selectedCol!)))
            if possibleMoves[Int(tappedRow)][Int(tappedCol)] {
                move(fromRow: selectedRow!, fromCol: selectedCol!, toRow: tappedRow, toCol: tappedCol)
                movesTs.append(Date())
            }
            clearSelection()
            updateStatusLabels()
        }
        saveGame()
    }


    func reset() {
        if game.MovesCount() <= 0 { return }

        saveGame()
        movesTs.removeAll()
        game.Reset()

        clearSelection()
        clearPossibleMovesMark()

        for row in 0..<totalRows {
            for col in 0..<totalCols {
                let piece = game.PieceAt(xq.Pos(UInt8(row), UInt8(col)))
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
        func createPossibleMovesMark(row: UInt8, col: UInt8) {
            let mark = SKShapeNode(circleOfRadius: 10)
            mark.position = pointForBoardCoordinate(col: col, row: row)
            mark.fillColor = possibleMoveColor
            mark.zPosition = 5
            possibleMovesMark.append(mark)
            addChild(mark)
        }

        let possibleMoves = self.game.PossibleMoves(xq.Pos(row, col))
        for row in 0..<totalRows {
            for col in 0..<totalCols {
                if (!possibleMoves[Int(row)][Int(col)]) {
                    continue;
                }
                let capture = game.PieceAt(xq.Pos(row, col))
                if capture != .EMPTY {
                    pieceToNode[capture]?.fillColor = pieceColorThreatened
                } else {
                    createPossibleMovesMark(row: row, col: col)
                }
            }
        }
    }

    func clearPossibleMovesMark() {
        for mark in self.possibleMovesMark {
            mark.removeFromParent()
        }
        self.possibleMovesMark.removeAll()
        for node in self.pieceToNode.values {
            node.fillColor = pieceColor
        }
    }

    func clearSelection() {
        self.selectedPiece?.strokeColor = pieceStrokeColor
        self.selectedRow = nil
        self.selectedCol = nil
        self.selectedPiece = nil
        self.selectedPieceValue = nil
    }

    func saveGame() {
        if game.MovesCount() <= 0 { return }
        func exportGameData() -> GameData {
            assert(game.MovesCount() == movesTs.count)
            var moves: [UInt16] = []
            moves.reserveCapacity(game.MovesCount())
            for m in game.ExportMoves() {
                moves.append(m)
            }
            var startingBoard: [[Int8]] = []
            let startingGame = xq.Game()
            startingBoard.reserveCapacity(Int(totalRows))
            for row in 0..<UInt8(totalRows) {
                startingBoard.append(Array(repeating: 0, count: Int(totalCols)))
                for col in 0..<UInt8(totalCols) {
                    startingBoard[Int(row)][Int(col)] = Int8(startingGame.PieceAt(xq.Pos(row, col)).rawValue)
                }
            }
            return GameData(
                board: startingBoard,
                moves: moves,
                movesTs: movesTs,
                isOver: game.IsGameOver(),
                isVsHuman: humanVsHuman,
                winner: game.GetWinner().rawValue
            )
        }

        let gameData = exportGameData()
        do {
            let historyKey = "xiangqi_history"
            let gameKeyPrefix = "game_"
            var histories: [String] = []

            // 3. Save to iCloud using NSUbiquitousKeyValueStore.
            let keyStore = NSUbiquitousKeyValueStore.default
            // Try to load existing histories
            if let data = keyStore.data(forKey: historyKey) {
                do {
                    histories = try JSONDecoder().decode([String].self, from: data)
                } catch {
                    print("Error decoding histories: \(error)")
                }
            }

            let encoder = JSONEncoder()
            // Optionally configure the encoder:
            // encoder.dateEncodingStrategy = .iso8601   // if you were encoding Dates directly
            let encodedData = try encoder.encode(gameData)

            let gameName = "\(gameKeyPrefix)\(Int(gameData.movesTs[0]))"
            if histories.last != gameName {
                histories.append(gameName)
                let encodedHistories = try JSONEncoder().encode(histories)
                keyStore.set(encodedHistories, forKey: historyKey)
            }
            keyStore.set(encodedData, forKey: gameName)
            keyStore.synchronize()
        } catch {
            print("Error encoding game data: \(error)")
        }
    }

    func loadMostRecentGameIfNotOver() {
        let historyKey = "xiangqi_history"
        var histories: [String] = []

        // 3. Save to iCloud using NSUbiquitousKeyValueStore.
        let keyStore = NSUbiquitousKeyValueStore.default
        // Try to load existing histories
        if let data = keyStore.data(forKey: historyKey) {
            do {
                histories = try JSONDecoder().decode([String].self, from: data)
            } catch {
                print("Error decoding histories: \(error)")
            }
        }

        if histories.isEmpty {
            return
        }

        let lastGameName = histories.last!
        if let data = keyStore.data(forKey: lastGameName) {
            do {
                let loadedGame = try JSONDecoder().decode(GameData.self, from: data)
                if loadedGame.isOver {
                    return
                }
                humanVsHuman = loadedGame.isVsHuman
                movesTs = loadedGame.movesTs.map {Date(timeIntervalSince1970: $0)}
                game.Reset() // TODO: should be resetting with loaded board, assuming default board.
                for m in loadedGame.moves {
                    move(
                        fromRow: UInt8((m & 0xF000) >> 12),
                        fromCol: UInt8((m & 0x0F00) >> 8),
                        toRow: UInt8((m & 0x00F0) >> 4),
                        toCol: UInt8(m & 0x000F),
                        animated: false
                    )
                }
            } catch {
                print("Error decoding game data: \(error)")
            }
        }
    }

    func move(fromRow: UInt8, fromCol: UInt8, toRow: UInt8, toCol: UInt8, animated: Bool = true) {
        let dest = pointForBoardCoordinate(col: toCol, row: toRow)
        let action = SKAction.move(to: dest, duration: animated ? 0.25 : 0.0)
        let piece = game.PieceAt(xq.Pos(UInt8(fromRow), UInt8(fromCol)))
        let captured = game.Move(
            xq.Pos(UInt8(fromRow), UInt8(fromCol)),
            xq.Pos(UInt8(toRow), UInt8(toCol))
        )
        if captured != .EMPTY {
            let capturedNode = pieceToNode[captured]
            capturedNode?.zPosition = 0
            capturedNode?.isHidden = true
        }
        let node = pieceToNode[piece]
        node?.zPosition = 1
        node?.run(action)
    }

    func undo() {
        if !game.CanUndo() { return }

        func undoSingleMove() {
            let undoneMove = game.Undo()
            let _ = movesTs.popLast()
            let dest = pointForBoardCoordinate(col: xq.Col(undoneMove.from), row: xq.Col(undoneMove.from))
            let action = SKAction.move(to: dest, duration: 0.25)
            pieceToNode[undoneMove.piece]?.run(action)
            if undoneMove.captured != .EMPTY {
                pieceToNode[undoneMove.captured]?.isHidden = false
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

        clearPossibleMovesMark()
        clearSelection()
        updateStatusLabels()
        saveGame()
    }
}
