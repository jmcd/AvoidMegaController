import Foundation

import RealmSwift

class TodoItem: Object {
	
	dynamic var id = ""
	
	dynamic var text = ""
	dynamic var inceptionDate = NSDate(timeIntervalSince1970: 0)
	dynamic var done = false
	
	override static func primaryKey() -> String? {
		return "id"
	}
}

extension TodoItem {
	
	static func ItemWithId(idOrNil: String?, text: String, done: Bool) -> TodoItem {
		let item = TodoItem()
		item.id = idOrNil ?? NSUUID().UUIDString
		item.text = text
		item.inceptionDate = NSDate()
		item.done = done
		return item
	}
}