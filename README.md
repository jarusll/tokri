# Tokri

Basket for your compooter.

## Building

### Prerequisites
Since this read `/dev/input/*` you need to be part of `input` group.
```bash
sudo usermod -aG input $USER
```
Note: Please log out and log back in for the group changes to take effect.

### Build
```bash
cmake -S . -B build
cmake --build build
```

## TODO
- Offer drag with multiple formats
- Free up gui thread from sending Image
- Add items from clipboard
- Tray icon
- Accept clipboard contents

## Logs
MVP
v2025.12.01-alpha.1
- I can drop files/directories/text

v2025.12.02
- It looks pretty

v2025.12.03
- It looks even prettier and you can drop images now

v2025.12.04
- Handle image drops
