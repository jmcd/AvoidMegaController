import Foundation
import RealmSwift

struct MainNavigationInstaller: ContainerInstaller {
	
	static let identifier = NSUUID().UUIDString
	
	func install(container: Container) {
		
		container.register(NavigationSpec.self, identifier: MainNavigationInstaller.identifier) { c in
			
			let fn = { (sourceVc: UIViewController) in
				let addScene = c.resolve(AddScene)
				addScene.todoController.completion = { sourceVc.dismissViewControllerAnimated(true, completion: nil) }
				addScene.todoController.writeAction = { model in
					let realm = c.resolve(Realm)
					try! realm.write {
						realm.add(TodoItem.ItemWithId(nil, text: model.text, done: model.done))
					}
				}
				sourceVc.showViewController(addScene.navigationController, sender: sourceVc)
			}
			
			
			return MainNavigation(showAdd: fn)
		}
	}
}