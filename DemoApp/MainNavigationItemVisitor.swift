import UIKit

class MainNavigation: NSObject, NavigationSpec {
	
	private let addSpec: NavigationButtonSpec
	private var addAction: ActionWrapper?
	
	private let aboutSpec: NavigationButtonSpec
	private var aboutAction: ActionWrapper?
	
	
	init(showAdd: (UIViewController) -> ()) {
		
		let alert = UIAlertController(title: "Ideas about app org", message: "A bunch of ideas about how an app might be organized, collected into one demo.", preferredStyle: .Alert)
		alert.addAction(UIAlertAction(title: "Oh.", style: .Default, handler: nil))
		
		addSpec = NavigationButtonSpec(
			buttonFactory: { UIBarButtonItem(barButtonSystemItem: .Add) },
			position: .Right,
			action: { showAdd($0.viewController) },
			validPredicate: { $0.viewController is TodoListViewController }
		)
		
		aboutSpec = NavigationButtonSpec(
			buttonFactory: { UIBarButtonItem(title: "About") },
			position: .Right,
			action: {
				$0.viewController.presentViewController(alert, animated: true, completion: nil)
			}
		)
	}
	
	func applyToContext(ctx: NavigationContext) {
		
		ctx.navigationItem.leftBarButtonItems = nil
		ctx.navigationItem.rightBarButtonItems = nil
		
		addAction = addSpec.addButtonForContext(ctx)
		aboutAction = aboutSpec.addButtonForContext(ctx)
	}
}