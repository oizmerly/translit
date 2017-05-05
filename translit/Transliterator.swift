import Foundation


/// Translit converter
class Transliterator {
    private let encodings: [String:String]
    private var combinationsCharSet = Set<Character>()
    private var currentCombination = ""
    private var uppercase = false
    
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
        let char = String(key).lowercased()
        LOG.info("translate '\(key.description)'")
        if !combinationsCharSet.contains(Character(char)) {
            LOG.info("this key isn't a part of any combination")
            drop()
            return (continue: true, translation: nil, backspaces: 0)
        }
        currentCombination.append(char)
        var translation = encodings[currentCombination]
        if translation == nil && !currentCombination.isEmpty {
            drop()
            currentCombination = char
            translation = encodings[currentCombination]
        }
        LOG.info("\(currentCombination) -> \(translation)")
        
        // fix case
        if char != String(key) {
            LOG.info("uppercased")
            uppercase = true
        }
        if uppercase {
            LOG.info("upercase result")
            translation = translation?.uppercased()
        }
        
        return (continue: translation == nil,
                translation: translation,
                backspaces: currentCombination.characters.count > 1 ? 1 : 0)
    }
    
    /// Clean current state
    public func drop() {
        LOG.info()
        currentCombination = ""
        uppercase = false
    }
}
