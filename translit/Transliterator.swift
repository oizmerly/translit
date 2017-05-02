import Foundation


/// Translit converter
class Transliterator {
    init() {
//        let asset = NSDataAsset(name: "conf", bundle: Bundle.main)
//        let json = try? JSONSerialization.jsonObject(with: asset!.data, options: JSONSerialization.ReadingOptions.allowFragments)
//        print(json)
        let path = Bundle.main.path(forResource: "conf", ofType: "json")
        let data = try? Data(contentsOf: URL(fileURLWithPath: path!))
        let jsonData = try? JSONSerialization.jsonObject(with: data!, options: []) as! [String:Any]
        let encoding = jsonData!["encoding"]
    }
}
