# Tokri

Basket for your Linux computer.

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
- qt6
- cmake
- gcc/g++

### Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## TODO
- Offer drag with multiple formats
- Free up gui thread from sending Image
- Add items from clipboard
- Accept clipboard contents
- Find(is this even needed)?

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

v2025.12.04
- It works on Michaelsoft Binbows
