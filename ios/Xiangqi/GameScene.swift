import Foundation
import SpriteKit
import GameKit
import XiangqiLib
import UIKit  // Needed for UIAlertController

let totalRows = UInt8(10)
let totalCols = UInt8(9)
let kNoPosition = UInt8(0xFF)
let kTotalPieces = UInt8(32)

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

class GameScene: SKScene {

    var game = xq.Game()

    // tileSize will be computed from a chosen board height.
    var tileSize: CGFloat = 0.0

    // boardOrigin is the bottom-left of the board (in scene coordinates).
    // We will align the board so that its right edge is flush with the scene’s right edge.
    var boardOrigin: CGPoint = .zero

    var pieces: [xq.Position : SKShapeNode] = [:]
    var selectedPosition: xq.Position = kNoPosition

    var possibleMovesMark: [SKShapeNode] = []

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
    func positionToPoint(_ position: xq.Position) -> CGPoint {
        // Our board data is in row‑major order with (0,0) at the top.
        // In the scene, boardOrigin is the bottom‑left.
        let x = boardOrigin.x + CGFloat(xq.Col(position)) * tileSize
        let y = boardOrigin.y + CGFloat(totalRows - 1 - xq.Row(position)) * tileSize
        return CGPoint(x: x, y: y)
    }

    /// Converts a scene point to a board coordinate (col, row) by snapping to the grid.
    /// Returns nil if the point is outside the board’s bounds.
    func pointToPosition(_ point: CGPoint) -> xq.Position {
        let boardWidth = CGFloat(totalCols - 1) * tileSize
        let boardHeight = CGFloat(totalRows - 1) * tileSize

        let relativeX = point.x - boardOrigin.x
        let relativeY = point.y - boardOrigin.y

        // Check that the point is within the board’s rectangle.
        let tsizeHalf = tileSize / 2
        if relativeX < -tsizeHalf || relativeX > boardWidth + tsizeHalf || relativeY < -tsizeHalf || relativeY > boardHeight + tsizeHalf {
            return kNoPosition
        }

        // Snap by rounding to the nearest grid index.
        let col = UInt8(round(relativeX / tileSize))
        // Our board rows are counted from the top. Since our origin is at the bottom,
        // compute row by flipping the relative Y.
        let row = UInt8(Int(totalRows - 1) - Int(round(relativeY / tileSize)))
        return xq.Pos(row, col)
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
                let start = positionToPoint(xq.Pos(0, c))           // top
                let end   = positionToPoint(xq.Pos(totalRows - 1, c)) // bottom
                path.move(to: start)
                path.addLine(to: end)

                let line = SKShapeNode(path: path)
                line.strokeColor = lineColor
                line.lineWidth = 2
                addChild(line)
            } else {
                // For inner columns, draw two segments (with a gap for the river between rows 4 and 5).
                let topPath = CGMutablePath()
                let startTop = positionToPoint(xq.Pos(0, c))
                let endTop = positionToPoint(xq.Pos(4, c))
                topPath.move(to: startTop)
                topPath.addLine(to: endTop)

                let topLine = SKShapeNode(path: topPath)
                topLine.strokeColor = .black
                topLine.lineWidth = 2
                addChild(topLine)

                let bottomPath = CGMutablePath()
                let startBottom = positionToPoint(xq.Pos(5, c))
                let endBottom = positionToPoint(xq.Pos(totalRows - 1, c))
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
        let blackPalaceTopLeft     = positionToPoint(xq.Pos(0, 3))
        let blackPalaceTopRight    = positionToPoint(xq.Pos(0, 5))
        let blackPalaceBottomLeft  = positionToPoint(xq.Pos(2, 3))
        let blackPalaceBottomRight = positionToPoint(xq.Pos(2, 5))

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
        let redPalaceTopLeft     = positionToPoint(xq.Pos(7, 3))
        let redPalaceTopRight    = positionToPoint(xq.Pos(7, 5))
        let redPalaceBottomLeft  = positionToPoint(xq.Pos(9, 3))
        let redPalaceBottomRight = positionToPoint(xq.Pos(9, 5))

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
        for row in 0..<UInt8(totalRows) {
            for col in 0..<UInt8(totalCols) {
                let pos = xq.Pos(row, col)
                let piece = game.PieceAt(pos)
                if piece != .EMPTY {
                    createPieceNode(piece, pos)
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

        let tappedPos = pointToPosition(location)
        if tappedPos == kNoPosition {
            return // Touch is outside board.
        }

        // Touching board:
        if !humanVsHuman && game.Turn() != .RED { return }

        let tappedPiece = game.PieceAt(tappedPos)
        let tappedPieceNode = pieces[tappedPos]

        if tappedPiece != .EMPTY {
            if selectedPosition == tappedPos {
                // Selecting the same piece, deselect.
                clearSelection()
            } else if (game.Turn() == .RED && xq.IsRed(tappedPiece)) || (game.Turn() == .BLACK && xq.IsBlack(tappedPiece)) {
                // Selecting a different self piece, change selection.
                let selectedNode = pieces[tappedPos]
                selectedNode?.strokeColor = pieceStrokeColor
                tappedPieceNode?.strokeColor = pieceStrokeSelectedColor
                selectedPosition = tappedPos
                drawPossibleMoves(tappedPos)
            } else if selectedPosition != kNoPosition && ((game.Turn() == .RED && xq.IsBlack(tappedPiece)) || (game.Turn() == .BLACK && xq.IsRed(tappedPiece))) {
                // Selecting an opponent piece, check possible moves.
                let possibleMoves = game.PossibleMoves(selectedPosition)
                if possibleMoves[Int(xq.Row(tappedPos))][Int(xq.Col(tappedPos))] {
                    move(from: selectedPosition, to: tappedPos)
                    movesTs.append(Date())
                    saveGame()
                }
                clearSelection()
                updateStatusLabels()
            }
        } else if selectedPosition != kNoPosition {
            // Move to empty space.
            let possibleMoves = game.PossibleMoves(selectedPosition)
            if possibleMoves[Int(xq.Row(tappedPos))][Int(xq.Col(tappedPos))] {
                move(from: selectedPosition, to: tappedPos)
                movesTs.append(Date())
                saveGame()
            }
            clearSelection()
            updateStatusLabels()
        }
    }


    func reset() {
        if game.MovesCount() <= 0 { return }

        saveGame()
        movesTs.removeAll()
        game.Reset()

        clearSelection()
        clearPossibleMovesMark()

        for piece in pieces.values {
            piece.removeFromParent()
        }
        pieces.removeAll()
        pieces.reserveCapacity(Int(kTotalPieces))
        updateStatusLabels()
        drawPieces()
    }

    func drawPossibleMoves(_ position: xq.Position) {
        func createPossibleMovesMark(_ position: xq.Position) {
            let mark = SKShapeNode(circleOfRadius: 10)
            mark.position = positionToPoint(position)
            mark.fillColor = possibleMoveColor
            mark.zPosition = 5
            possibleMovesMark.append(mark)
            addChild(mark)
        }

        let possibleMoves = game.PossibleMoves(position)
        for row in 0..<totalRows {
            for col in 0..<totalCols {
                if (!possibleMoves[Int(row)][Int(col)]) {
                    continue;
                }
                let possiblePos = xq.Pos(row, col)
                let capture = game.PieceAt(possiblePos)
                if capture != .EMPTY {
                    pieces[possiblePos]?.fillColor = pieceColorThreatened
                } else {
                    createPossibleMovesMark(possiblePos)
                }
            }
        }
    }

    func clearPossibleMovesMark() {
        for mark in possibleMovesMark {
            mark.removeFromParent()
        }
        self.possibleMovesMark.removeAll()
        for node in pieces.values {
            node.fillColor = pieceColor
        }
    }

    func clearSelection() {
        pieces[selectedPosition]?.strokeColor = pieceStrokeColor
        selectedPosition = kNoPosition
        clearPossibleMovesMark()
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
                    move(from: UInt8((m & 0xFF00) >> 8), to: UInt8(m & 0x00FF), animated: false)
                }
            } catch {
                print("Error decoding game data: \(error)")
            }
        }
    }

