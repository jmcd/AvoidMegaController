import UIKit

class CancelDoneNavigation: NSObject, NavigationSpec {
	
	private let onCancel: () -> ()
	private let onDone: () -> ()
	
	init(onCancel: () -> (), onDone: () -> ()) {
		self.onCancel = onCancel
		self.onDone = onDone
	}
	
	func applyToContext(ctx: NavigationContext) {
		ctx.navigationItem.leftBarButtonItem = UIBarButtonItem(barButtonSystemItem: .Cancel, target: self, action: "cancel")
		ctx.navigationItem.rightBarButtonItem = UIBarButtonItem(barButtonSystemItem: .Done, target: self, action: "done")
	}
	
	func done() {
		onDone()
	}
	
	func cancel() {
		onCancel()
	}
}
