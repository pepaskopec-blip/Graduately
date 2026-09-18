import Foundation
import UIKit

struct UpdateState {
    var status = "idle"
    var remote: String?
    var messageKey = "update_current"
    var messageArg: String? = String(AppConfig.commit.prefix(7))
    var canInstall = false
}

final class Updater {
    private let repo = AppConfig.updateRepo
    private let branch = AppConfig.updateBranch

    func check(interactive: Bool, cb: @escaping (UpdateState) -> Void) {
        if AppConfig.commit == "dev" {
            emit(cb, UpdateState(status: "dev", messageKey: "update_err_devbuild", messageArg: nil))
            return
        }
        emit(cb, UpdateState(status: "checking", messageKey: "update_checking", messageArg: nil))
        DispatchQueue.global(qos: .utility).async {
            do {
                let atom = try self.get("https://github.com/\(self.repo)/commits/\(self.branch).atom")
                let remote: String
                // VERSION-ios names the commit whose IPA is really in the
                // branch; it is missing only when the iOS job failed or on
                // older branches, where the branch-wide commit is used.
                if let tip = self.atomTip(atom),
                   let marker = try? self.get("https://raw.githubusercontent.com/\(self.repo)/\(tip)/VERSION-ios").trimmingCharacters(in: .whitespacesAndNewlines),
                   marker.count >= 7 {
                    remote = marker
                } else if let sha = self.atomMainSha(atom) {
                    remote = sha
                } else if let tip = self.atomTip(atom) {
                    remote = try self.get("https://raw.githubusercontent.com/\(self.repo)/\(tip)/VERSION").trimmingCharacters(in: .whitespacesAndNewlines)
                } else {
                    throw URLError(.cannotParseResponse)
                }
                let local = AppConfig.commit
                if self.sameCommit(remote, local) {
                    self.emit(cb, UpdateState(status: "ok", remote: remote, messageKey: "update_uptodate", messageArg: String(remote.prefix(7))))
                } else {
                    self.emit(cb, UpdateState(status: "available", remote: remote, messageKey: "update_available", messageArg: String(remote.prefix(7)), canInstall: true))
                }
            } catch {
                self.emit(cb, UpdateState(status: "error", messageKey: "update_err_network", messageArg: nil))
            }
        }
    }

    func install(cb: @escaping (UpdateState) -> Void) {
        emit(cb, UpdateState(status: "downloading", messageKey: "update_downloading", messageArg: nil))
        DispatchQueue.global(qos: .utility).async {
            do {
                let atom = try self.get("https://github.com/\(self.repo)/commits/\(self.branch).atom")
                let tip = self.atomTip(atom) ?? ""
                let page = "https://github.com/\(self.repo)/raw/main/installers/maturita-ios.html"
                DispatchQueue.main.async {
                    if let url = URL(string: page) {
                        UIApplication.shared.open(url)
                    }
                }
                self.emit(cb, UpdateState(status: "staged", remote: tip, messageKey: "update_ios_sideload", messageArg: String(tip.prefix(7))))
            } catch {
                self.emit(cb, UpdateState(status: "error", messageKey: "update_err_download", messageArg: nil))
            }
        }
    }

    private func emit(_ cb: @escaping (UpdateState) -> Void, _ state: UpdateState) {
        DispatchQueue.main.async { cb(state) }
    }

    private func atomTip(_ atom: String) -> String? {
        if let m = atom.range(of: #"Commit/([0-9a-f]{40})"#, options: [.regularExpression, .caseInsensitive]) {
            let s = String(atom[m])
            return s.split(separator: "/").last.map(String.init)
        }
        if let m = atom.range(of: #"/commit/([0-9a-f]{7,40})"#, options: [.regularExpression, .caseInsensitive]) {
            return String(atom[m]).split(separator: "/").last.map(String.init)
        }
        return nil
    }

    private func atomMainSha(_ atom: String) -> String? {
        guard let regex = try? NSRegularExpression(pattern: #"Build from ([0-9a-f]{7,40})"#, options: .caseInsensitive),
              let match = regex.firstMatch(in: atom, range: NSRange(atom.startIndex..., in: atom)),
              let range = Range(match.range(at: 1), in: atom)
        else { return nil }
        return String(atom[range])
    }

    private func sameCommit(_ a: String, _ b: String) -> Bool {
        let n = min(a.count, b.count)
        return n >= 7 && a.prefix(n).lowercased() == b.prefix(n).lowercased()
    }

    private func get(_ url: String) throws -> String {
        guard let u = URL(string: url) else { throw URLError(.badURL) }
        var req = URLRequest(url: u, timeoutInterval: 15)
        req.setValue("maturita.c-ios", forHTTPHeaderField: "User-Agent")
        req.setValue("*/*", forHTTPHeaderField: "Accept")
        let sem = DispatchSemaphore(value: 0)
        var result: Result<String, Error> = .failure(URLError(.unknown))
        URLSession.shared.dataTask(with: req) { data, resp, err in
            if let err { result = .failure(err) }
            else if let http = resp as? HTTPURLResponse, !(200...299).contains(http.statusCode) {
                result = .failure(URLError(.badServerResponse))
            } else if let data, let text = String(data: data, encoding: .utf8) {
                result = .success(text)
            } else {
                result = .failure(URLError(.cannotDecodeContentData))
            }
            sem.signal()
        }.resume()
        _ = sem.wait(timeout: .now() + 20)
        return try result.get()
    }
}
