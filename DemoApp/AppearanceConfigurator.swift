import UIKit

struct AppearanceConfigurator: ApplicationStartupStep {
	
	func performWithApplication(application: UIApplication, container: Container) {
		
		UIWindow.appearance().backgroundColor = UIColor.whiteColor()
		
		application.setStatusBarStyle(.LightContent, animated: false)
		
		let navFgColor = UIColor.whiteColor()
		let navBar = UINavigationBar.appearance()
		navBar.barTintColor = UIColor.darkGrayColor()
		navBar.tintColor = navFgColor
		navBar.titleTextAttributes = [
			NSForegroundColorAttributeName : navFgColor
		]
	}
}
