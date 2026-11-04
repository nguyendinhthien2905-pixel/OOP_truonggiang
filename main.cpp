// ===
#include "HangSo.h"
#include "NganHang.h"
#include "TienIch.h"

int main() {
    TienIch::KhoiTaoConsole();
    TienIch::ManHinhLoading(TIEU_DE_CONSOLE, THOI_GIAN_LOADING_MS);
    NganHang nganHang;
    nganHang.Chay();
    return 0;
}