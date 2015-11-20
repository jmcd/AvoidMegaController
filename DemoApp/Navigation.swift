import UIKit

struct NavigationContext {
	let navigationItem: UINavigationItem
	let viewController: UIViewController
}

protocol NavigationSpec {
	func applyToContext(ctx: NavigationContext)
}

extension UINavigationController {
	
	static func navigationControllerWithRootViewController(rootViewController: UIViewController, spec: NavigationSpec) -> UINavigationController {
		
		let nc = HoooBoyNavigationController()
		nc.viewControllers = [rootViewController]
		nc.spec = spec
		return nc
	}
	
	private class HoooBoyNavigationController: UINavigationController, NavigationSpec {
		
		var spec: NavigationSpec?
		
		func applyToContext(ctx: NavigationContext) {
			spec?.applyToContext(ctx)
		}
	}
}

extension UIViewController {
	
	func exposeNavigation() {
		
		if let nv = self.navigationController as? NavigationSpec {
			let ctx = NavigationContext(navigationItem: self.navigationItem, viewController: self)
			nv.applyToContext(ctx)
		}
	}
}
