BÁO CÁO THỰC HÀNH: PHÂN TÍCH LỖI BUFFER OVERFLOW
Họ và tên: Võ Nguyễn Nhật Nam
Mã số sinh viên: N23DCAT045
Môn học: An toàn hệ điều hành
Phân tích các ví dụ :
1. Ví dụ 1: Truy cập ngoài phạm vi (Out-of-bounds)
• Mã nguồn: Khai báo int array[5] (chỉ số từ 0-4) nhưng truy cập array[5].
• Kết quả: Chương trình in ra một giá trị rác trong bộ nhớ (vị trí tiếp theo sau mảng).
• Nhận xét: Đây là lỗi logic cơ bản, tuy chưa gây sụp đổ chương trình ngay lập tức nhưng có thể
làm rò rỉ thông tin bộ nhớ.
2. Ví dụ 2: Ghi đè bộ nhớ liên tục
• Mã nguồn: Mảng có 5 phần tử nhưng dùng vòng lặp for chạy đến 255 để gán giá trị 10.
• Kết quả: Xảy ra lỗi Segmentation Fault.
• Nhận xét: Việc ghi đè quá xa phạm vi mảng đã xâm phạm vào các vùng nhớ quản lý của hệ
thống hoặc dữ liệu của các biến khác, dẫn đến chương trình bị cưỡng chế dừng.
3. Ví dụ 3: Khảo sát Stack Frame bằng GDB
• Mục tiêu: Quan sát cách hàm function(a, b) được tổ chức trên Stack.
• Lệnh thực hiện:
Bash
gcc -fno-pie -no-pie -ggdb vidu3.c -o vidu3
gdb ./vidu3
(gdb) disas main
(gdb) disas function
• Phân tích: Khi gọi function(1, 2), các tham số được đẩy vào stack, sau đó là địa chỉ trả về
(Return Address) để sau khi hàm kết thúc, CPU biết quay lại lệnh printf trong main.
4. Ví dụ 4: Phân tích chi tiết lỗi tràn bộ đệm
1. Phân tích Source Code
void return_input (void){
 char array[30]; // Buffer 30 bytes
 gets (array); // LỖI NGUY HIỂM: Không kiểm tra độ dài nhập vào
 printf("%s\n", array);
}
• Lỗ hổng: Hàm gets() sẽ đọc dữ liệu cho đến khi gặp ký tự xuống dòng. Nếu người dùng nhập > 
30 ký tự, dữ liệu sẽ tràn ra khỏi array và ghi đè lên các ô nhớ lân cận trên Stack.
2. Phân tích Động với GDB (Phân tích Stack)
Qua quá trình debug, ta xác định được cấu trúc Stack Frame của hàm return_input như sau:
• Địa chỉ Buffer: Bắt đầu tại $ebp – 0x20 (tức là cách EBP 32 bytes).
• Saved EBP: Nằm tại địa chỉ 0xbffff598.
• Return Address: Nằm tại địa chỉ 0xbffff59c.
Bảng trạng thái bộ nhớ (Trước và sau khi nhập 50 ký tự):
3. Giải thích hiện tượng Segmentation Fault
Khi hàm return_input chạy đến lệnh ret, CPU thực hiện pop eip. Lúc này, giá trị tại địa chỉ 0xbffff59c (vốn 
đã bị ta ghi đè thành 0x43434343) được nạp vào thanh ghi lệnh EIP.
• CPU cố gắng thực thi mã tại địa chỉ 0x43434343.
• Vì đây là địa chỉ không hợp lệ/không có quyền thực thi, hệ điều hành gửi tín hiệu SIGSEGV, làm 
chương trình bị "crash".
Kết luận và Giải pháp phòng ngừa
1. Kết luận
• Lỗi Buffer Overflow xảy ra do sự thiếu kiểm soát kích thước dữ liệu đầu vào.
• Kẻ tấn công có thể lợi dụng lỗi này để thay đổi Return Address, từ đó thực thi các đoạn mã độc 
hại (shellcode) hoặc thay đổi logic chương trình.
2. Giải pháp
• Sử dụng hàm an toàn: Thay thế gets() bằng fgets(array, sizeof(array), stdin) để giới hạn số ký tự
đọc.
• Cơ chế bảo vệ của Compiler:
o Stack Canaries: Chèn một giá trị "ngẫu nhiên" trước Return Address để kiểm tra xem có 
bị ghi đè không.
o ASLR (Address Space Layout Randomization): Làm ngẫu nhiên địa chỉ bộ nhớ mỗi lần 
chạy.
o NX Bit (No-Execute): Ngăn chặn thực thi mã trên vùng Stack.
