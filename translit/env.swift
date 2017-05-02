/// Simple logging utility
class LOG {
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
    }

    static func out(severity: String, message: String, function: String, line: Int) {
        print("\(severity): \(function)@\(line) \(message)")
    }
}
