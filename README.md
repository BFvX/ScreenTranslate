# Screen Translate (屏幕翻译工具)

## 免责声明

本项目的开发过程（包括这个README）大量使用了AI工具，包括谷歌的 Gemini 和 OpenAI 的 ChatGPT。因此，开发者不对最终代码的实现或功能承担任何责任。

## 这是什么
一个使用 C++ 和 Qt 6 构建的简洁桌面应用，可以捕捉屏幕的任意部分并使用 Google Gemini API 翻译其中的文本。

## 功能特性

*   **灵活的屏幕捕捉:**
    *   **单次选择:** 选择屏幕上的任何区域进行一次性翻译。
    *   **固定区域:** 定义一个特定区域，以便快速、重复地翻译同一位置。
*   **Gemini API 集成:** 利用支持视觉的 Gemini 模型进行文字识别和翻译。
*   **翻译历史:** 保留您最近的纯文本翻译记录，以提供对话上下文。
*   **代理支持:** 可为网络请求配置 HTTP 代理。
*   **自定义主题:** 可在浅色和深色模式之间切换。
*   **持久化设置:** 您的 API 密钥、区域和其他设置会在会话之间保存。

## 技术栈

*   **C++17**
*   **Qt 6:** 用于用户界面和应用逻辑。

## 从源码构建

### 前提条件

1.  **Qt 6:** 您必须安装 Qt 6 (包括 Core, Gui, Widgets, 和 Network 模块)。
2.  **C++ 编译器:** 支持 C++17 的现代 C++ 编译器 (例如 GCC, Clang, MSVC)。
3.  **构建工具:**
    *   **CMake** (3.16 或更高版本)
    *   或 **qmake**

### API 密钥设置

本应用需要 Google Gemini API 密钥才能运行。

1.  从 [Google AI Studio](https://aistudio.google.com/app/apikey) 获取 API 密钥。
2.  在应用内的 **Settings** (设置) 菜单中输入并保存您的 API 密钥。

### 构建说明

您可以使用 CMake 或 qmake 来构建项目。

#### 使用 CMake

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建项目
cmake --build .
```

#### 使用 qmake

```bash
# 创建构建目录
mkdir build
cd build

# 运行 qmake
qmake ../screen_translate.pro

# 运行 make (或在 Windows 上运行 nmake/jom)
make
```

## 使用方法

1.  **启动应用。**
2.  **设置 API 密钥:** 进入 **Settings** (设置) 并输入您的 Gemini API 密钥。您可以使用 "Test Connection" (测试连接) 按钮来验证它。
3.  **选择并翻译:** 点击 **"Select & Translate"** (选择并翻译) 按钮。您的屏幕会变暗，允许您在要翻译的文本上拖出一个矩形。翻译后的文本将出现在主窗口中。
4.  **固定区域:**
    *   点击 **"Set Fixed Region"** (设置固定区域) 来定义屏幕的特定区域。
    *   点击 **"Translate Fixed Region"** (翻译固定区域) 来再次捕捉和翻译该区域，无需重新选择。

## 许可证

本应用使用 Qt 6，它在 LGPLv3 许可下提供。LGPLv3 许可证的副本已包含在 `LICENSE.QT` 文件中。您可以在 [Qt 官方网站](https://www.qt.io/download-open-source) 上找到 Qt 的源代码。

## 致谢

本应用的用户界面风格和图标基于精美的 **PyDracula** 主题。

*   **项目:** [Modern_GUI_PyDracula_PySide6_or_PyQt6](https://github.com/Wanderson-Magalhaes/Modern_GUI_PyDracula_PySide6_or_PyQt6)
*   **作者:** Wanderson M. Pimenta

