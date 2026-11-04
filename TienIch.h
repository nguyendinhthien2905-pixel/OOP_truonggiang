#ifndef TIENICH_H_INCLUDED_
#define TIENICH_H_INCLUDED_

#include <iostream>
#include <string>
#include <vector>

//mã màu
enum Mau {
    MAU_DEN = 0,
    MAU_XAM = 7,            // màu nhãn (chữ bên trái của bảng thông tin)
    MAU_XANH_DUONG = 9,
    MAU_XANH_LA = 10,       // màu số tiền
    MAU_DO = 12,            // màu báo lỗi
    MAU_CHU_DAO = 13,
    MAU_VANG = 14,
    MAU_TRANG = 15          // màu giá trị
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
    const int DO_RONG_KHUNG = 70;   // độ rộng chung của mọi khung (tiêu đề, menu, bảng thông tin)

    //console
    void KhoiTaoConsole();
    void DatMau(Mau mauChu);
    void DatMauNen(Mau mauChu, Mau mauNen);
    void DatMauMacDinh();
    void XoaManHinh();
    void DatViTriCon(int x, int y);
    void HienConTro(bool hien);
    void ManHinhLoading(const std::string& tieuDe, int thoiGianMs);   // màn hình loading lúc khởi động

    int LayDoRongConsole();                                         // số cột của cửa sổ console hiện tại
    std::string LeTrai(int doRongNoiDung);                          // chuỗi khoảng trắng để đẩy nội dung ra giữa màn hình
    int DoRongHienThi(const std::string& chuoi);                    // số ô chữ thật sự chiếm trên màn hình (chữ có dấu = 1 ô)
    std::string CanTrai(const std::string& chuoi, int doRong);      // thay cho std::setw + std::left (setw đếm sai chữ có dấu)
    std::string CanPhai(const std::string& chuoi, int doRong);      // thay cho std::setw + std::right
    std::string LapLai(const std::string& chuoi, int soLan);        // lặp lại 1 chuỗi, dùng để vẽ đường kẻ ═══

    //Phím và Menu
    Phim DocPhim();
    void TamDung();
    int ChonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc, const std::string& dongTieuDeCot = "");

    //In
    void InTieuDe(const std::string& tieuDe);
    void InDong(const std::string& nhan, const std::string& giaTri, Mau mauGiaTri = MAU_TRANG);
    //Bảng
    std::string KeDoc();
    void InKeNgangBang(const std::vector<int>& doRongCot, int loai);   // loai: 0 trên, 1 giữa, 2 dưới
    void BatDauBangThongTin();      // in ╔════╗ và bật chế độ InDong in trong khung
    void InNganCach();              // chia khung thông tin thành từng nhóm
    void KetThucBangThongTin();     // in ╚════╝
    void BaoLoi(const std::string& thongBao);
    void BaoThanhCong(const std::string& thongBao);
    void BaoThongTin(const std::string& thongBao);

    //Nhập liệu (trả về false nếu người dùng để trống (hủy) hoặc hết dữ liệu
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa);
    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua);
    bool NhapSoNguyen(std::istream& is, const std::string& loiNhac, long long& ketQua, long long giaTriNhoNhat, long long giaTriLonNhat);
    bool NhapSoThuc(std::istream& is, const std::string& loiNhac, double& ketQua, double giaTriNhoNhat, double giaTriLonNhat);

    bool NhapSoTienTrongKhung(const std::string& loiNhac, long long& ketQua, long long giaTriNhoNhat, long long giaTriLonNhat);
    bool XacNhanGiaoDich();         // Enter = xác nhận (true), Esc = hủy (false)

    //Định dạng
    std::string DinhDangTien(long long soTien);
    std::string DinhDangPhanTram(double giaTri);
    std::string ChuyenInHoa(const std::string& chuoi);
    std::string CatKhoangTrang(const std::string& chuoi);   // Xoa khoang trang dau / cuoi

}

#endif