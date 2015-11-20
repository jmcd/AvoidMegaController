import UIKit


struct AddSceneInstaller: ContainerInstaller {
	
	func install(container: Container) {
		
		container.register(AddScene.self) { c in
			let todoVc = c.resolve(TodoViewController)
			let nav = CancelDoneNavigation(onCancel: todoVc.cancel, onDone: todoVc.commit)
			let nc = UINavigationController.navigationControllerWithRootViewController(todoVc, spec: nav)
			nc.modalPresentationStyle = .FormSheet
			return AddScene(navigationController: nc, todoController: todoVc)
		}
	}
}