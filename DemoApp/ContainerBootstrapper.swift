import UIKit

struct ContainerInitializer: ApplicationStartupStep {
	
	private static let installers: [ContainerInstaller] = [
		RealmInstaller(),
		WindowInstaller(),
		MainNavigationInstaller(),
		ListViewControllerInstaller(),
		ListSceneInstaller(),
		TodoViewControllerInstaller(),
		AddSceneInstaller(),
	]
	
	func performWithApplication(application: UIApplication, container: Container) {
		
		for installer in ContainerInitializer.installers {
			installer.install(container)
		}
	}
}