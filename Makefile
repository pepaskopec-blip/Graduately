# Every app reads the lessons from content/ (built into content.json).
#   Android, Linux, Windows: Kotlin + Compose in android/ (shared code in android/shared)
#   iOS, macOS:              Swift + SwiftUI in ios/ and macos/
#
# Published builds pass COMMIT (the CI workflow exports it); the updaters
# compare it against the build in the repository. Without it a build stays
# "dev" and never offers an update.

GRADLE = cd android && ./gradlew

all: content

content:
	python3 scripts/build-content.py

# Runs the desktop app from source on this machine (Linux, Windows or macOS).
run:
	$(GRADLE) :desktop:run

# Answer checks shared by Kotlin and Swift (tests/answers.json), plus
# progress migration and import of GTK progress.
test:
	$(GRADLE) :desktop:test
	@if command -v xcrun >/dev/null 2>&1; then \
	  chmod +x scripts/test-answers-swift.sh && ./scripts/test-answers-swift.sh; \
	else \
	  echo "Swift checks skipped (no Xcode)"; \
	fi

android:
	$(GRADLE) :app:assembleRelease

linux:
	chmod +x scripts/bundle-linux.sh
	./scripts/bundle-linux.sh

windows:
	bash scripts/bundle-windows.sh

ios:
	chmod +x scripts/bundle-ios.sh
	./scripts/bundle-ios.sh

macos:
	chmod +x scripts/bundle-macos-swift.sh
	./scripts/bundle-macos-swift.sh

# Browsers render .command/.sh/.cmd as text/plain. Rebuild these zips after
# changing a script so the README download links stay in sync. The macOS zip
# is a double-clickable .app — no Terminal, no chmod.
installer-zips:
	chmod +x installers/pack-installer-zips.sh
	./installers/pack-installer-zips.sh

smoke:
	chmod +x scripts/smoke-platforms.sh
	./scripts/smoke-platforms.sh

clean:
	rm -rf build dist dist-android dist-ios
	$(GRADLE) clean

.PHONY: all content run test android linux windows ios macos installer-zips smoke clean
