import Foundation


/// Translit converter
class Transliterator {
    private let encodings: [String:String]
    private var combinationsCharSet = Set<Character>()
    
    init() {
        LOG.info("read configuration")
        let path = Bundle.main.path(forResource: "conf", ofType: "json")
        let confData = try? Data(contentsOf: URL(fileURLWithPath: path!))
        let conf = try? JSONSerialization.jsonObject(with: confData!, options: []) as! [String:Any]
        encodings = conf!["encoding"] as! [String:String]
        LOG.info("conf: \(encodings)")
        // init
        for encoding in encodings {
            for char in encoding.key.characters {
                combinationsCharSet.insert(char)
            }
        }
    }
    
    func translate(key: Character) -> (continue: Bool, translation: String?) {
        LOG.info("translate '\(key.description)'")
        if !combinationsCharSet.contains(key) {
            LOG.info("this key isn't a part of any combination")
            return (continue: true, translation: nil)
        }
        return (continue: false, translation: encodings[String(key)])
    }
}
