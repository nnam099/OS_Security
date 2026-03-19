BÁO CÁO THỰC HÀNH: PHÂN TÍCH LỖI BUFFER OVERFLOW
Họ và tên: Võ Nguyễn Nhật Nam

Mã số sinh viên: N23DCAT045

Môn học: An toàn hệ điều hành

1. Ví dụ 1: Truy cập ngoài phạm vi (Out-of-bounds)
Mã nguồn: Khai báo int array[5] (chỉ số hợp lệ từ 0-4) nhưng thực hiện truy cập vào array[5].

Kết quả: Chương trình không báo lỗi ngay mà in ra một giá trị "rác" trong bộ nhớ (đây thực chất là giá trị nằm ở ô nhớ ngay sau mảng).

Nhận xét: Đây là lỗi logic cơ bản. Dù chưa gây sụp đổ (crash) chương trình ngay lập tức, nhưng nó gây ra lỗi rò rỉ thông tin bộ nhớ, có thể bị khai thác để tìm hiểu cấu trúc bộ nhớ của tiến trình.

2. Ví dụ 2: Ghi đè bộ nhớ liên tục
Mã nguồn: Mảng có kích thước 5 phần tử, nhưng sử dụng vòng lặp for chạy đến 255 để gán giá trị 10.

Kết quả: Xảy ra lỗi Segmentation Fault.

Nhận xét: Việc ghi đè quá xa phạm vi mảng đã xâm phạm vào các vùng nhớ quản lý của hệ thống (như Metadata của Heap hoặc Stack Frame) hoặc dữ liệu của các biến quan trọng khác. Hệ điều hành phát hiện sự xâm phạm vùng nhớ trái phép và cưỡng chế dừng chương trình để bảo vệ hệ thống.

3. Ví dụ 3: Khảo sát Stack Frame bằng GDB
Mục tiêu: Quan sát cách hàm function(a, b) được tổ chức trên Stack.

Các bước thực hiện:

Biên dịch không sử dụng các cơ chế bảo vệ:

Bash
gcc -fno-pie -no-pie -ggdb vidu3.c -o vidu3
Sử dụng GDB để phân tích:

Bash
gdb ./vidu3
(gdb) disas main
(gdb) disas function
Phân tích: Khi gọi function(1, 2), các tham số được đẩy vào Stack theo thứ tự ngược lại, sau đó là Return Address (địa chỉ trả về). Địa chỉ này cực kỳ quan trọng vì nó giúp CPU quay lại đúng lệnh tiếp theo trong hàm main sau khi function kết thúc.

4. Ví dụ 4: Phân tích chi tiết lỗi tràn bộ đệm (Buffer Overflow)
4.1. Phân tích Mã nguồn
C
void return_input (void){
    char array[30]; // Buffer 30 bytes
    gets (array);   // LỖI NGUY HIỂM: Không kiểm tra độ dài nhập vào
    printf("%s\n", array);
}
Lỗ hổng: Hàm gets() đọc dữ liệu cho đến khi gặp ký tự xuống dòng mà không quan tâm đến kích thước mảng đích. Nếu nhập > 30 ký tự, dữ liệu sẽ tràn ra ngoài array và ghi đè lên các ô nhớ lân cận trên Stack.

4.2. Phân tích Động với GDB
Cấu trúc Stack Frame của hàm return_input:

Địa chỉ Buffer: Bắt đầu tại $ebp – 0x20 (cách EBP 32 bytes).

Saved EBP: Nằm tại địa chỉ 0xbffff598.

Return Address: Nằm tại địa chỉ 0xbffff59c.

Gemini đã nói
Chào bạn Nhật Nam, một bài báo cáo thực hành về lỗ hổng phần mềm như Buffer Overflow cần sự trình bày khoa học, rành mạch và sử dụng định dạng Markdown chuẩn để hiển thị đẹp mắt trên GitHub (file .md).

Dưới đây là nội dung bài báo cáo của bạn đã được tôi tối ưu hóa cấu trúc, thêm các khối mã (code blocks) và bảng biểu để trông chuyên nghiệp hơn. Bạn có thể copy toàn bộ nội dung trong khung bên dưới để tạo file README.md hoặc BaoCao.md trên GitHub.

BÁO CÁO THỰC HÀNH: PHÂN TÍCH LỖI BUFFER OVERFLOW
Họ và tên: Võ Nguyễn Nhật Nam

Mã số sinh viên: N23DCAT045

Môn học: An toàn hệ điều hành

1. Ví dụ 1: Truy cập ngoài phạm vi (Out-of-bounds)
Mã nguồn: Khai báo int array[5] (chỉ số hợp lệ từ 0-4) nhưng thực hiện truy cập vào array[5].

Kết quả: Chương trình không báo lỗi ngay mà in ra một giá trị "rác" trong bộ nhớ (đây thực chất là giá trị nằm ở ô nhớ ngay sau mảng).

Nhận xét: Đây là lỗi logic cơ bản. Dù chưa gây sụp đổ (crash) chương trình ngay lập tức, nhưng nó gây ra lỗi rò rỉ thông tin bộ nhớ, có thể bị khai thác để tìm hiểu cấu trúc bộ nhớ của tiến trình.

