#include <stdio.h>
#include <string.h>

// Thêm khai báo này để "lừa" trình biên dịch bỏ qua lỗi gets
char *gets(char *s); 

void return_input (void){
    char array[30];
    gets (array);
    printf("%s\n", array);
}

int main(){ // Thêm 'int' ở đây
    return_input();
    return 0;
}