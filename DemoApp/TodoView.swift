import UIKit

struct TodoViewModel {
	var id: String?
	var text = ""
	var done = false
}

extension TodoViewModel {
	init(copyingValuesFrom todo: TodoItem) {
		id = todo.id
		text = todo.text
		done = todo.done
	}
}

class TodoView: MinimalView {
	
	var dataSource = TodoViewModel() {
		didSet {
			reload()
		}
	}
	
	private	let textField: UITextField = {
		let tf = UITextField()
		tf.placeholder = "Task"
		return tf
	}()
	
	private let doneLabel: UILabel = {
		let l = UILabel()
		l.text = "Done?"
		return l
	}()
	
	private let doneSwitch = UISwitch()
	
	override func boot() {
		
		reload()
		
		doneSwitch.addTarget(self, action: "doneSwitchValueChanged", forControlEvents: .ValueChanged)
		textField.addTarget(self, action: "textFieldEditingChanged", forControlEvents: .EditingChanged)
		
		addSubviews([textField, doneLabel, doneSwitch])
		
		NSLayoutConstraintBuilder()
			.usingViews([
				"tf": textField,
				"dl": doneLabel,
				"ds": doneSwitch,
				])
			.createConstraintsWithVisualFormats([
				"V:|[tf]-[ds]",
				"H:|[tf]|",
				"H:[dl]-[ds]|",
				])
			.addConstraintsToView(self)
		
		addConstraint(doneLabel.constraintWithAttribute(.CenterY, toItem: doneSwitch))
	}
	
	func doneSwitchValueChanged() {
		dataSource.done = doneSwitch.on
	}
	
	func textFieldEditingChanged() {
		dataSource.text = textField.text ?? ""
	}
	
	private func reload() {
		textField.text = dataSource.text
		doneSwitch.on = dataSource.done
	}
	
	override func canBecomeFirstResponder() -> Bool {
		return textField.canBecomeFirstResponder()
	}
	
	override func becomeFirstResponder() -> Bool {
		return textField.becomeFirstResponder()
	}
	
	override func resignFirstResponder() -> Bool {
		return textField.resignFirstResponder()
	}
}
