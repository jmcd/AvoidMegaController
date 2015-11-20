import UIKit
import RealmSwift

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
	
	var window: UIWindow?
	
	private static let startupSteps: [ApplicationStartupStep] = [
		ContainerInitializer(),
		AppearanceConfigurator(),
		DummyDataInitializer(),
	]
	
	func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
		
		let container = Container.defaultContainer
		for startupStep in AppDelegate.startupSteps {
			startupStep.performWithApplication(application, container: container)
		}
		
		window = container.resolve(UIWindow)
		window?.makeKeyAndVisible()
		
		return true
	}
}