import Cocoa
import Foundation


/// App main class
@NSApplicationMain class AppDelegate: NSObject, NSApplicationDelegate {
    // UI controls
    @IBOutlet weak var statusMenu: NSMenu!
    @IBOutlet weak var menuItemEnable: NSMenuItem!
    @IBOutlet weak var menuItemDisable: NSMenuItem!

    @IBAction func quitApp(_: NSMenuItem) {
        NSApplication.shared().terminate(self)
    }
    
    @IBAction func enable(_: NSMenuItem) {
        menuItemEnable.isHidden = true
        menuItemDisable.isHidden = false
    }
    
    @IBAction func disable(_: NSMenuItem) {
        menuItemEnable.isHidden = false
        menuItemDisable.isHidden = true
    }

    private let statusItem = NSStatusBar.system().statusItem(withLength:NSVariableStatusItemLength)
    static let transliterator = Transliterator()
    
    func applicationDidFinishLaunching(_: Notification) {
        LOG.info()
        
        // setup status bar UI
        statusItem.image = NSImage(named: "StatusIcon")
        statusItem.title = "Translit"
        statusItem.toolTip = "Translit"
        statusItem.menu = statusMenu
        
        KbdHook.install()
    }

    func applicationWillTerminate(_: Notification) {
        LOG.info()
    }
    
    
}
