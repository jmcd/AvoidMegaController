import UIKit
import RealmSwift


struct ListSceneInstaller: ContainerInstaller {
	
	func install(container: Container) {
		
		// Scene is a list view wrapped in navigation
		container.register(ListScene.self) { c in
			
			let listVc = c.resolve(TodoListViewController)
			listVc.title = "Todo List"
			
			let nav = c.resolve(NavigationSpec.self, identifier: MainNavigationInstaller.identifier)
			
			let nc = UINavigationController.navigationControllerWithRootViewController(listVc, spec: nav)
			
			return ListScene(navigationController: nc, listController: listVc)
		}
	}
}

