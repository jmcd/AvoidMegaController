import UIKit


struct WindowInstaller: ContainerInstaller {
	
	func install(container: Container) {
		
		container.register(UIWindow.self, lifestyle: .Singleton) { c in
			
			let scene = c.resolve(ListScene)
			
			let w = UIWindow(frame: UIScreen.mainScreen().bounds)
			w.rootViewController = scene.navigationController
			return w
		}
	}
}