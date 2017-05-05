import Foundation


/// Translit converter
class Transliterator {
    private let encodings: [String:String]
    private var combinationsCharSet = Set<Character>()
    private var currentCombination = ""
    
    public init() {
        LOG.info()
        encodings = ConfManager.get("encodings") as! [String:String]
        LOG.info("conf: \(encodings)")
        // init
        for encoding in encodings {
            for char in encoding.key.characters {
                combinationsCharSet.insert(char)
            }
        }
    }
    
    public func translate(key: Character) -> (continue: Bool, translation: String?, backspaces: Int) {
        LOG.info("translate '\(key.description)'")
        if !combinationsCharSet.contains(key) {
            LOG.info("this key isn't a part of any combination")
            drop()
            return (continue: true, translation: nil, backspaces: 0)
        }
        currentCombination.append(key)
        var translation = encodings[currentCombination]
        if translation == nil && !currentCombination.isEmpty {
            currentCombination = String(key)
            translation = encodings[currentCombination]
        }
        LOG.info("\(currentCombination) -> \(translation)")
        return (continue: translation == nil,
                translation: translation,
                backspaces: currentCombination.characters.count > 1 ? 1 : 0)
    }
    
    /// Clean current state
    public func drop() {
        LOG.info()
        currentCombination = ""
    }
}
