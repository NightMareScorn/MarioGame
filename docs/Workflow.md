# QUY TRÌNH LÀM VIỆC (DEVELOPMENT WORKFLOW) - MARIO PROJECT

Tài liệu này quy định chi tiết các bước thực hiện nhiệm vụ, quy chuẩn đặt tên và quy trình kiểm duyệt code để đảm bảo tính đồng nhất cho dự án.

## 1. Quy trình thực hiện Task (Step-by-Step)

Mỗi thành viên khi thực hiện một nhiệm vụ (Task) phải tuân thủ 6 bước sau:

### Bước 1: Tiếp nhận và Phân tích
- Kiểm tra danh sách Task trên **Notion**.
- Chuyển trạng thái Task từ `To Do` sang `In Progress`.
- Xác định mã Issue (VD: `#5`, `#7`) và các ràng buộc kỹ thuật (Xem Note V.1 & V.2).

### Bước 2: Khởi tạo Nhánh (Branching)
- Luôn cập nhật code mới nhất từ nhánh `main` trước khi tạo nhánh mới.
- **Cú pháp đặt tên nhánh:** `[loại]/[mã-issue]-[mô-tả-ngắn]`
    - `feat/`: Tính năng mới (Features).
    - `fix/`: Sửa lỗi (Bug fixes).
    - `docs/`: Tài liệu (Documentation).
    - `refactor/`: Tối ưu hóa/Cấu trúc lại code.
- **Ví dụ:** 
    - `docs/5-workflow-docs`
    - `feat/7-sequence-diagram`
    - `feat/10-aabb-collision`

```bash
git checkout main
git pull origin main
git checkout -b feat/7-sequence-diagram
```

### Bước 3: Thực hiện Task & Kiểm thử Local
- Code theo chuẩn OOP và Design Patterns đã thống nhất (Singleton cho Manager, FSM cho Mario).
- Nếu là Task code: Phải chạy thử (Debug Mode - F3) để đảm bảo không lỗi logic/văng game.
- Nếu là Task tài liệu: Đảm bảo định dạng `.md` hiển thị đúng trên GitHub.

### Bước 4: Commit Code (Commit Convention)
- Phải commit thường xuyên, không dồn quá nhiều thay đổi vào một commit duy nhất.
- **Cú pháp commit:** `[loại]: [mô tả hành động] #[mã-issue]`
- **Ví dụ:**
    - `docs: add workflow documentation #5`
    - `feat: implement AABB logic for Goomba interaction #7`
    - `fix: resolve corner noclip bug in world 1-1 #12`

```bash
git add .
git commit -m "feat: complete sequence diagram for movement and collision #7"
git push origin feat/7-sequence-diagram
```

### Bước 5: Tạo Pull Request (PR) & Review
- Truy cập GitHub, tạo PR từ nhánh vừa push vào nhánh `main`.
- **Tiêu đề PR:** `[Task #[mã-issue]] [Tên Task]` (VD: `[Task #7] Vẽ Sequence Diagram`).
- **Nội dung PR (Template):**
    ```markdown
    ## Mô tả thay đổi
    - Đã vẽ xong 3 sơ đồ sequence: Di chuyển, Va chạm, Tương tác vật phẩm.
    - Cập nhật tài liệu kỹ thuật trong folder /docs.
    
    ## Issue liên quan
    - Closes #7
    
    ## Link Notion
    - [Dán link task trên notion vào đây]
    
    ## Hình ảnh minh họa (nếu có)
    - [Screenshot/GIF/Link Draw.io]
    ```
- **Gán Reviewer:** Chọn nhóm trưởng hoặc thành viên khác để kiểm tra code.

### Bước 6: Hoàn tất Task
- Sau khi PR được **Approved** và **Merged**, tiến hành xóa nhánh local.
- Cập nhật trạng thái Task trên Notion thành `Done`.
- Dán link Pull Request đã merge vào mục "Comment" hoặc "Link" của Task trên Notion để lưu vết.

---

## 2. Quy định về Cấu trúc Thư mục và File

Để tránh "Spaghetti code", thành viên phải tuân thủ vị trí lưu trữ:
- `/src`: Toàn bộ mã nguồn C++.
    - `/src/Entities`: Player, Enemy, Items.
    - `/src/Framework`: Engine, Camera, Physics, Collision.
    - `/src/Scenes`: Menu, Level 1, GameOver.
- `/assets`: Chứa Texture, Sound, Map (JSON/CSV).
- `/docs`: Chứa tài liệu workflow, thiết kế, sơ đồ sequence.

---

## 3. Lưu ý Kỹ thuật quan trọng (Checklist trước khi PR)

1. **Memory Leak:** Đã giải phóng bộ nhớ (delete pointer) trong destructor chưa?
2. **Resource Manager:** Texture/Sound đã được load qua Manager chưa (không load thủ công)?
3. **Fixed Time Step:** Logic vật lý có nằm trong hàm update sử dụng delta time cố định không?
4. **Collision:** Đã xử lý va chạm AABB đúng hướng chưa (Top/Bottom/Left/Right)?
5. **Naming Convention:** 
    - Class: `PascalCase` (VD: `MarioPlayer`).
    - Variable/Function: `camelCase` (VD: `isJumping`, `updatePosition()`).