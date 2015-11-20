import UIKit

struct TodoViewControllerInstaller: ContainerInstaller {
	
	func install(container: Container) {
		container.register(TodoViewController)
	}
}
