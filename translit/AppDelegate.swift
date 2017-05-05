import Cocoa
import Foundation


/// App main class
@NSApplicationMain class AppDelegate: NSObject, NSApplicationDelegate {
    // UI outlets and actions
    @IBOutlet weak var statusMenu: NSMenu!
    @IBOutlet weak var menuItemEnable: NSMenuItem!
    @IBOutlet weak var menuItemDisable: NSMenuItem!
    private let systemMenuIcon = NSStatusBar.system().statusItem(withLength:NSVariableStatusItemLength)
    
    @IBAction func quitApp(_: NSMenuItem) {
        LOG.info()
        NSApplication.shared().terminate(self)
    }
    
    @IBAction func enable(_: NSMenuItem) {
        ConfManager.enable()
        refreshMenu()
    }
    
    @IBAction func disable(_: NSMenuItem) {
        ConfManager.disable()
        refreshMenu()
    }
    
    func applicationDidFinishLaunching(_: Notification) {
        LOG.setup(enabled: CommandLine.arguments.contains("--debug"))
        LOG.info()
        
        ConfManager.initialize(app: self)
        
        // setup status bar UI
        systemMenuIcon.toolTip = "Translit"
        systemMenuIcon.menu = statusMenu
        
        refreshMenu()
        
        KbdHook.install()
    }

    func applicationWillTerminate(_: Notification) {
        LOG.info()
    }
    
    public func refreshMenu() {
        systemMenuIcon.image = ConfManager.enabled ?
            NSImage(named: "AppIconEnabled") : NSImage(named: "AppIconDisabled")
        menuItemEnable.isHidden = ConfManager.enabled
        menuItemDisable.isHidden = !ConfManager.enabled
    }
}
