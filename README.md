<img src="./resources/net.surajyadav.Tokri.png" width="128" height="128" alt="Tokri icon">

# Tokri

> A desktop basket to drag and drop text, URLs, images, and files.

- Shake while dragging to open the basket and drop items inside.
- Drag items **out** to move them
- Hold Ctrl (Windows/Linux) or ⌘ Command (macOS) while dragging to copy

## Images
![](./assets/image.gif)

## Text & URLs
![](./assets/text.gif)

## Files
![](./assets/files.gif)

## Download

### Windows - [Installer](https://github.com/jarusll/tokri/releases/download/v2026.01.01/TokriSetup.exe) - [Portable (.zip)](https://github.com/jarusll/tokri/releases/download/v2026.01.01/Tokri.zip)

### macOS - [DMG installer](https://github.com/jarusll/tokri/releases/download/v2026.01.01/Tokri.dmg)

#### Install via brew
```zsh
brew tap jarusll/tap
brew install --cask jarusll/tap/tokri
```

> **Note for macOS users**
>
> This app is **unsigned**, so macOS will block it on first launch.
>
> To run it:
>
> - Run in Terminal:
>   ```zsh
>   sudo /usr/bin/xattr -dr com.apple.quarantine /Applications/Tokri.app
>   ```
>
> Or allow it via:
> **System Settings → Privacy & Security → Open Anyway**

### Linux - [Flatpak bundle](https://github.com/jarusll/tokri/releases/download/v2026.01.01/Tokri.flatpak)

> **Note for Linux users**
>
> This application reads from `/dev/input/*` to detect mouse activation gestures.
> Add your user to the `input` group:
>
> ```bash
> sudo usermod -aG input $USER
> ```
>
> Log out and log back in for the change to take effect.

> **Note for KDE users**
>
> Mouse activation gestures do not work on **native Wayland** (KDE-specific limitation).
>
> Run the app under **XWayland** by disabling Wayland access:
> - Open **Flatseal**
> - Select the app
> - Remove the **Wayland** permission
>
> This forces XWayland and restores gesture support.
>
> ![](./assets/flatseal.png)

## Building for Linux

#### Dependencies
- qt6-base

### Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Acknowledgements
- 🎨 Design by [Akshay Majgaonkar](https://www.linkedin.com/in/akshay-majgaonkar/)

## TODO
- Open url on double click
- Publish on Flathub
- Double click on macOS dock should wakeup Tokri
- Add items from clipboard
- Placeholder text and Image
- Item count
