// Linux and Windows app: the same Compose UI as Android (../shared), packaged
// with its own Java runtime by jpackage.
plugins {
    id("org.jetbrains.kotlin.jvm")
    id("org.jetbrains.kotlin.plugin.compose")
    id("org.jetbrains.compose")
}

val commit: String = System.getenv("COMMIT")?.takeIf { it.isNotBlank() } ?: "dev"
val repoRoot: File = rootProject.projectDir.parentFile
val generated = layout.buildDirectory.dir("generated/resources")

kotlin {
    compilerOptions {
        jvmTarget.set(org.jetbrains.kotlin.gradle.dsl.JvmTarget.JVM_17)
    }
    sourceSets.named("main") {
        kotlin.srcDir("../shared/src")
    }
}

java {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
}

sourceSets.named("main") {
    resources.srcDir(generated)
}

dependencies {
    implementation(compose.desktop.currentOs)
    implementation(compose.material3)
    implementation("org.jetbrains.compose.material:material-icons-extended:1.7.3")
    implementation("org.json:json:20250517")
    testImplementation(kotlin("test"))
}

val buildContent = tasks.register<Exec>("buildContent") {
    val out = generated.get().file("content.json").asFile
    workingDir = repoRoot
    val python = System.getenv("PYTHON")?.takeIf { it.isNotBlank() }
        ?: if (System.getProperty("os.name").startsWith("Windows")) "python" else "python3"
    commandLine(python, "scripts/build-content.py", "--out", out.path)
    inputs.dir(repoRoot.resolve("content"))
    inputs.file(repoRoot.resolve("scripts/build-content.py"))
    outputs.file(out)
}

val buildInfo = tasks.register("buildInfo") {
    val out = generated.get().file("build-info.properties").asFile
    inputs.property("commit", commit)
    outputs.file(out)
    doLast {
        out.parentFile.mkdirs()
        out.writeText("commit=$commit\n")
    }
}

val appIcon = tasks.register<Copy>("appIcon") {
    from(repoRoot.resolve("data/share/icons/hicolor/256x256/apps/maturita.png"))
    into(generated)
    rename { "icon.png" }
}

tasks.named("processResources") {
    dependsOn(buildContent, buildInfo, appIcon)
}

tasks.withType<Test>().configureEach {
    useJUnitPlatform()
    systemProperty("graduately.repo", repoRoot.path)
}

compose.desktop {
    application {
        mainClass = "org.maturita.maturita.desktop.MainKt"
        // jpackage and ProGuard need a JDK with jmods (Temurin 21 has them, 24+ may not).
        System.getenv("PACKAGE_JDK")?.takeIf { it.isNotBlank() }?.let { javaHome = it }
        // Shrinking drops the thousands of unused icons and keeps the
        // packages under GitHub's 100 MB file limit.
        buildTypes.release.proguard {
            version.set("7.10.0")
            obfuscate.set(false)
            configurationFiles.from(project.file("proguard-rules.pro"))
        }
        nativeDistributions {
            // The launcher must stay graduately(.exe): installers and the
            // updater of older builds start it by that name.
            packageName = "graduately"
            packageVersion = "1.0.0"
            description = "Graduately"
            vendor = "Graduately"
            modules("java.naming", "jdk.crypto.ec", "jdk.unsupported")
            linux {
                iconFile.set(repoRoot.resolve("data/share/icons/hicolor/512x512/apps/maturita.png"))
            }
            windows {
                iconFile.set(repoRoot.resolve("assets/app-icon.ico"))
            }
        }
    }
}
