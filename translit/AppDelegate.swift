import Cocoa
import Foundation


@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    let transliterator = Transliterator()
    
    let statusItem = NSStatusBar.system().statusItem(withLength:NSVariableStatusItemLength)

    @IBOutlet weak var statusMenu: NSMenu!
    @IBOutlet weak var menuItemEnable: NSMenuItem!
    @IBOutlet weak var menuItemDisable: NSMenuItem!
    
    func applicationDidFinishLaunching(_: Notification) {
        LOG.info()
        
        // setup status bar UI
        statusItem.image = NSImage(named: "StatusIcon")
        statusItem.title = "Translit"
        statusItem.toolTip = "Translit"
        statusItem.menu = statusMenu
        
        initHook()
    }

    func applicationWillTerminate(_: Notification) {
        LOG.info()
    }
    
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
    
    func initHook() {
        let eventTap = CGEvent.tapCreate(
            tap: CGEventTapLocation.cgSessionEventTap,
            place: CGEventTapPlacement.headInsertEventTap,
            options: CGEventTapOptions.defaultTap,
            eventsOfInterest: CGEventMask(
                (1 << CGEventType.keyDown.rawValue) |
                (1 << CGEventType.keyUp.rawValue) |
                (1 << CGEventType.flagsChanged.rawValue)
            ),
            callback: {
                (proxy: CGEventTapProxy,
                 type: CGEventType,
                 event: CGEvent,
                 refcon: UnsafeMutableRawPointer?) -> Unmanaged<CGEvent>? in
                    AppDelegate.handleTap(event: event, proxy: proxy)
                    return Unmanaged.passRetained(event)
            },
            userInfo: nil)
        let runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0)
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        CGEvent.tapEnable(tap: eventTap!, enable: true)
        // CFRunLoopRun()
    }
    
    static func handleTap(event: CGEvent, proxy: CGEventTapProxy) {
        if event.type != CGEventType.keyDown {
            return
        }
        // read keyboard raw data
        var count = 1
        var rawKey: UniChar = 0
        event.keyboardGetUnicodeString(maxStringLength: count, actualStringLength: &count, unicodeString: &rawKey)
        if count != 1 {
            LOG.info("wrong raw input length: \(count)")
            return
        }
        let key = Character(UnicodeScalar(rawKey)!)
        LOG.info("input char \(key)")
        //post(text: "превед", proxy: proxy)
        //event.type = CGEventType.null
    }
    
    static func post(text: String, proxy: CGEventTapProxy) {
        var unichar: [UniChar] = [0]
        for char in text.utf16 {
            unichar[0] = char
            let event = CGEvent(keyboardEventSource: nil, virtualKey: 0, keyDown: true)!
            event.keyboardSetUnicodeString(stringLength: 1, unicodeString: unichar)
            event.tapPostEvent(proxy)
        }
    }
}
