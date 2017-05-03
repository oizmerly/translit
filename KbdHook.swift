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
        // CFRunLoopRun()
    }
    
    private static func handleTapEvent(event: CGEvent, proxy: CGEventTapProxy) {
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
        if !transliterator.translate(char: key) {
            LOG.info("swallow keyboard event")
            event.type = CGEventType.null
            post(text: "\(key)", proxy: proxy)
        }
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
