import RealmSwift

struct RealmInstaller: ContainerInstaller {
	
	func install(container: Container) {
		
		container.register(Realm.self) { _ in
			try! Realm()
		}
	}
}