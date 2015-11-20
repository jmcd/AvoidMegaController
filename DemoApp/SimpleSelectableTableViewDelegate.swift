import UIKit


class SimpleSelectableTableViewDelegate: NSObject, UITableViewDelegate {
	
	var didSelectRowAtIndexPath: ((UITableView, NSIndexPath) -> ()) = { (_, _) in }
	
	func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
		didSelectRowAtIndexPath(tableView, indexPath)
	}
}