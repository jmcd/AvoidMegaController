import Foundation

class ActionWrapper: NSObject {
	
	let action: () -> ()
	
	static let selector: Selector = "perform"
	
	init(action: () -> ()) {
		self.action = action
	}
	
	func perform() {
		action()
	}
}