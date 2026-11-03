//  TaiKhoanThanhToan.cpp - Cai dat lop TaiKhoanThanhToan
#include "TaiKhoanThanhToan.h"

#include "HangSo.h"
#include "TienIch.h"

TaiKhoanThanhToan::TaiKhoanThanhToan()
    : TaiKhoan(), m_HanMucTinDung(0) {
}

TaiKhoanThanhToan::TaiKhoanThanhToan(const std::string& soTaiKhoan, const std::string& tenChuTaiKhoan,
    long long soDu, long long hanMucTinDung)
    : TaiKhoan(soTaiKhoan, tenChuTaiKhoan, soDu), m_HanMucTinDung(hanMucTinDung < 0 ? 0 : hanMucTinDung) {
}

long long TaiKhoanThanhToan::LayHanMucTinDung() const {
    return m_HanMucTinDung;
}

void TaiKhoanThanhToan::DatHanMucTinDung(long long hanMucTinDung) {
    m_HanMucTinDung = (hanMucTinDung < 0) ? 0 : hanMucTinDung;
}


 // tien toi da co the rut ngay luc nay = So du + Han muc

long long TaiKhoanThanhToan::LaySoTienKhaDung() const {
    return LaySoDu() + m_HanMucTinDung;
}

//n So tien dang no (so du am). Khong no => 0

long long TaiKhoanThanhToan::LayDuNo() const {
    return (LaySoDu() < 0) ? -LaySoDu() : 0;
}

/* @Description Rut tien thanh toan. Dieu kien rieng:
 *                So du sau khi rut duoc phep am, nhung khong
 *                am qua han muc tin dung (So du - So tien >= -Han muc)
 */
bool TaiKhoanThanhToan::RutTien(long long soTien, std::string& thongBao) {
    if (!KiemTraSoTienRut(soTien, thongBao)) {
        return false;
    }
    if (soTien > LaySoTienKhaDung()) {
        thongBao = "Vượt hạn mức tín dụng. Số tiền khả dụng tối đa: "
            + TienIch::DinhDangTien(LaySoTienKhaDung()) + " VND.";
        return false;
    }
    DatSoDu(LaySoDu() - soTien);
    thongBao = "Rút tiền thành công.";
    if (LaySoDu() < 0) {
        thongBao += " Tài khoản đang ghi nợ " + TienIch::DinhDangTien(LayDuNo())
            + " VND (trong hạn mức tín dụng).";
    }
    return true;
}

// In thong tin chung + han muc, du no, so tien kha dung

void TaiKhoanThanhToan::HienThiThongTin() const {
    TaiKhoan::HienThiThongTin();            // nhom 1 + dong "So du hien tai"
    TienIch::InDong("Dư nợ hiện tại", TienIch::DinhDangTien(LayDuNo()) + " VND",
        LayDuNo() > 0 ? MAU_DO : MAU_XANH_LA);
    TienIch::InNganCach();
    HienThiHanMucRut();
    TienIch::InDong("Trạng thái", LayDuNo() > 0 ? "Đang dùng hạn mức tín dụng" : "Bình thường",
        LayDuNo() > 0 ? MAU_DO : MAU_XANH_LA);
}

void TaiKhoanThanhToan::HienThiHanMucRut() const {
    TienIch::InDong("Hạn mức tín dụng", TienIch::DinhDangTien(m_HanMucTinDung) + " VND", MAU_VANG);
    TienIch::InDong("Số tiền khả dụng", TienIch::DinhDangTien(LaySoTienKhaDung()) + " VND", MAU_XANH_LA);
}

std::string TaiKhoanThanhToan::LayLoaiTaiKhoan() const {
    return "Thanh toán";
}

std::string TaiKhoanThanhToan::LayThongSoRieng() const {
    return "Hạn mức " + TienIch::DinhDangTien(m_HanMucTinDung);
}

std::string TaiKhoanThanhToan::LayQuyDinhRut() const {
    return "Được rút vượt số dư trong hạn mức tín dụng (" + TienIch::DinhDangTien(m_HanMucTinDung)
        + " VND).";
}

// THANHTOAN|SoTaiKhoan|Ten|SoDu|HanMucTinDung

std::string TaiKhoanThanhToan::LayDongFile() const {
    return std::string(LOAI_FILE_THANH_TOAN) + KY_TU_NGAN_CACH + LayDongChung()
        + KY_TU_NGAN_CACH + std::to_string(m_HanMucTinDung);
}

// Nhap thong tin chung roi nhap han muc tin dung

void TaiKhoanThanhToan::Nhap(std::istream& is) {
    TaiKhoan::Nhap(is);
    if (!is) {
        return;
    }
    long long hanMuc = 0;
    if (!TienIch::NhapSoNguyen(is, "  Hạn mức tín dụng (VND): ", hanMuc, 0, HAN_MUC_TIN_DUNG_TOI_DA)) {
        is.setstate(std::ios::failbit);
        return;
    }
    m_HanMucTinDung = hanMuc;
}