import XCTest
@testable import DemoApp


class DataLoader {
	
	func loadData() -> [String] {
		return ["Hello", "World!"]
	}
}

class DataController {
	
	var data: [String]!
}

class SomeNSObject: NSObject {
	
}

class Context {
	var data: String?
}

class ContainerTests: XCTestCase {
	
	func testWithoutDeps() {
		
		let container = Container()
		
		container.register(DataLoader.self, providedBy: { _ in DataLoader() })
		
		let dataLoader: DataLoader = container.resolve(DataLoader.self)
		
		let data = dataLoader.loadData()
		
		XCTAssertEqual("Hello",  data[0])
	}
	
	func testWithDeps() {
		
		let container = Container()
		
		container.register(DataLoader.self, providedBy: { _ in DataLoader() })
		container.register(DataController.self, providedBy: {
			let dc = DataController()
			dc.data = $0.resolve(DataLoader.self).loadData()
			return dc
		})
		
		let dataController = container.resolve(DataController.self)
		
		XCTAssertEqual("Hello",  dataController.data[0])
	}
	
	func testWithNSObjectEmptyInit() {
		let container = Container()
		container.register(SomeNSObject.self)
		let _ = container.resolve(SomeNSObject.self)
	}
	
	func testWithSingleton() {
		let container = Container()
		container.register(ContainerRegistration(type: Context.self, lifestyle: .Singleton, provider: { _ in Context() }))
		
		let c0 = container.resolve(Context.self)
		c0.data = "Hi!"
		
		let c1 = container.resolve(Context.self)
		
		XCTAssertTrue(c0 === c1)
		XCTAssertEqual("Hi!", c0.data)
		XCTAssertEqual("Hi!", c1.data)
	}
}
