#ifndef TIENICH_H_INCLUDED_
#define TIENICH_H_INCLUDED_

#include <iostream>
#include <string>
#include <vector>

//mã màu
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

//các phím có ch?c n?ng thao tác
enum Phim {
    PHIM_KHAC,
    PHIM_LEN,
    PHIM_XUONG,
    PHIM_ENTER,
    PHIM_ESC
};

namespace TienIch {

    const int KHONG_CHON = -1;      // B?m phím ESC ?? thoát menu

    //console
    void KhoiTaoConsole();
    void DatMau(Mau mauChu);
    void DatMauNen(Mau mauChu, Mau mauNen);
    void DatMauMacDinh();
    void XoaManHinh();
    void DatViTriCon(int x, int y);
    void HienConTro(bool hien);

    //Phím và Menu
    Phim DocPhim();
    void TamDung();
    int ChonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc);

    //In
    void InTieuDe(const std::string& tieuDe);
    void InDong(const std::string& nhan, const std::string& giaTri, Mau mauGiaTri = MAU_TRANG);
    void BaoLoi(const std::string& thongBao);
    void BaoThanhCong(const std::string& thongBao);
    void BaoThongTin(const std::string& thongBao);

    //Nh?p li?u (tr? v? false n?u ng??i dùng ?? tr?ng (h?y) ho?c h?t d? li?u
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa);
    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua);
    bool NhapSoNguyen(std::istream& is, const std::string& loiNhac, long long& ketQua, long long giaTriNhoNhat, long long giaTriLonNhat);
    bool NhapSoThuc(std::istream& is, const std::string& loiNhac, double& ketQua, double giaTriNhoNhat, double giaTriLonNhat);

    //??nh d?ng
    std::string DinhDangTien(long long soTien);
    std::string DinhDangPhanTram(double giaTri);
    std::string ChuyenInHoa(const std::string& chuoi);
    std::string CatKhoangTrang(const std::string& chuoi);   // Xoa khoang trang dau / cuoi

}

#endif