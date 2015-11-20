import UIKit

extension UIView {
	
	func constraintsMaximizingInView(view: UIView, topLayoutGuide: UILayoutSupport? = nil, bottomLayoutGuide: UILayoutSupport? = nil, insets: UIEdgeInsets = UIEdgeInsetsZero) -> [NSLayoutConstraint] {
		
		return [
			self.constraintWithAttribute(.Top, toItem: topLayoutGuide ?? view, itemAttribute: topLayoutGuide == nil ? .Top : .Bottom, constant: insets.top),
			self.constraintWithAttribute(.Bottom, toItem: bottomLayoutGuide ?? view, itemAttribute: bottomLayoutGuide == nil ? .Bottom : .Top, constant: insets.bottom),
			self.constraintWithAttribute(.Left, toItem: view, constant: insets.left),
			self.constraintWithAttribute(.Right, toItem: view, constant: insets.right),
		]
	}
	
	func constraintWithAttribute(attribute: NSLayoutAttribute, relatedBy: NSLayoutRelation = .Equal, toItem: AnyObject?, multiplier: CGFloat = 1.0, constant: CGFloat = 0.0) -> NSLayoutConstraint {
		return NSLayoutConstraint(item: self, attribute: attribute, relatedBy: relatedBy, toItem: toItem, attribute: attribute, multiplier: multiplier, constant: constant)
	}
	
	func constraintWithAttribute(attribute: NSLayoutAttribute, relatedBy: NSLayoutRelation = .Equal, toItem: AnyObject?, itemAttribute: NSLayoutAttribute, multiplier: CGFloat = 1.0, constant: CGFloat = 0.0) -> NSLayoutConstraint {
		return NSLayoutConstraint(item: self, attribute: attribute, relatedBy: relatedBy, toItem: toItem, attribute: itemAttribute, multiplier: multiplier, constant: constant)
	}
	
	func constraintWithAttribute(attribute: NSLayoutAttribute, relatedBy: NSLayoutRelation, constant: CGFloat) -> NSLayoutConstraint {
		return NSLayoutConstraint(item: self, attribute: attribute, relatedBy: relatedBy, toItem: nil, attribute: .NotAnAttribute, multiplier: 1.0, constant: constant)
	}
}

extension UIView {
	
	func addSubviews(subviews: [UIView], translatesAutoresizingMaskIntoConstraints: Bool = false) {
		for sv in subviews {
			addSubview(sv)
			sv.translatesAutoresizingMaskIntoConstraints = translatesAutoresizingMaskIntoConstraints
		}
	}
}

class NSLayoutConstraintBuilder {
	
	var views = Dictionary<String, AnyObject>()
	var constraints = Array<NSLayoutConstraint>()
	
	func usingViews(nameToViewOrLayoutGuide:[String:AnyObject]) -> NSLayoutConstraintBuilder {
		
		for kvp in nameToViewOrLayoutGuide {
			let obj = kvp.1
			if let v = obj as? UIView {
				v.translatesAutoresizingMaskIntoConstraints = false
			}
			self.views[kvp.0] = obj
		}
		
		return self;
	}
	
	func createConstraintsWithVisualFormat(format:String) -> NSLayoutConstraintBuilder {
		let cs = NSLayoutConstraint.constraintsWithVisualFormat(format, options: NSLayoutFormatOptions(), metrics: nil, views: self.views)
		self.constraints.appendContentsOf(cs)
		return self
	}
	
	func createConstraintsWithVisualFormats(formats:Array<String>) -> NSLayoutConstraintBuilder {
		for f in formats {
			self.createConstraintsWithVisualFormat(f)
		}
		return self
	}
	
	func addConstraintsToView(view: UIView) -> NSLayoutConstraintBuilder {
		view.addConstraints(self.constraints)
		self.constraints = Array<NSLayoutConstraint>()
		return self
	}
}

extension UIEdgeInsets {
	
	static func uniform(value: CGFloat) -> UIEdgeInsets {
		return UIEdgeInsets(top: value, left: value, bottom: -value, right: -value)
	}
}

class MinimalView: UIView {
	
	convenience init() {
		self.init(frame: CGRectZero)
	}
	
	override init(frame: CGRect) {
		super.init(frame: frame)
		boot()
	}
	
	required init?(coder aDecoder: NSCoder) {
		fatalError("init(coder:) has not been implemented")
	}
	
	func boot() {
		
	}
}
