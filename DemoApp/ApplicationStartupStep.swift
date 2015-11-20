import UIKit

protocol ApplicationStartupStep {
	func performWithApplication(application: UIApplication, container: Container)
}