// 
//  TaiKhoanTietKiem.cpp - Cai dat lop TaiKhoanTietKiem

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

// Tien lai don 1 thang = So du x Lai suat / 100 / 12

long long TaiKhoanTietKiem::TinhTienLaiThang() const {
    if (LaySoDu() <= 0) {
        return 0;
    }
    return static_cast<long long>(std::llround(LaySoDu() * m_LaiSuat / 100.0 / 12.0));
}

// Tien lai don 1 nam = So du x Lai suat / 100

long long TaiKhoanTietKiem::TinhTienLaiNam() const {
    if (LaySoDu() <= 0) {
        return 0;
    }
    return static_cast<long long>(std::llround(LaySoDu() * m_LaiSuat / 100.0));
}

/* Rut tien tiet kiem. Dieu kien rieng:
 *                - Toi da HAN_MUC_RUT_TIET_KIEM moi lan
 *                - Sau khi rut phai con >= SO_DU_TOI_THIEU_TIET_KIEM
 */
bool TaiKhoanTietKiem::RutTien(long long soTien, std::string& thongBao) {
    if (!KiemTraSoTienRut(soTien, thongBao)) {
        return false;
    }
    if (soTien > HAN_MUC_RUT_TIET_KIEM) {
        thongBao = "Tài khoản tiết kiệm chỉ được rút tối đa "
            + TienIch::DinhDangTien(HAN_MUC_RUT_TIET_KIEM) + " VND mỗi lần.";
        return false;
    }
    if (LaySoDu() - soTien < SO_DU_TOI_THIEU_TIET_KIEM) {
        long long toiDaRutDuoc = LaySoDu() - SO_DU_TOI_THIEU_TIET_KIEM;
        if (toiDaRutDuoc < 0) {
            toiDaRutDuoc = 0;
        }
        thongBao = "Phải giữ lại tối thiểu " + TienIch::DinhDangTien(SO_DU_TOI_THIEU_TIET_KIEM)
            + " VND. Số tiền có thể rút tối đa: " + TienIch::DinhDangTien(toiDaRutDuoc) + " VND.";
        return false;
    }
    DatSoDu(LaySoDu() - soTien);
    thongBao = "Rút tiền thành công.";
    return true;
}

// In thong tin chung + lai suat va tien lai du kien
void TaiKhoanTietKiem::HienThiThongTin() const {
    TaiKhoan::HienThiThongTin();            // nhom 1 + dong "So du hien tai"
    const long long tienLaiNam = TinhTienLaiNam();
    TienIch::InDong("Lãi suất", TienIch::DinhDangPhanTram(m_LaiSuat) + " / năm", MAU_VANG);
    TienIch::InDong("Lãi dự kiến (1 năm)", TienIch::DinhDangTien(tienLaiNam) + " VND", MAU_XANH_LA);
    TienIch::InDong("Số dư sau 1 năm", TienIch::DinhDangTien(LaySoDu() + tienLaiNam) + " VND", MAU_XANH_LA);
    TienIch::InNganCach();
    HienThiHanMucRut();                     // nhom 3: dung chung voi man hinh Rut tien
}

void TaiKhoanTietKiem::HienThiHanMucRut() const {
    TienIch::InDong("Số dư tối thiểu", TienIch::DinhDangTien(SO_DU_TOI_THIEU_TIET_KIEM) + " VND");
    TienIch::InDong("Hạn mức rút/lần", TienIch::DinhDangTien(HAN_MUC_RUT_TIET_KIEM) + " VND");
}

std::string TaiKhoanTietKiem::LayLoaiTaiKhoan() const {
    return "Tiết kiệm";
}

std::string TaiKhoanTietKiem::LayThongSoRieng() const {
    return "Lãi suất " + TienIch::DinhDangPhanTram(m_LaiSuat) + "/năm";
}

std::string TaiKhoanTietKiem::LayQuyDinhRut() const {
    return "Giữ lại tối thiểu " + TienIch::DinhDangTien(SO_DU_TOI_THIEU_TIET_KIEM)
        + " VND; tối đa " + TienIch::DinhDangTien(HAN_MUC_RUT_TIET_KIEM)
        + " VND/lần.";
}

// TIETKIEM|SoTaiKhoan|Ten|SoDu|LaiSuat

std::string TaiKhoanTietKiem::LayDongFile() const {
    std::ostringstream laiSuat;
    laiSuat << std::setprecision(10) << m_LaiSuat;
    return std::string(LOAI_FILE_TIET_KIEM) + KY_TU_NGAN_CACH + LayDongChung()
        + KY_TU_NGAN_CACH + laiSuat.str();
}

// Nhap thong tin chung roi nhap lai suat
void TaiKhoanTietKiem::Nhap(std::istream& is) {
    TaiKhoan::Nhap(is);
    if (!is) {
        return;
    }
    double laiSuat = 0.0;
    if (!TienIch::NhapSoThuc(is, "  Lãi suất (%/năm, từ " + TienIch::DinhDangPhanTram(LAI_SUAT_TOI_THIEU)
        + " đến " + TienIch::DinhDangPhanTram(LAI_SUAT_TOI_DA) + "): ",
        laiSuat, LAI_SUAT_TOI_THIEU, LAI_SUAT_TOI_DA)) {
        is.setstate(std::ios::failbit);
        return;
    }
    m_LaiSuat = laiSuat;
}