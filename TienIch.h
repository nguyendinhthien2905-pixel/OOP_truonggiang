// =====================================================================
//  TienIch.h - Cac ham tien ich cho giao dien Console (Windows)
//  Gom: mau sac, xoa man hinh, menu phim mui ten, nhap lieu, dinh dang
// =====================================================================
#ifndef TIENICH_H_INCLUDED_
#define TIENICH_H_INCLUDED_

#include <iostream>
#include <string>
#include <vector>

// Ma mau console Windows (4 bit: mau chu hoac mau nen)
enum Mau {
    MAU_DEN = 0,
    MAU_XAM = 7,
    MAU_XANH_DUONG = 9,
    MAU_XANH_LA = 10,
    MAU_XANH_NGOC = 11,
    MAU_DO = 12,
    MAU_TIM = 13,
    MAU_VANG = 14,
    MAU_TRANG = 15
};

// Cac phim ma chuong trinh quan tam
enum Phim {
    PHIM_KHAC,
    PHIM_LEN,
    PHIM_XUONG,
    PHIM_ENTER,
    PHIM_ESC
};

namespace TienIch {

    const int KHONG_CHON = -1;      // ChonMenu tra ve khi nhan Esc

    // ---- Console ----
    void KhoiTaoConsole();
    void DatMau(Mau mauChu);
    void DatMauNen(Mau mauChu, Mau mauNen);
    void DatMauMacDinh();
    void XoaManHinh();
    void DatViTriCon(int x, int y);
    void HienConTro(bool hien);

    // ---- Phim va menu ----
    Phim DocPhim();
    void TamDung();
    int ChonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc);

    // ---- In an ----
    void InTieuDe(const std::string& tieuDe);
    void InDong(const std::string& nhan, const std::string& giaTri, Mau mauGiaTri = MAU_TRANG);
    void BaoLoi(const std::string& thongBao);
    void BaoThanhCong(const std::string& thongBao);
    void BaoThongTin(const std::string& thongBao);

    // ---- Nhap lieu: tra ve false neu nguoi dung de trong (huy) hoac het du lieu ----
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa);
    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua);
    bool NhapSoNguyen(std::istream& is, const std::string& loiNhac, long long& ketQua,
                      long long giaTriNhoNhat, long long giaTriLonNhat);
    bool NhapSoThuc(std::istream& is, const std::string& loiNhac, double& ketQua,
                    double giaTriNhoNhat, double giaTriLonNhat);

    // ---- Dinh dang ----
    std::string DinhDangTien(long long soTien);
    std::string DinhDangPhanTram(double giaTri);
    std::string ChuyenInHoa(const std::string& chuoi);
    std::string CatKhoangTrang(const std::string& chuoi);   // Xoa khoang trang dau / cuoi

} // namespace TienIch

#endif
