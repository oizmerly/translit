import Cocoa


/// Simple logging utility
class LOG {
    private static var enabled = true
    
    static func setup(enabled status: Bool) {
        enabled = status
    }
    
    static func info(_ message: String = "", function: String = #function, line: Int = #line) {
        out(severity: "INFO", message: message, function: function, line: line)
    }

    static func warn(_ message: String = "", function: String = #function, line: Int = #line) {
        out(severity: "WARN", message: message, function: function, line: line)
    }
    
    static func error(_ message: String = "", function: String = #function, line: Int = #line) {
        out(severity: "ERROR", message: message, function: function, line: line)
    }

    static func fatal(_ message: String = "", function: String = #function, line: Int = #line) {
        out(severity: "FATAL", message: message, function: function, line: line)
        NSApplication.shared().terminate(nil)
    }

    static func out(severity: String, message: String, function: String, line: Int) {
        if enabled {
            print("\(severity): \(function)@\(line) \(message)")
        }
    }
}
