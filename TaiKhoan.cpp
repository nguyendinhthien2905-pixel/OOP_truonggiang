#include "TaiKhoan.h"

#include <iomanip>

#include "HangSo.h"
#include "TienIch.h"

//  Constructor / Destructor

TaiKhoan::TaiKhoan()
    : m_SoDu(0) {
}

TaiKhoan::TaiKhoan(const std::string& soTaiKhoan, const std::string& tenChuTaiKhoan, long long soDu)
    : m_SoTaiKhoan(soTaiKhoan), m_TenChuTaiKhoan(tenChuTaiKhoan), m_SoDu(soDu) {
}

TaiKhoan::~TaiKhoan() {
}

//  Getter / Setter

const std::string& TaiKhoan::LaySoTaiKhoan() const {
    return m_SoTaiKhoan;
}

const std::string& TaiKhoan::LayTenChuTaiKhoan() const {
    return m_TenChuTaiKhoan;
}

long long TaiKhoan::LaySoDu() const {
    return m_SoDu;
}

void TaiKhoan::DatSoTaiKhoan(const std::string& soTaiKhoan) {
    m_SoTaiKhoan = soTaiKhoan;
}

void TaiKhoan::DatTenChuTaiKhoan(const std::string& tenChuTaiKhoan) {
    m_TenChuTaiKhoan = tenChuTaiKhoan;
}

void TaiKhoan::DatSoDu(long long soDu) {
    m_SoDu = soDu;
}


//Nap tien vao tai khoan
//  true neu thanh cong; thongBao luon chua noi dung ket qua

bool TaiKhoan::NapTien(long long soTien, std::string& thongBao) {
    if (soTien <= 0) {
        thongBao = "So tien nap phai lon hon 0.";
        return false;
    }
    if (m_SoDu + soTien > SO_DU_TOI_DA) {
        thongBao = "So du sau khi nap vuot gioi han cho phep ("
            + TienIch::DinhDangTien(SO_DU_TOI_DA) + " VND).";
        return false;
    }
    m_SoDu += soTien;
    thongBao = "Nap tien thanh cong.";
    return true;
}


bool TaiKhoan::KiemTraSoTienRut(long long soTien, std::string& thongBao) const {
    if (soTien <= 0) {
        thongBao = "So tien rut phai lon hon 0.";
        return false;
    }
    return true;
}


// Rut tien khong duoc rut qua so du

bool TaiKhoan::RutTien(long long soTien, std::string& thongBao) {
    if (!KiemTraSoTienRut(soTien, thongBao)) {
        return false;
    }
    if (soTien > m_SoDu) {
        thongBao = "So du khong du de thuc hien giao dich.";
        return false;
    }
    m_SoDu -= soTien;
    thongBao = "Rut tien thanh cong.";
    return true;
}


void TaiKhoan::HienThiThongTin() const {
    TienIch::InDong("So tai khoan", m_SoTaiKhoan);
    TienIch::InDong("Chu tai khoan", m_TenChuTaiKhoan);
    TienIch::InDong("Loai tai khoan", LayLoaiTaiKhoan(), MAU_VANG);
    TienIch::InDong("So du hien tai", TienIch::DinhDangTien(m_SoDu) + " VND",
        m_SoDu < 0 ? MAU_DO : MAU_XANH_LA);
}

std::string TaiKhoan::LayLoaiTaiKhoan() const {
    return "Tai khoan";
}

std::string TaiKhoan::LayThongSoRieng() const {
    return "-";
}

std::string TaiKhoan::LayQuyDinhRut() const {
    return "Khong duoc rut qua so du.";
}


std::string TaiKhoan::LayDongChung() const {
    return m_SoTaiKhoan + KY_TU_NGAN_CACH + m_TenChuTaiKhoan + KY_TU_NGAN_CACH + std::to_string(m_SoDu);
}

std::string TaiKhoan::LayDongFile() const {
    return std::string("TAIKHOAN") + KY_TU_NGAN_CACH + LayDongChung();
}


//  Nhap / Xuat

void TaiKhoan::Nhap(std::istream& is) {
    std::string soTaiKhoan;
    std::string ten;
    long long soDu = 0;

    if (!TienIch::NhapChuoiSo(is, "  So tai khoan (" + std::to_string(DO_DAI_SO_TAI_KHOAN)
        + " chu so): ", DO_DAI_SO_TAI_KHOAN, soTaiKhoan)
        || !TienIch::NhapChuoi(is, "  Ten chu tai khoan: ", ten, DO_DAI_TEN_TOI_DA)
        || !TienIch::NhapSoNguyen(is, "  So du ban dau (VND): ", soDu, 0, SO_DU_TOI_DA)) {
        is.setstate(std::ios::failbit);
        return;
    }
    m_SoTaiKhoan = soTaiKhoan;
    m_TenChuTaiKhoan = TienIch::ChuyenInHoa(ten);
    m_SoDu = soDu;
}


// | STK | Ten | Loai | So du | Thong so rieng |

void TaiKhoan::Xuat(std::ostream& os) const {
    const std::ios_base::fmtflags coCu = os.flags();
    os << "| " << std::left << std::setw(DO_RONG_COT_SO_TAI_KHOAN) << m_SoTaiKhoan
        << " | " << std::setw(DO_RONG_COT_TEN) << m_TenChuTaiKhoan
        << " | " << std::setw(DO_RONG_COT_LOAI) << LayLoaiTaiKhoan()
        << " | " << std::right << std::setw(DO_RONG_COT_SO_DU) << TienIch::DinhDangTien(m_SoDu)
        << " | " << std::left << std::setw(DO_RONG_COT_THONG_SO) << LayThongSoRieng()
        << " |";
    os.flags(coCu);
}

std::istream& operator>>(std::istream& is, TaiKhoan& taiKhoan) {
    taiKhoan.Nhap(is);     
    return is;
}

std::ostream& operator<<(std::ostream& os, const TaiKhoan& taiKhoan) {
    taiKhoan.Xuat(os);
    return os;
}

// 
//  Ke bang

void TaiKhoan::InDuongKeBang(std::ostream& os) {
    const std::ios_base::fmtflags coCu = os.flags();
    const char kyTuCu = os.fill('-');
    os << '+' << std::setw(DO_RONG_COT_STT + 2) << ""
        << '+' << std::setw(DO_RONG_COT_SO_TAI_KHOAN + 2) << ""
        << '+' << std::setw(DO_RONG_COT_TEN + 2) << ""
        << '+' << std::setw(DO_RONG_COT_LOAI + 2) << ""
        << '+' << std::setw(DO_RONG_COT_SO_DU + 2) << ""
        << '+' << std::setw(DO_RONG_COT_THONG_SO + 2) << ""
        << "+\n";
    os.fill(kyTuCu);
    os.flags(coCu);
}

void TaiKhoan::InTieuDeBang(std::ostream& os) {
    const std::ios_base::fmtflags coCu = os.flags();
    os << std::left
        << "| " << std::setw(DO_RONG_COT_STT) << "STT"
        << " | " << std::setw(DO_RONG_COT_SO_TAI_KHOAN) << "So tai khoan"
        << " | " << std::setw(DO_RONG_COT_TEN) << "Chu tai khoan"
        << " | " << std::setw(DO_RONG_COT_LOAI) << "Loai"
        << " | " << std::setw(DO_RONG_COT_SO_DU) << "So du (VND)"
        << " | " << std::setw(DO_RONG_COT_THONG_SO) << "Thong so rieng"
        << " |\n";
    os.flags(coCu);
}