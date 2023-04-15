#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct {
    char mssv[10];
    char hoten[50];
    char ngaysinh[20];
    float diemtb;
} SinhVien;

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    SinhVien sv;

    if (argc != 3) {
        printf("Sử dụng: %s <địa chỉ IP> <cổng>\n", argv[0]);
        return 1;
    }

    // Khởi tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return 1;
    }

    // Thiết lập địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    // Kết nối tới server
    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("connect failed");
        return 1;
    }

    // Nhập thông tin sinh viên
    printf("Nhập thông tin sinh viên:\n");
    printf("MSSV: ");
    fgets(sv.mssv, sizeof(sv.mssv), stdin);
    sv.mssv[strcspn(sv.mssv, "\n")] = '\0';

    printf("Họ tên: ");
    fgets(sv.hoten, sizeof(sv.hoten), stdin);
    sv.hoten[strcspn(sv.hoten, "\n")] = '\0';

    printf("Ngày sinh: ");
    fgets(sv.ngaysinh, sizeof(sv.ngaysinh), stdin);
    sv.ngaysinh[strcspn(sv.ngaysinh, "\n")] = '\0';

    printf("Điểm trung bình các môn học: ");
    scanf("%f", &sv.diemtb);
    getchar(); // Đọc ký tự newline thừa

    // Đóng gói dữ liệu
    snprintf(buffer, BUFFER_SIZE, "%s,%s,%s,%f", sv.mssv, sv.hoten, sv.ngaysinh, sv.diemtb);

    // Gửi dữ liệu đến server
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("send failed");
        return 1;
    }

    printf("Đã gửi thông tin sinh viên đến server.\n");

    // Đóng kết nối
    close(sock);

    return 0;
}
