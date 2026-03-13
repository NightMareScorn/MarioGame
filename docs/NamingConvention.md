# C++ Naming Convention

## 1. Tệp và Thư mục (Files & Directories)
* **Thư mục:** Chữ thường; dùng dấu gạch dưới (`snake_case`) khi tên có nhiều từ.
    * Ví dụ: `engine`, `game`, `systems`, `gameover`, `game_over`.
* **Tệp mã nguồn (.h, .cpp):** `PascalCase`. Bắt buộc trùng khớp chính xác với tên Class/Struct chứa bên trong.
    * Ví dụ: `PlayerController.cpp`, `AnimationSystem.h`.

## 2. Cấu trúc Dữ liệu (Data Structures)
* **Class, Struct:** `PascalCase`. Sử dụng danh từ.
    * Ví dụ: `SpriteRenderer`, `CollisionData`, `Entity`.
* **Interface (Lớp thuần ảo):** Tiền tố `I` + `PascalCase`.
    * Ví dụ: `ICollidable`, `IUpdatable`.
* **Enum (Bắt buộc dùng `enum class`):** Tên Enum `PascalCase`, Giá trị Enum `PascalCase`.
    * Ví dụ: `enum class EntityState { Idle, Running, Jumping };`.

## 3. Hàm và Phương thức (Functions & Methods)
* **Hàm thành viên, Hàm tự do:** `PascalCase`. Bắt buộc bắt đầu bằng một động từ.
    * Ví dụ: `Initialize()`, `UpdatePhysics()`, `GetPosition()`.

## 4. Biến và Quản lý Bộ nhớ (Variables & Memory)
* **Biến :** `camelCase`.
    * Ví dụ: `moveSpeed`, `health`, `instanceCount`, `engineState`.
* **Con trỏ (Raw Pointers, DirectX COM Objects, Smart Pointers):** `camelCase`.
    * Ví dụ: `device`, `renderTarget`, `playerEntity`, `backgroundTexture`.
* **Hằng số (Const, Constexpr) & Macro (#define):** `UPPER_SNAKE_CASE`.
    * Ví dụ: `MAX_ENEMIES_PER_SCREEN`, `GRAVITY_FORCE`.

## 5. Kiểu Logic (Booleans)
* **Biến và Hàm trả về `bool`:** Sử dụng `camelCase` (với biến) hoặc `PascalCase` (với hàm), bắt buộc bắt đầu bằng `is`, `has`, `can`, `should`.
    * Ví dụ: `isGrounded`, `hasFireFlower`, `CanJump()`, `ShouldDespawn()`.