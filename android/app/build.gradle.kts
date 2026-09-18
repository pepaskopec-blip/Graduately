plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.plugin.compose")
}

val commit: String = System.getenv("COMMIT")?.takeIf { it.isNotBlank() } ?: "dev"

// Every CI build must be installable over the previous one, so the version
// code grows with the workflow run and local builds stay below it.
val ciRun: Int? = System.getenv("GITHUB_RUN_NUMBER")?.toIntOrNull()
val appVersionCode: Int = ciRun?.let { 100 + it } ?: 7

android {
    namespace = "org.maturita.maturita"
    compileSdk = 37

    defaultConfig {
        applicationId = "org.maturita.maturita"
        minSdk = 26
        targetSdk = 35
        versionCode = appVersionCode
        versionName = "android-$appVersionCode"
        buildConfigField("String", "COMMIT", "\"$commit\"")
        buildConfigField("String", "UPDATE_REPO", "\"pepaskopec-blip/maturita.c\"")
        buildConfigField("String", "UPDATE_BRANCH", "\"builds\"")
    }

    // Android only installs an update signed with the same key as the app
    // already on the phone. The default debug keystore is generated per
    // machine, so CI builds would never update each other; sign everything
    // with the repository's sideload key instead. It is not a secret (this
    // is a sideloaded open-source app), but a real key can be supplied
    // through MATURITA_KEYSTORE / MATURITA_KEYSTORE_PASSWORD.
    signingConfigs {
        create("sideload") {
            val ks = System.getenv("MATURITA_KEYSTORE")?.takeIf { it.isNotBlank() }
                ?: rootProject.file("keystore/sideload.jks").path
            val pass = System.getenv("MATURITA_KEYSTORE_PASSWORD")?.takeIf { it.isNotBlank() }
                ?: "maturita-sideload"
            storeFile = file(ks)
            storePassword = pass
            keyAlias = System.getenv("MATURITA_KEY_ALIAS")?.takeIf { it.isNotBlank() } ?: "maturita"
            keyPassword = System.getenv("MATURITA_KEY_PASSWORD")?.takeIf { it.isNotBlank() } ?: pass
        }
    }

    buildTypes {
        debug {
            signingConfig = signingConfigs.getByName("sideload")
        }
        release {
            isMinifyEnabled = false
            signingConfig = signingConfigs.getByName("sideload")
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro",
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    buildFeatures {
        compose = true
        buildConfig = true
    }
}

kotlin {
    compilerOptions {
        jvmTarget.set(org.jetbrains.kotlin.gradle.dsl.JvmTarget.JVM_17)
    }
}

dependencies {
    val composeBom = platform("androidx.compose:compose-bom:2024.12.01")
    implementation(composeBom)
    implementation("androidx.compose.ui:ui")
    implementation("androidx.compose.ui:ui-tooling-preview")
    implementation("androidx.compose.material3:material3")
    implementation("androidx.compose.material:material-icons-extended")
    implementation("androidx.core:core-ktx:1.15.0")
    implementation("androidx.activity:activity-compose:1.9.3")
    implementation("androidx.lifecycle:lifecycle-viewmodel-compose:2.8.7")
    implementation("androidx.lifecycle:lifecycle-runtime-compose:2.8.7")
    debugImplementation("androidx.compose.ui:ui-tooling")
}

val extractContent = tasks.register<Exec>("extractContent") {
    workingDir = rootProject.projectDir.parentFile
    commandLine("python3", "scripts/extract-android-content.py")
    inputs.dir(rootProject.projectDir.parentFile.resolve("src"))
    outputs.file(file("src/main/assets/content.json"))
}

tasks.named("preBuild").configure {
    dependsOn(extractContent)
}
