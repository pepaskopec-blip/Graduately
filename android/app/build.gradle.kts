plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.plugin.compose")
}

val commit: String = System.getenv("COMMIT")?.takeIf { it.isNotBlank() } ?: "dev"

android {
    namespace = "org.maturita.maturita"
    compileSdk = 37

    defaultConfig {
        applicationId = "org.maturita.maturita"
        minSdk = 26
        targetSdk = 35
        versionCode = 6
        versionName = "android-6"
        buildConfigField("String", "COMMIT", "\"$commit\"")
        buildConfigField("String", "UPDATE_REPO", "\"pepaskopec-blip/maturita.c\"")
        buildConfigField("String", "UPDATE_BRANCH", "\"builds\"")
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            signingConfig = signingConfigs.getByName("debug")
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
