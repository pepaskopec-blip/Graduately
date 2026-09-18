import SwiftUI

struct MapNode {
    let id: String
    let label: String
    let locked: Bool
    let done: Bool
    let current: Bool
    var finish: Bool = false
    var destination: Route?
}
