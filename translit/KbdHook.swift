import Foundation


/// Keyboard interceptor. KbdHook is a singleton, so everything is static
class KbdHook {
    private static let transliterator = Transliterator()
    private static var toggleCode: UInt64 = 0
    // maping special key -> flag, we use it to init toggle sequence
    private static let specialKeys: [String: UInt64] = [
        "LSHIFT"  : 0x20102,
        "RSHIFT"  : 0x20104,
        "LCTRL"   : 0x40101,
        "RCTRL"  : 0x42100,
        "LOPTION" : 0x80120,
        "ROPTION" : 0x80140,
        "LCMD"    : 0x100108,
        "RCMD"    : 0x100110,
        "FN"      : 0x800100
    ]

    /// Install keyboard hook
    public static func install() {
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
                     KbdHook.handleTapEvent(event: event, proxy: proxy)
                     return Unmanaged.passRetained(event)
            },
            userInfo: nil)
        let runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0)
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        CGEvent.tapEnable(tap: eventTap!, enable: true)
        // CFRunLoopRun() we are already on event loop, no need to run a new one, right? \todo
        
        let toggle = ConfManager.get("toggle") as! [String]
        LOG.info("toggle sequence: \(toggle)")
        toggleCode = toggle.map({specialKeys[$0]!}).reduce(0, { $0 | $1 })
        LOG.info("toggle code: \(toggleCode)")
    }
    
    private static func handleTapEvent(event: CGEvent, proxy: CGEventTapProxy) {
        if event.type == CGEventType.flagsChanged {
            LOG.info("flags: " + String(format: "%x", event.flags.rawValue))
            if event.flags.rawValue == toggleCode {
               ConfManager.toggle()
            } else {
                transliterator.drop()
            }
            return
        }
        if event.type != CGEventType.keyDown {
            // \todo currently we don't support key up, it seems this doesn't cause any visible problem. Need additional
            // investigation...
            return
        }
        if !ConfManager.enabled {
            LOG.info("disabled")
            return
        }
        for mask in [CGEventFlags.maskControl,
                    CGEventFlags.maskAlternate,
                    CGEventFlags.maskCommand,
                    CGEventFlags.maskHelp, 
                    CGEventFlags.maskSecondaryFn,
                    CGEventFlags.maskNumericPad] {
            if event.flags.contains(mask) {
                LOG.info("skip event")
                transliterator.drop()
                return
            }
        }
        // read keyboard raw data
        var count = 1
        var rawKey: UniChar = 0
        event.keyboardGetUnicodeString(maxStringLength: count, actualStringLength: &count, unicodeString: &rawKey)
        if count != 1 {
            LOG.info("wrong raw key length: \(count)")
            return
        }
        let key = Character(UnicodeScalar(rawKey)!)
        let result = transliterator.translate(key: key)
        if !result.continue {
            LOG.info("swallow keyboard event and post translation")
            event.type = CGEventType.null
            backspace(count: result.backspaces, proxy: proxy)
            post(text: result.translation!, proxy: proxy)
        }
    }
    
    // Imitate keypresses posting posting keyboard events
    private static func post(text: String, proxy: CGEventTapProxy) {
        var unichar: [UniChar] = [0]
        for char in text.utf16 {
            unichar[0] = char
            let event = CGEvent(keyboardEventSource: nil, virtualKey: 0, keyDown: true)!
            event.keyboardSetUnicodeString(stringLength: 1, unicodeString: unichar)
            event.tapPostEvent(proxy)
        }
    }
    
    private static func backspace(count: Int, proxy: CGEventTapProxy) {
        if count <= 0 {
            return // there is nothing to do
        }
        LOG.info("do backsapce \(count) times")
        let keyDown = CGEvent(keyboardEventSource: nil, virtualKey: 0x33, keyDown: true)!
        let keyUp = CGEvent(keyboardEventSource: nil, virtualKey: 0x33, keyDown: false)!
        for _ in 0..<count {
            keyDown.tapPostEvent(proxy)
            keyUp.tapPostEvent(proxy)
        }
    }
}