2. Ví dụ 2: Ghi đè bộ nhớ liên tục
Mã nguồn: Mảng có kích thước 5 phần tử, nhưng sử dụng vòng lặp for chạy đến 255 để gán giá trị 10.

Kết quả: Xảy ra lỗi Segmentation Fault.

Nhận xét: Việc ghi đè quá xa phạm vi mảng đã xâm phạm vào các vùng nhớ quản lý của hệ thống (như Metadata của Heap hoặc Stack Frame) hoặc dữ liệu của các biến quan trọng khác. Hệ điều hành phát hiện sự xâm phạm vùng nhớ trái phép và cưỡng chế dừng chương trình để bảo vệ hệ thống.

3. Ví dụ 3: Khảo sát Stack Frame bằng GDB
Mục tiêu: Quan sát cách hàm function(a, b) được tổ chức trên Stack.

Các bước thực hiện:

Biên dịch không sử dụng các cơ chế bảo vệ:

Bash
gcc -fno-pie -no-pie -ggdb vidu3.c -o vidu3
Sử dụng GDB để phân tích:

Bash
gdb ./vidu3
(gdb) disas main
(gdb) disas function
Phân tích: Khi gọi function(1, 2), các tham số được đẩy vào Stack theo thứ tự ngược lại, sau đó là Return Address (địa chỉ trả về). Địa chỉ này cực kỳ quan trọng vì nó giúp CPU quay lại đúng lệnh tiếp theo trong hàm main sau khi function kết thúc.

4. Ví dụ 4: Phân tích chi tiết lỗi tràn bộ đệm (Buffer Overflow)
4.1. Phân tích Mã nguồn
C
void return_input (void){
    char array[30]; // Buffer 30 bytes
    gets (array);   // LỖI NGUY HIỂM: Không kiểm tra độ dài nhập vào
    printf("%s\n", array);
}
Lỗ hổng: Hàm gets() đọc dữ liệu cho đến khi gặp ký tự xuống dòng mà không quan tâm đến kích thước mảng đích. Nếu nhập > 30 ký tự, dữ liệu sẽ tràn ra ngoài array và ghi đè lên các ô nhớ lân cận trên Stack.

4.2. Phân tích Động với GDB
Cấu trúc Stack Frame của hàm return_input:

Địa chỉ Buffer: Bắt đầu tại $ebp – 0x20 (cách EBP 32 bytes).

Saved EBP: Nằm tại địa chỉ 0xbffff598.

Return Address: Nằm tại địa chỉ 0xbffff59c.

Bảng trạng thái bộ nhớ (Trước và sau khi nhập 50 ký tự 'C'):

Vùng nhớ	Địa chỉ	Giá trị trước khi nhập	Giá trị sau khi nhập
Buffer (array)	0xbffff578	Rác / Trống	0x43434343 ('CCCC')
Saved EBP	0xbffff598	Địa chỉ EBP cũ	0x43434343 (Bị ghi đè)
Return Address	0xbffff59c	Địa chỉ lệnh printf	0x43434343 (Bị ghi đè)
4.3. Giải thích hiện tượng Segmentation Fault
Khi hàm return_input thực hiện lệnh ret, CPU sẽ pop giá trị tại địa chỉ 0xbffff59c nạp vào thanh ghi lệnh EIP.

Lúc này, giá trị bị ghi đè là 0x43434343 ('CCCC') được nạp vào EIP.

CPU cố gắng nhảy đến địa chỉ 0x43434343 để thực thi lệnh tiếp theo.

Vì đây là địa chỉ không hợp lệ hoặc không có quyền thực thi, Hệ điều hành gửi tín hiệu SIGSEGV, khiến chương trình bị crash.

5. Kết luận và Giải pháp phòng ngừa
5.1. Kết luận
Lỗi Buffer Overflow xảy ra do sự thiếu kiểm soát kích thước dữ liệu đầu vào.

Kẻ tấn công có thể lợi dụng lỗi này để thay đổi Return Address, từ đó thực thi các đoạn mã độc hại (shellcode) hoặc thay đổi luồng điều khiển của chương trình.

5.2. Giải pháp
Sử dụng hàm an toàn: Thay thế gets() bằng fgets(array, sizeof(array), stdin) để giới hạn số ký tự đọc.

Cơ chế bảo vệ của Trình biên dịch (Compiler):

Stack Canaries: Chèn một giá trị "ngẫu nhiên" trước Return Address; nếu giá trị này bị thay đổi, chương trình sẽ tự dừng.

ASLR (Address Space Layout Randomization): Làm ngẫu nhiên địa chỉ bộ nhớ mỗi lần chạy để kẻ tấn công khó đoán định vị trí.

NX Bit (No-Execute): Đánh dấu vùng Stack là không thể thực thi mã, ngăn chặn shellcode.

Return Address: Nằm tại địa chỉ 0xbffff59c.

Bảng trạng thái bộ nhớ (Trước và sau khi nhập 50 ký tự 'C'):
