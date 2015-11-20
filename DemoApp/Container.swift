import Foundation


public class Container {
	
	static let defaultContainer = Container()
	
	private var registry = [ContainerKey: Any]()
	private var previouslyProvidedSingletons = [ContainerKey: Any]()
	
	func register<T>(registration: ContainerRegistration<T>) {
		let key = keyForType(registration.type, identifier: registration.identifier)
		if registry[key] == nil {
			registry[key] = registration
		} else {
			fatalError("duplicate registration with key \(key)")
		}
	}
	
	func resolve<T>(type: T.Type, identifier: String = "") -> T {
		let key = keyForType(type, identifier: identifier)
		
		if let registration = registry[key] as? ContainerRegistration<T> {
			
			if registration.lifestyle == .Singleton {
				if let prev = previouslyProvidedSingletons[key] as? T {
					return prev
				}
				let newSingleton = registration.provider(container: self)
				previouslyProvidedSingletons[key] = newSingleton
				return newSingleton
			}
			
			return registration.provider(container: self)
		} else {
			fatalError("no registered component for key '\(key)'")
		}
	}
	
	func keyForType<T>(type: T.Type, identifier: String) -> ContainerKey {
		return ContainerKey(typeName: "\(type)", identifier: identifier)
	}
}

extension Container {
	
	// convieniece funcs
	
	func register<T>(type: T.Type, identifier: String = "", lifestyle: ContainerLifestyle = .Transient, providedBy provider: ((c: Container) -> T)) -> Container{
		let reg = ContainerRegistration(type: type, identifier: identifier, lifestyle: lifestyle, provider: provider)
		register(reg)
		return self
	}
	
	func register<T: NSObject>(type: T.Type) -> Container {
		register(type) { _ in T() }
		return self
	}
}

class ContainerRegistration<T> {
	
	let type: T.Type
	let identifier: String
	let lifestyle: ContainerLifestyle
	let provider: ((container: Container) -> T)
	
	init(type: T.Type, identifier: String = "", lifestyle: ContainerLifestyle, provider: ((c: Container) -> T)) {
		self.type = type
		self.identifier = identifier
		self.lifestyle = lifestyle
		self.provider = provider
	}
}

struct ContainerKey: Hashable, CustomStringConvertible {
	let typeName: String
	let identifier: String
	
	private func stringVersion() -> String {
		return "\(typeName)|\(identifier)"
	}
	
	var hashValue: Int { get {
		let h0 = typeName.hashValue
		let h1 = identifier.hashValue
		let h0x = h0&397
		
		let hv = h0x &+ h1;
		return  hv
		}
	}
	
	var description: String {
		get {
			return stringVersion()
		}
	}
}

func ==(lhs: ContainerKey, rhs: ContainerKey) -> Bool {
	return lhs.typeName == rhs.typeName && lhs.identifier == rhs.identifier
}

enum ContainerLifestyle {
	case Transient
	case Singleton
}

protocol ContainerInstaller {
	func install(container: Container)
}