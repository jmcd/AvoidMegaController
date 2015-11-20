import UIKit


extension UIBarButtonItem {
	
	public convenience init(barButtonSystemItem systemItem: UIBarButtonSystemItem) {
		self.init(barButtonSystemItem: systemItem, target: nil, action: "")
	}
	
	public convenience init(title: String?) {
		self.init(title: title, style: .Plain, target: nil, action: "")
	}
}

enum NavigationButtonPosition {
	case Left
	case Right
	
	func items(navItem: UINavigationItem) -> [UIBarButtonItem]? {
		switch self {
		case .Left:
			return navItem.leftBarButtonItems
		case .Right:
			return navItem.rightBarButtonItems
		}
	}
	
	func setItems(navItem: UINavigationItem, items: [UIBarButtonItem]) {
		switch self {
		case .Left:
			navItem.leftBarButtonItems = items
		case .Right:
			navItem.rightBarButtonItems = items
		}
	}
}

extension UINavigationItem {
	
	func addButton(button: UIBarButtonItem, atPosition position: NavigationButtonPosition) {
		var newItems = [UIBarButtonItem]()
		if let existing = position.items(self) {
			newItems.appendContentsOf(existing)
		}
		newItems.append(button)
		position.setItems(self, items: newItems)
	}
}

struct NavigationButtonSpec {
	
	let buttonFactory: () -> (UIBarButtonItem)
	let position: NavigationButtonPosition
	let action: (NavigationContext) -> ()
	let validPredicate: (NavigationContext) -> Bool
	
	init(buttonFactory: () -> (UIBarButtonItem), position: NavigationButtonPosition, action: (NavigationContext) -> (), validPredicate:((NavigationContext) -> Bool) = { _ in true }) {
		self.buttonFactory = buttonFactory
		self.action = action
		self.position = position
		self.validPredicate = validPredicate
	}
	
	func addButtonForContext(ctx: NavigationContext) -> ActionWrapper? {
		if !validPredicate(ctx) {
			return nil
		}
		
		let aw = ActionWrapper(action: {
			self.action(ctx)
		})
		
		let b = buttonFactory()
		b.target = aw
		b.action = ActionWrapper.selector
		
		ctx.navigationItem.addButton(b, atPosition: position)
		
		return aw
	}
}

