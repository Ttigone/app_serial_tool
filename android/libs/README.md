# 离线构建依赖清单

本项目支持完全离线构建 Android APK。请按以下清单下载文件并放到指定目录。

## 一、Android 库依赖（放到 `android/libs/`）

目录：`e:\app_serial_tool\android\libs\`

| 文件名 | 下载地址 |
|--------|----------|
| `usb-serial-for-android-3.8.0.aar` | https://jitpack.io/com/github/mik3y/usb-serial-for-android/3.8.0/usb-serial-for-android-3.8.0.aar |
| `core-1.13.1.aar` | https://maven.google.com/androidx/core/core/1.13.1/core-1.13.1.aar |
| `annotation-1.8.0.jar` | https://maven.google.com/androidx/annotation/annotation/1.8.0/annotation-1.8.0.jar |
| `collection-1.4.0.jar` | https://maven.google.com/androidx/collection/collection/1.4.0/collection-1.4.0.jar |
| `lifecycle-common-2.8.1.jar` | https://maven.google.com/androidx/lifecycle/lifecycle-common/2.8.1/lifecycle-common-2.8.1.jar |
| `versionedparcelable-1.1.1.aar` | https://maven.google.com/androidx/versionedparcelable/versionedparcelable/1.1.1/versionedparcelable-1.1.1.aar |

## 二、Gradle Wrapper 文件（放到 `android/gradle/wrapper/`）

目录：`e:\app_serial_tool\android\gradle\wrapper\`

| 文件名 | 下载地址 |
|--------|----------|
| `gradle-8.7-bin.zip` | https://services.gradle.org/distributions/gradle-8.7-bin.zip |
| `gradle-wrapper.jar` | https://github.com/gradle/gradle/raw/v8.7.0/gradle/wrapper/gradle-wrapper.jar |

### ⚠️ 重要：验证 `gradle-wrapper.jar`

下载后必须验证 jar 是否完整。用压缩软件打开 `gradle-wrapper.jar`，查看 `META-INF/MANIFEST.MF` 文件内容，必须包含：
```
Main-Class: org.gradle.wrapper.GradleWrapperMain
```

如果提示 **"没有主清单属性"**，说明 jar 下载损坏或不完整，需要重新下载或手动修复。

## 三、构建配置检查

确保以下文件已按离线模式配置：

### 1. `android/build.gradle`

依赖部分应为：
```gradle
dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar', '*.aar'])

    // 在线构建时可取消注释以下依赖；离线构建时请保持注释
    // implementation 'androidx.core:core:1.13.1'
    // implementation 'com.github.mik3y:usb-serial-for-android:3.8.0'
}
```

### 2. `android/gradle.properties`

取消注释离线模式：
```properties
org.gradle.offline=true
```

### 3. `android/gradle/wrapper/gradle-wrapper.properties`

应为本地文件路径：
```properties
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=gradle-8.7-bin.zip
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
```

## 四、下载步骤

1. 找一台能联网的电脑。
2. 用浏览器或下载工具下载上表中的所有文件。
3. 将 6 个库文件拷贝到 `e:\app_serial_tool\android\libs\`。
4. 将 2 个 wrapper 文件拷贝到 `e:\app_serial_tool\android\gradle\wrapper\`。
5. **务必验证 `gradle-wrapper.jar` 的 MANIFEST.MF 包含 `Main-Class: org.gradle.wrapper.GradleWrapperMain`。**
6. 按第三节修改 `build.gradle`、`gradle.properties`、`gradle-wrapper.properties`。
7. 在 Qt Creator 中重新构建项目。

## 五、常见问题

**Q: 为什么用 Gradle 8.7 + AGP 8.5.0？**  
A: Qt 6.11.1 的 Android 构建工具链与该版本组合兼容性最好，且你本地已有对应环境。

**Q: 可以换其他 Gradle 版本吗？**  
A: 可以，但请确保 AGP 版本与 Gradle 版本兼容，并且 `gradle-wrapper.properties` 中的 `distributionUrl` 指向正确的 zip 文件。
