import UIKit


protocol TodoTableViewDataSource: UITableViewDataSource {
	
	var onChange: (() -> ()) { get set }
	func itemAtIndexPath(indexPath: NSIndexPath) -> TodoItem
}

class TodoListViewController: UIViewController {
	
	static let cellReuseId = NSUUID().UUIDString
	
	var dataSource: TodoTableViewDataSource?
	var showDetail: ((TodoListViewController, TodoItem) -> ())?
	
	private lazy var tableViewDelegate: UITableViewDelegate = {
		let d = SimpleSelectableTableViewDelegate()
		d.didSelectRowAtIndexPath = { _, indexPath in
			if let selectedItem = self.dataSource?.itemAtIndexPath(indexPath) {
			self.showDetail?(self, selectedItem)
			}
		}
		return d
	}()
	
	private lazy var tableView: UITableView = {
		let tv = UITableView()
		tv.registerClass(UITableViewCell.self, forCellReuseIdentifier: TodoListViewController.cellReuseId)
		tv.dataSource = self.dataSource
		tv.delegate = self.tableViewDelegate
		self.dataSource?.onChange = { tv.reloadData() }
		return tv
	}()
	
	
	override func viewWillAppear(animated: Bool) {
		super.viewWillAppear(animated)
		exposeNavigation()
	}
	
	override func viewDidLoad() {
		
		automaticallyAdjustsScrollViewInsets = false
		
		view.addSubviews([tableView])
		
		view.addConstraints(tableView.constraintsMaximizingInView(view, topLayoutGuide: topLayoutGuide, bottomLayoutGuide: bottomLayoutGuide))		
	}
}

