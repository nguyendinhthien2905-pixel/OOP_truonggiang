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
    MAU_CHU_DAO = 6,        // màu chủ đạo cam #ec803e (khi console không hỗ trợ màu 24-bit thì dùng màu vàng đậm gần nhất)
    MAU_DO = 12,
    MAU_TIM = 13,
    MAU_VANG = 14,
    MAU_TRANG = 15
};

//các phím có chức năng thao tác
enum Phim {
    PHIM_KHAC,
    PHIM_LEN,
    PHIM_XUONG,
    PHIM_ENTER,
    PHIM_ESC
};

namespace TienIch {

    const int KHONG_CHON = -1;      // Bấm phím ESC để thoát menu

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
    //Bảng
    std::string KeDoc();
    void InKeNgangBang(const std::vector<int>& doRongCot, int loai);   // loai: 0 trên, 1 giữa, 2 dưới
    void BatDauBangThongTin();
    void KetThucBangThongTin();
    void BaoLoi(const std::string& thongBao);
    void BaoThanhCong(const std::string& thongBao);
    void BaoThongTin(const std::string& thongBao);

    //Nhập liệu (trả về false nếu người dùng để trống (hủy) hoặc hết dữ liệu
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa);
    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua);
    bool NhapSoNguyen(std::istream& is, const std::string& loiNhac, long long& ketQua, long long giaTriNhoNhat, long long giaTriLonNhat);
    bool NhapSoThuc(std::istream& is, const std::string& loiNhac, double& ketQua, double giaTriNhoNhat, double giaTriLonNhat);

    //Định dạng
    std::string DinhDangTien(long long soTien);
    std::string DinhDangPhanTram(double giaTri);
    std::string ChuyenInHoa(const std::string& chuoi);
    std::string CatKhoangTrang(const std::string& chuoi);   // Xoa khoang trang dau / cuoi

}

#endif