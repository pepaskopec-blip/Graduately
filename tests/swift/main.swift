// Runs tests/answers.json against ios/Graduately/Data/Answers.swift, the same
// cases the Kotlin apps check (android/desktop/src/test). Usage:
//   swift-answers <tests/answers.json> <content.json>
import Foundation

let args = CommandLine.arguments
guard args.count == 3,
      let casesData = FileManager.default.contents(atPath: args[1]),
      let cases = try? JSONSerialization.jsonObject(with: casesData) as? [String: Any],
      let contentData = FileManager.default.contents(atPath: args[2]),
      let content = try? JSONSerialization.jsonObject(with: contentData) as? [String: Any]
else {
    print("usage: swift-answers tests/answers.json content.json")
    exit(2)
}

var failures = 0
var checked = 0

func rows(_ name: String) -> [[Any]] { cases[name] as? [[Any]] ?? [] }
func str(_ v: Any) -> String? { v as? String }
func points(_ v: Any) -> [(Int, Int)] { (v as? [[Int]] ?? []).map { ($0[0], $0[1]) } }

func expect<T: Equatable>(_ got: T, _ want: T, _ what: String) {
    checked += 1
    if got != want {
        failures += 1
        print("FAIL \(what): got \(got), want \(want)")
    }
}

for r in rows("normalize") { expect(normalizeAnswer(str(r[0])), str(r[1]) ?? "", "normalize \(r[0])") }
for r in rows("accepts") { expect(answerAccepts(normalizeAnswer(str(r[0])), str(r[1])), r[2] as! Bool, "accepts \(r)") }
for r in rows("incomplete") { expect(answerIncomplete(normalizeAnswer(str(r[0])), str(r[1])), r[2] as! Bool, "incomplete \(r)") }
for r in rows("netTxtEq") { expect(netTxtEq(str(r[0]), str(r[1])), r[2] as! Bool, "netTxtEq \(r)") }
for r in rows("hasUmlaut") { expect(hasUmlaut(str(r[0])), r[1] as! Bool, "hasUmlaut \(r)") }
for r in rows("math") { expect(mathAnswerOk(str(r[0]), str(r[1])), r[2] as! Bool, "math \(r)") }
for r in rows("drawNeed") { expect(mathDrawNeed(str(r[0])), r[1] as! Int, "drawNeed \(r)") }
for r in rows("drawGiven") {
    expect(mathDrawGiven(str(r[0])).map { "\($0.0),\($0.1)" }, points(r[1]).map { "\($0.0),\($0.1)" }, "drawGiven \(r)")
}
for r in rows("draw") { expect(mathDrawOk(str(r[0]), points(r[1])), r[2] as! Bool, "draw \(r)") }

// Every worked answer in content/ must pass its own check.
var problems = 0
for section in ["mat0", "mat", "mat2", "mat3", "mat4"] {
    for lesson in content[section] as? [[String: Any]] ?? [] {
        for p in lesson["problems"] as? [[String: Any]] ?? [] {
            guard let spec = p["answer"] as? String else { continue }
            let at = "\(section) \(lesson["key"] ?? ""): \(spec)"
            if mathIsDraw(spec) {
                expect(mathDrawNeed(spec) > 0, true, at)
            } else {
                let body = spec.split(separator: ":", maxSplits: 1).last.map(String.init) ?? spec
                expect(mathAnswerOk(spec, body.components(separatedBy: "|")[0]), true, at)
            }
            problems += 1
        }
    }
}
expect(problems > 100, true, "content has math problems")

print("swift: \(checked - failures)/\(checked) ok")
exit(failures == 0 ? 0 : 1)
