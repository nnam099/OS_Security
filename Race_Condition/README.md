1. GIỚI THIỆU
Race condition là một loại lỗ hổng bảo mật xảy ra khi kết quả của một chương trình phụ thuộc vào thời điểm và thứ tự thực thi của các tiến trình. Trong lab này, chúng ta tập trung vào lỗ hổng TOCTOU (Time-Of-Check to Time-Of-Use) - một dạng race condition phổ biến trong các chương trình thao tác với file theo đường dẫn (path).

Lỗ hổng xuất hiện khi chương trình:

Kiểm tra (Check) điều kiện an toàn của một đường dẫn

Sử dụng (Use) đường dẫn đó để mở/ghi file

Nếu giữa hai bước này có khoảng thời gian đủ để một tiến trình khác thay đổi đối tượng mà đường dẫn trỏ đến, kẻ tấn công có thể lợi dụng để ghi vào file nhạy cảm với quyền cao.

2. MỤC TIÊU LAB
Hiểu rõ nguyên lý hoạt động của lỗ hổng TOCTOU

Xác định được "khe hở" giữa thời điểm check và use

Xây dựng tình huống tấn công race condition thành công

Khai thác chương trình SUID để ghi đè vào file được bảo vệ

So sánh hiệu quả của giải pháp khắc phục

3. MÔ HÌNH LAB
3.1 Các thành phần chính
Thành phần	Mô tả	Vai trò
passwd	File mục tiêu thuộc sở hữu root	File cần bảo vệ, user thường không có quyền ghi
vulnerable-program	Chương trình SUID có lỗ hổng	Nạn nhân, chạy với quyền root
symbolic-link	Chương trình tạo symlink	Công cụ tấn công, đổi hướng file
exploit.sh	Script khai thác	Tự động hóa race condition
3.2 Sơ đồ luồng tấn công
text
┌─────────────────────────────────────────────────────────────────────────┐
│                          LUỒNG TẤN CÔNG                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  [Tiến trình A - vulnerable-program]                                    │
│                                                                         │
│  ┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐   │
│  │ CHECK           │     │    KHE HỞ       │     │ USE             │   │
│  │ access(file)    │ ──► │  DELAY loop     │ ──► │ fopen(file)     │   │
│  │ file = thường   │     │  (500000 vòng)  │     │ file = ???      │   │
│  └─────────────────┘     └─────────────────┘     └─────────────────┘   │
│                                   ▲                                     │
│                                   │                                     │
│  [Tiến trình B - symbolic-link]   │                                     │
│                                   │                                     │
│                         ┌─────────────────────┐                         │
│                         │ ĐỔI SYMLINK         │                         │
│                         │ unlink(file)        │                         │
│                         │ symlink → passwd    │                         │
│                         └─────────────────────┘                         │
│                                                                         │
│  KẾT QUẢ: fopen() mở passwd (file root) và ghi dữ liệu ✅               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
4. CÁC BƯỚC THỰC HIỆN
4.1 Thiết lập môi trường
Bước 1: Tạo người dùng lab2

bash
sudo adduser lab2
# Đặt mật khẩu: 123
Bước 2: Đăng nhập và tạo thư mục làm việc

bash
su - lab2
cd ~
mkdir lab2
cd lab2
Bước 3: Tạo các file nguồn

File	Nội dung chính
vulnerable-program.c	Chương trình có lỗ hổng TOCTOU
symbolic-link.c	Chương trình tạo symlink
exploit.sh	Script khai thác
vulnerable-program-fix.c	Phiên bản đã sửa lỗi
exploit-fix.sh	Script test bản vá
4.2 Biên dịch và thiết lập SUID
bash
# Chuyển sang root
sudo su
cd /home/lab2/lab2

# Biên dịch
clang vulnerable-program.c -o vulnerable-program
clang symbolic-link.c -o symbolic-link
clang vulnerable-program-fix.c -o vulnerable-program-fix

# Thiết lập SUID bit
chmod u+s vulnerable-program
chmod u+s vulnerable-program-fix

# Tạo file mục tiêu
echo "This is a file owned by the user root" > passwd
echo "password root is 123root" >> passwd

