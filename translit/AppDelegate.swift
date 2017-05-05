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
        LOG.info()
        menuItemEnable.isHidden = true
        menuItemDisable.isHidden = false
    }
    
    @IBAction func disable(_: NSMenuItem) {
        LOG.info()
        menuItemEnable.isHidden = false
        menuItemDisable.isHidden = true
    }
    
    func applicationDidFinishLaunching(_: Notification) {
        LOG.setup(enabled: CommandLine.arguments.contains("--debug"))
        LOG.info()
        ConfManager.load()
        
        // setup status bar UI
        systemMenuIcon.image = NSImage(named: "StatusIcon")
        systemMenuIcon.title = "Translit"
        systemMenuIcon.toolTip = "Translit"
        systemMenuIcon.menu = statusMenu
        
        KbdHook.install()
    }

    func applicationWillTerminate(_: Notification) {
        LOG.info()
    }
}