    func move(from: xq.Position, to: xq.Position, animated: Bool = true) {
        clearSelection()
        clearPossibleMovesMark()
        

        let dest = positionToPoint(to)
        let action = SKAction.move(to: dest, duration: animated ? 0.25 : 0.0)
        let captured = game.Move(from, to)
        if captured != .EMPTY {
            pieces[to]?.zPosition = 1
            removePieceNode(to)
        }
        let node = pieces[from]
        pieces.removeValue(forKey: from)
        pieces[to] = node
        node?.zPosition = 2
        node?.run(action)
    }

    func undo() {
        if !game.CanUndo() { return }
        
        clearSelection()
        clearPossibleMovesMark()

        func undoSingleMove() {
            let undoneMove = game.Undo()
            let _ = movesTs.popLast()
            let action = SKAction.move(to: positionToPoint(undoneMove.from), duration: 0.25)
            let node = pieces.removeValue(forKey: undoneMove.to)
            pieces[undoneMove.from] = node
            node?.run(action)
            if undoneMove.captured != .EMPTY {
                createPieceNode(undoneMove.captured, undoneMove.to)
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
        saveGame()
    }

    func createPieceNode(_ piece: xq.Piece, _ position: xq.Position) {
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
        let node = SKShapeNode(circleOfRadius: pieceRadius)
        node.position = positionToPoint(position)
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
        pieces[position] = node
    }
    
    func removePieceNode(_ position: xq.Position) {
        pieces[position]?.removeFromParent()
        pieces.removeValue(forKey: position)
    }
}
