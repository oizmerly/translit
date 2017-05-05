import Foundation


/// Configuration manager
class ConfManager {
    private static let confFileName = "conf"
    private static var conf: [String:Any]?
    
    /// Load app settings
    public static func load() {
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
}
