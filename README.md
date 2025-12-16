# Tokri

Basket for your computer.

> **Note**
>
> This application reads from `/dev/input/*`.
> Add your user to the `input` group:
> ```bash
> sudo usermod -aG input $USER
> ```
> Log out and log back in for the change to take effect.

## Building

#### Dependencies
- qt6-base

### Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## TODO
- Spawn near cursor(Impossible on wayland it seems)
- Move mode for drag out
- Offer drag with multiple formats
- Free up gui thread from sending Image
- Add items from clipboard
- Accept clipboard contents
- Search(is this even needed)?
