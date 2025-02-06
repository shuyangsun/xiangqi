import UIKit
import SpriteKit
import GameplayKit

class GameViewController: UIViewController {
    
    var scene: GameScene?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Ensure the view is an SKView.
        if let skView = self.view as? SKView {
            // Create an instance of your custom scene.
            // Using the view's bounds to set the scene size.
            scene = GameScene(size: skView.bounds.size)
            
            // Set the scale mode to scale to fit the window.
            scene?.scaleMode = .aspectFill
            
            // Present the scene.
            skView.presentScene(scene)
            
            // Optional: Debug info
            skView.ignoresSiblingOrder = true
            skView.showsFPS = true
            skView.showsNodeCount = true

            scene?.loadMostRecentGameIfNotOver()
        }
    }
    
    // Optional: Hide the status bar for a full-screen experience.
    override var prefersStatusBarHidden: Bool {
        return true
    }
}

