import Foundation


/// Keyboard interceptor. KbdHook is a singleton, so everything is static
class KbdHook {
    private static let transliterator = Transliterator()
    
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
    }
    
    private static func handleTapEvent(event: CGEvent, proxy: CGEventTapProxy) {
        if event.type == CGEventType.flagsChanged {
            LOG.info("flags")
            return
        }
        if event.type != CGEventType.keyDown {
            // \todo currently we don't support key up, it seems this doesn't cause any visible problem. Need additional
            // investigation...
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