# Thoát root
exit
4.3 Thực hiện khai thác
bash
su - lab2
cd ~/lab2
chmod 755 exploit.sh exploit-fix.sh
./exploit.sh
5. KẾT QUẢ KHAI THÁC
5.1 Kết quả chạy exploit.sh
text
Starting attack...
No permission
No permission
STOP... The passwd file has been changed
This is a file owned by the user root
password root is 123root

TOCTOU-Attack-Success
5.2 Kiểm tra file passwd sau tấn công
bash
cat passwd
Kết quả:

text
This is a file owned by the user root
password root is 123root

TOCTOU-Attack-Success
Nhận xét: Dòng TOCTOU-Attack-Success đã được ghi thành công vào file passwd - file thuộc sở hữu của root. Điều này chứng tỏ cuộc tấn công đã thành công.

6. PHÂN TÍCH LỖ HỔNG
6.1 Mã nguồn có lỗ hổng
c
if (!access(fileName, W_OK)) {        // CHECK: kiểm tra quyền
    // DELAY - tạo khe hở
    for (i = 0; i < DELAY; i++) {
        int a = i ^ 2;
    }
    
    fileHandler = fopen(fileName, "a+");  // USE: mở file
    // ... ghi dữ liệu
}
6.2 Nguyên nhân
Vấn đề	Giải thích
Check và Use không atomic	Giữa access() và fopen() có khoảng thời gian
Vòng lặp DELAY	Cố tình tạo khe hở lớn để dễ khai thác
SUID program	Chương trình chạy với quyền root
Path không ổn định	Cùng một đường dẫn có thể trỏ đến file khác nhau
6.3 Cơ chế tấn công
Thời điểm CHECK: passwdlocal là file thường, user có quyền ghi

Khe hở: Chương trình thực hiện vòng lặp DELAY

Tấn công: Script chạy symbolic-link đổi passwdlocal thành symlink trỏ đến passwd

Thời điểm USE: fopen("passwdlocal") mở symlink → thực chất mở passwd

Kết quả: Ghi dữ liệu vào passwd với quyền root

7. CÁCH KHẮC PHỤC
7.1 Mã nguồn đã sửa
c
if (!access(fileName, W_OK)) {
    // DELAY
    for (i = 0; i < DELAY; i++) {
        int a = i ^ 2;
    }
    
    /* FIX: Hạ quyền xuống user thực tế */
    seteuid(getuid());    // ← Dòng sửa lỗi quan trọng
    
    fileHandler = fopen(fileName, "a+");
    // ... ghi dữ liệu
}
7.2 Giải thích giải pháp
Hàm	Tác dụng
getuid()	Lấy UID của user thực tế (lab2)
seteuid()	Đặt effective UID = UID thực tế
Kết quả: Trước khi mở file, chương trình hạ quyền từ root xuống lab2. Dù có mở được file passwd, cũng không có quyền ghi.

7.3 Kiểm tra bản vá
bash
./exploit-fix.sh
# (Chạy vài giây, nhấn Ctrl+C)
cat passwd
Kết quả sau khi chạy exploit-fix.sh:

text
This is a file owned by the user root
password root is 123root
Nhận xét: File passwd không bị thay đổi. Bản vá hoạt động hiệu quả.

8. KẾT LUẬN
8.1 Tổng kết
Nội dung	Kết quả
Khai thác lỗ hổng TOCTOU	✅ Thành công
Ghi đè file root	✅ Thành công
Bản vá có hiệu quả	✅ Đã kiểm chứng
8.2 Bài học rút ra
Path không phải định danh an toàn - Cùng một đường dẫn có thể trỏ đến các file khác nhau tại các thời điểm khác nhau

Check và Use cần atomic - Không nên để khoảng cách thời gian giữa kiểm tra và sử dụng

SUID program cần thận trọng - Nếu không được viết cẩn thận, có thể bị lợi dụng để leo thang đặc quyền

Nguyên tắc hạ quyền - Chỉ giữ đặc quyền cao khi thực sự cần thiết, hạ quyền ngay sau khi có thể

8.3 Đề xuất phòng tránh
Sử dụng file descriptor (open() trước, sau đó fstat() để kiểm tra)

Hạ quyền trước khi thao tác file nhạy cảm

Tránh sử dụng SUID khi không thực sự cần thiết

Kiểm tra kỹ các chương trình có sử dụng access() và fopen() trên cùng đường dẫn

