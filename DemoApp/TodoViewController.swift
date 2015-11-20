import UIKit

class TodoViewController: UIViewController {
	
	// deps
	var initalSourceOrNil: TodoItem?
	var writeAction: ((TodoViewModel) -> ())?
	var completion: (() -> ())?
	
	private let todoView = TodoView()
	private var cancelChanges = false

	
	override func viewWillAppear(animated: Bool) {
		super.viewWillAppear(animated)
		exposeNavigation()
	}
	
	override func viewDidLoad() {
		
		assert(writeAction != nil)
		
		view.backgroundColor = UIColor.whiteColor()
		
		view.addSubviews([todoView])
		
		view.addConstraints(
			todoView.constraintsMaximizingInView(view, topLayoutGuide: topLayoutGuide, insets: UIEdgeInsets.uniform(20))
		)
		
		if let s = initalSourceOrNil {
			todoView.dataSource = TodoViewModel(copyingValuesFrom: s)
		}
	}
	
	override func viewDidAppear(animated: Bool) {
		super.viewDidAppear(true)
		todoView.becomeFirstResponder()
	}
	
	override func viewWillDisappear(animated: Bool) {
		
		if !cancelChanges {
			writeAction?(todoView.dataSource)
		}
		
		todoView.resignFirstResponder()
		super.viewWillDisappear(true)
	}
	
	func commit() {
		// todo: we could add validation here
		completion?()
	}
	
	func cancel() {
		cancelChanges = true
		completion?()
	}
}