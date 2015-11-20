import Foundation
import RealmSwift

class TodoTableViewRealmDataSource: NSObject, TodoTableViewDataSource {
	
	let listOperation: (() -> Results<TodoItem>)
	let cellReuseId: String
	
	private var prevResults: Results<TodoItem>?
	
	private lazy var items: Results<TodoItem> = {
		return self.listOperation()
	}()
	
	init(listOperation: (() -> Results<TodoItem>), cellReuseId: String) {
		self.listOperation = listOperation
		self.cellReuseId = cellReuseId
	}
	
	func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return items.count
	}
	
	func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
		let todo = itemAtIndexPath(indexPath)
		let cell = tableView.dequeueReusableCellWithIdentifier(cellReuseId)! // OK to blow up? I think so.
		cell.textLabel?.text = todo.text
		cell.accessoryType = todo.done ? .Checkmark : .None
		return cell
	}
	
	var onChange: (() -> ()) = { }
	
	func itemAtIndexPath(indexPath: NSIndexPath) -> TodoItem {
		return items[indexPath.row]
	}
}