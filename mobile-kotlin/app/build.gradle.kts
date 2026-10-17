plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.rahmanyazgan.finansasistani"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.rahmanyazgan.finansasistani"
        minSdk = 26
        targetSdk = 36
        versionCode = 4
        versionName = "1.0.3"
    }
    signingConfigs {
        create("release") {
            storeFile = file("keystore.jks")
            storePassword = "1e4e23452ab872f7750605b0b96de44d"
            keyAlias = "01659a8da958cf7e0f4c196f2e9bcfb3"
            keyPassword = "1d9dd98ec1f60c08f7fd1c210e2ac5fc"
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
    buildFeatures {
        compose = true
    }
    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.1"
    }
    buildTypes {
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
            signingConfig = signingConfigs.getByName("release")
        }
    }
}

dependencies {
    implementation("androidx.core:core-ktx:1.12.0")
    implementation("androidx.lifecycle:lifecycle-runtime-ktx:2.7.0")
    implementation("androidx.activity:activity-compose:1.8.2")
    implementation(platform("androidx.compose:compose-bom:2023.08.00"))
    implementation("androidx.compose.ui:ui")
    implementation("androidx.compose.ui:ui-graphics")
    implementation("androidx.compose.ui:ui-tooling-preview")
    implementation("androidx.compose.material3:material3")
    implementation("androidx.navigation:navigation-compose:2.7.7")
    implementation("com.google.android.material:material:1.11.0")
    implementation("androidx.compose.material:material-icons-extended")
}
