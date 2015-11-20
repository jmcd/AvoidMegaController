import UIKit
import RealmSwift

struct DummyDataInitializer: ApplicationStartupStep {
	
	private let itemTexts = [
		"Write some tests",
		"Do a blog post",
	]
	
	func performWithApplication(application: UIApplication, container: Container) {
	
		let realm = container.resolve(Realm)
		
		let count = realm.objects(TodoItem).count
		
		if count > 0 {
			return
		}
		
		let items = itemTexts.enumerate().map { TodoItem.ItemWithId(nil, text: $1, done: $0%2==1) }
		
		try! realm.write {
			for item in items {
				realm.add(item)
			}
		}
	}
}
