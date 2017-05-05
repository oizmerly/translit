import Foundation


/// Configuration manager
class ConfManager {
    private static let confFileName = "conf"
    private static var conf: [String:Any]?
    private static var appDelegate: AppDelegate?
    
    /// Load app settings
    public static func initialize(app: AppDelegate) {
        appDelegate = app
        LOG.info("load configuration")
        let path = Bundle.main.path(forResource: confFileName, ofType: "json")
        let confData = try? Data(contentsOf: URL(fileURLWithPath: path!))
        conf = try? JSONSerialization.jsonObject(with: confData!, options: []) as! [String:Any]
        if conf == nil {
            LOG.fatal("cannot read conf")
        }
        LOG.info("conf: \(conf!)")
    }
    
    public static func get(_ key: String) -> Any? {
        return  conf![key]
    }
 
    //------------------------------------------------------------------------------------------------------------------
    // dynamic conf
    
    private static var enabledStatus = false
    public static var enabled: Bool { get { return enabledStatus } }
    
    /// Enable transliteration
    public static func enable() {
        LOG.info()
        enabledStatus = true
        appDelegate?.refreshMenu()
    }

    /// disable transliteration
    public static func disable() {
        LOG.info()
        enabledStatus = false
        appDelegate?.refreshMenu()
    }
    
    /// Enable enabled status
    public static func toggle() {
        LOG.info("\(enabled) -> \(!enabled)")
        if enabled {
            disable()
        } else {
            enable()
        }
    }
}
