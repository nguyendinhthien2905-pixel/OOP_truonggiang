
#include "TaiKhoanTietKiem.h"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "HangSo.h"
#include "TienIch.h"

TaiKhoanTietKiem::TaiKhoanTietKiem()
    : TaiKhoan(), m_LaiSuat(0.0) {
}

TaiKhoanTietKiem::TaiKhoanTietKiem(const std::string& soTaiKhoan, const std::string& tenChuTaiKhoan,
                                   long long soDu, double laiSuat)
    : TaiKhoan(soTaiKhoan, tenChuTaiKhoan, soDu), m_LaiSuat(laiSuat < 0.0 ? 0.0 : laiSuat) {
}

double TaiKhoanTietKiem::LayLaiSuat() const {
    return m_LaiSuat;
}

void TaiKhoanTietKiem::DatLaiSuat(double laiSuat) {
    m_LaiSuat = (laiSuat < 0.0) ? 0.0 : laiSuat;
}


long long TaiKhoanTietKiem::TinhTienLaiThang() const {
    if (LaySoDu() <= 0) {
        return 0;
    }
    return static_cast<long long>(std::llround(LaySoDu() * m_LaiSuat / 100.0 / 12.0));
}


long long TaiKhoanTietKiem::TinhTienLaiNam() const {
    if (LaySoDu() <= 0) {
        return 0;
    }
    return static_cast<long long>(std::llround(LaySoDu() * m_LaiSuat / 100.0));
}


bool TaiKhoanTietKiem::RutTien(long long soTien, std::string& thongBao) {
    if (!KiemTraSoTienRut(soTien, thongBao)) {
        return false;
    }
    if (soTien > HAN_MUC_RUT_TIET_KIEM) {
        thongBao = "Tai khoan tiet kiem chi duoc rut toi da "
                   + TienIch::DinhDangTien(HAN_MUC_RUT_TIET_KIEM) + " VND moi lan.";
        return false;
    }
    if (LaySoDu() - soTien < SO_DU_TOI_THIEU_TIET_KIEM) {
        long long toiDaRutDuoc = LaySoDu() - SO_DU_TOI_THIEU_TIET_KIEM;
        if (toiDaRutDuoc < 0) {
            toiDaRutDuoc = 0;
        }
        thongBao = "Phai giu lai toi thieu " + TienIch::DinhDangTien(SO_DU_TOI_THIEU_TIET_KIEM)
                   + " VND. So tien co the rut toi da: " + TienIch::DinhDangTien(toiDaRutDuoc) + " VND.";
        return false;
    }
    DatSoDu(LaySoDu() - soTien);
    thongBao = "Rut tien thanh cong.";
    return true;
}


void TaiKhoanTietKiem::HienThiThongTin() const {
    TaiKhoan::HienThiThongTin();
    const long long tienLaiNam = TinhTienLaiNam();
    TienIch::InDong("Lai suat", TienIch::DinhDangPhanTram(m_LaiSuat) + " / nam", MAU_VANG);
    TienIch::InDong("Tien lai du kien (1 thang)", TienIch::DinhDangTien(TinhTienLaiThang()) + " VND", MAU_XANH_LA);
    TienIch::InDong("Tien lai du kien (1 nam)", TienIch::DinhDangTien(tienLaiNam) + " VND", MAU_XANH_LA);
    TienIch::InDong("So du sau 1 nam (lai don)", TienIch::DinhDangTien(LaySoDu() + tienLaiNam) + " VND", MAU_XANH_LA);
    TienIch::InDong("So du toi thieu phai giu", TienIch::DinhDangTien(SO_DU_TOI_THIEU_TIET_KIEM) + " VND");
    TienIch::InDong("Han muc rut moi lan", TienIch::DinhDangTien(HAN_MUC_RUT_TIET_KIEM) + " VND");
}

std::string TaiKhoanTietKiem::LayLoaiTaiKhoan() const {
    return "Tiet kiem";
}

std::string TaiKhoanTietKiem::LayThongSoRieng() const {
    return "Lai suat " + TienIch::DinhDangPhanTram(m_LaiSuat) + "/nam";
}

std::string TaiKhoanTietKiem::LayQuyDinhRut() const {
    return "Giu lai toi thieu " + TienIch::DinhDangTien(SO_DU_TOI_THIEU_TIET_KIEM)
           + " VND; toi da " + TienIch::DinhDangTien(HAN_MUC_RUT_TIET_KIEM)
           + " VND/lan; boi so cua " + TienIch::DinhDangTien(BUOC_TIEN) + " VND.";
}


std::string TaiKhoanTietKiem::LayDongFile() const {
    std::ostringstream laiSuat;
    laiSuat << std::setprecision(10) << m_LaiSuat;
    return std::string(LOAI_FILE_TIET_KIEM) + KY_TU_NGAN_CACH + LayDongChung()
           + KY_TU_NGAN_CACH + laiSuat.str();
}


void TaiKhoanTietKiem::Nhap(std::istream& is) {
    TaiKhoan::Nhap(is);
    if (!is) {
        return;
    }
    double laiSuat = 0.0;
    if (!TienIch::NhapSoThuc(is, "  Lai suat (%/nam, tu " + TienIch::DinhDangPhanTram(LAI_SUAT_TOI_THIEU)
                                 + " den " + TienIch::DinhDangPhanTram(LAI_SUAT_TOI_DA) + "): ",
                             laiSuat, LAI_SUAT_TOI_THIEU, LAI_SUAT_TOI_DA)) {
        is.setstate(std::ios::failbit);
        return;
    }
    m_LaiSuat = laiSuat;
}
