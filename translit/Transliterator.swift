import Foundation


/// Translit converter
class Transliterator {
    let encodings: [String:String]
    
    init() {
        LOG.info("read configuration")
        let path = Bundle.main.path(forResource: "conf", ofType: "json")
        let confData = try? Data(contentsOf: URL(fileURLWithPath: path!))
        let conf = try? JSONSerialization.jsonObject(with: confData!, options: []) as! [String:Any]
        encodings = conf!["encoding"] as! [String:String]
        LOG.info("conf: \(encodings)")
    }
    
    func translate(char: Character) -> Bool {
        LOG.info("translate '\(char)'")
        return false
    }
}
