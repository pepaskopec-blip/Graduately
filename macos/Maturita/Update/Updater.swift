import AppKit
import Foundation

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
                if let tip = self.atomTip(atom),
                   let marker = try? self.get("https://raw.githubusercontent.com/\(self.repo)/\(tip)/VERSION-macos")
                    .trimmingCharacters(in: .whitespacesAndNewlines),
                   marker.count >= 7 {
                    remote = marker
                } else if let sha = self.atomMainSha(atom) {
                    remote = sha
                } else if let tip = self.atomTip(atom) {
                    remote = try self.get("https://raw.githubusercontent.com/\(self.repo)/\(tip)/VERSION")
                        .trimmingCharacters(in: .whitespacesAndNewlines)
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
                guard !tip.isEmpty else { throw URLError(.cannotParseResponse) }
                let zipName = Self.assetName()
                let url = "https://raw.githubusercontent.com/\(self.repo)/\(tip)/\(zipName)"
                let cache = FileManager.default.urls(for: .cachesDirectory, in: .userDomainMask).first
                    ?? URL(fileURLWithPath: NSTemporaryDirectory())
                let staging = cache.appendingPathComponent("maturita-update-\(ProcessInfo.processInfo.processIdentifier)")
                try? FileManager.default.removeItem(at: staging)
                try FileManager.default.createDirectory(at: staging, withIntermediateDirectories: true)
                let archive = staging.appendingPathComponent(zipName)
                try self.download(url, to: archive)
                let unpacked = staging.appendingPathComponent("new")
                try FileManager.default.createDirectory(at: unpacked, withIntermediateDirectories: true)
                try self.run("/usr/bin/ditto", ["-x", "-k", archive.path, unpacked.path])
                guard let source = self.findApp(in: unpacked) else {
                    throw URLError(.cannotParseResponse)
                }
                guard let target = Self.runningAppURL() else {
                    throw URLError(.cannotOpenFile)
                }
                let script = self.writeSwapScript()
                let proc = Process()
                proc.executableURL = URL(fileURLWithPath: "/bin/sh")
                proc.arguments = [script.path, "\(ProcessInfo.processInfo.processIdentifier)", target.path, source.path, staging.path]
                try proc.run()
                self.emit(cb, UpdateState(status: "staged", remote: tip, messageKey: "update_staged", messageArg: String(tip.prefix(7))))
                DispatchQueue.main.async {
                    NSApp.terminate(nil)
                }
            } catch {
                self.emit(cb, UpdateState(status: "error", messageKey: "update_err_download", messageArg: nil))
            }
        }
    }

    private static func assetName() -> String {
        #if arch(arm64)
        return "maturita-macos-arm64.zip"
        #else
        return "maturita-macos-x86_64.zip"
        #endif
    }

    private static func runningAppURL() -> URL? {
        let bundle = Bundle.main.bundleURL
        return bundle.pathExtension == "app" ? bundle : nil
    }

    private func findApp(in root: URL) -> URL? {
        let keys: [URLResourceKey] = [.isDirectoryKey]
        guard let en = FileManager.default.enumerator(at: root, includingPropertiesForKeys: keys) else { return nil }
        for case let url as URL in en where url.pathExtension == "app" {
            return url
        }
        return nil
    }

    private func writeSwapScript() -> URL {
        let url = URL(fileURLWithPath: NSTemporaryDirectory()).appendingPathComponent("maturita-swift-update.sh")
        let body = """
        #!/bin/sh
        if [ -z "$MATURITA_UPDATE_DETACHED" ]; then
          MATURITA_UPDATE_DETACHED=1
          export MATURITA_UPDATE_DETACHED
          nohup /bin/sh "$0" "$@" >/dev/null 2>&1 &
          exit 0
        fi
        pid=$1; target=$2; source=$3; staging=$4
        n=0
        while kill -0 "$pid" 2>/dev/null && [ $n -lt 600 ]; do
          sleep 0.2; n=$((n+1))
        done
        place=$target
        if /usr/bin/ditto "$source" "$target" 2>/dev/null; then
          xattr -cr "$target" 2>/dev/null
        else
          alt="$HOME/Applications/$(basename "$target")"
          mkdir -p "$HOME/Applications"
          if /usr/bin/ditto "$source" "$alt" 2>/dev/null; then
            xattr -cr "$alt" 2>/dev/null
            place=$alt
          fi
        fi
        open -n "$place"
        rm -rf "$staging"
        rm -f "$0"
        """
        try? body.write(to: url, atomically: true, encoding: .utf8)
        try? FileManager.default.setAttributes([.posixPermissions: 0o755], ofItemAtPath: url.path)
        return url
    }

    private func emit(_ cb: @escaping (UpdateState) -> Void, _ state: UpdateState) {
        DispatchQueue.main.async { cb(state) }
    }

    private func atomTip(_ atom: String) -> String? {
        if let m = atom.range(of: #"Commit/([0-9a-f]{40})"#, options: [.regularExpression, .caseInsensitive]) {
            return String(atom[m]).split(separator: "/").last.map(String.init)
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
        req.setValue("Graduately-macos", forHTTPHeaderField: "User-Agent")
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

    private func download(_ url: String, to dest: URL) throws {
        guard let u = URL(string: url) else { throw URLError(.badURL) }
        let sem = DispatchSemaphore(value: 0)
        var result: Result<Void, Error> = .failure(URLError(.unknown))
        let task = URLSession.shared.downloadTask(with: u) { file, resp, err in
            if let err { result = .failure(err) }
            else if let http = resp as? HTTPURLResponse, !(200...299).contains(http.statusCode) {
                result = .failure(URLError(.badServerResponse))
            } else if let file {
                do {
                    try? FileManager.default.removeItem(at: dest)
                    try FileManager.default.moveItem(at: file, to: dest)
                    result = .success(())
                } catch {
                    result = .failure(error)
                }
            } else {
                result = .failure(URLError(.cannotDecodeContentData))
            }
            sem.signal()
        }
        task.resume()
        _ = sem.wait(timeout: .now() + 1800)
        try result.get()
    }

    private func run(_ launch: String, _ args: [String]) throws {
        let proc = Process()
        proc.executableURL = URL(fileURLWithPath: launch)
        proc.arguments = args
        try proc.run()
        proc.waitUntilExit()
        if proc.terminationStatus != 0 {
            throw URLError(.cannotParseResponse)
        }
    }
}
