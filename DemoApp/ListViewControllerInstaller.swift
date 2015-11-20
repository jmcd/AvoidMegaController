import UIKit
import RealmSwift


struct ListViewControllerInstaller: ContainerInstaller {
	
	func install(container: Container) {
		
		
		var notificationToken: RealmSwift.NotificationToken?
		
		// Data source
		container.register(TodoTableViewDataSource.self) { c in
			
			let realm = c.resolve(Realm.self)
			
			let listOp = {
				realm.objects(TodoItem)
			}
			
			let dataSource = TodoTableViewRealmDataSource(listOperation: listOp, cellReuseId: TodoListViewController.cellReuseId)
			
			notificationToken = realm.addNotificationBlock { notification, realm in
				dataSource.onChange()
			}
			
			return dataSource
		}
		
		// the view controller itself
		container.register(TodoListViewController.self) { c in
			let vc = TodoListViewController()
			vc.dataSource = c.resolve(TodoTableViewDataSource.self)
			
			vc.showDetail = { (listVc: TodoListViewController, selectedItem: TodoItem) in
				let detailVc = c.resolve(TodoViewController)
				detailVc.initalSourceOrNil = selectedItem
				detailVc.writeAction = { model in
					let realm = c.resolve(Realm)
					try! realm.write {
						realm.add(TodoItem.ItemWithId(model.id, text: model.text, done: model.done), update: true)
					}
				}
				listVc.showViewController(detailVc, sender: listVc)
			}
			
			return vc
		}
	}
}