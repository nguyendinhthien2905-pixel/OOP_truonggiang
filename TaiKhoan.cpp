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
        thongBao = "Số tiền nạp phải lớn hơn 0.";
        return false;
    }
    if (m_SoDu + soTien > SO_DU_TOI_DA) {
        thongBao = "Số dư sau khi nạp vượt giới hạn cho phép ("
            + TienIch::DinhDangTien(SO_DU_TOI_DA) + " VND).";
        return false;
    }
    m_SoDu += soTien;
    thongBao = "Nạp tiền thành công.";
    return true;
}


bool TaiKhoan::KiemTraSoTienRut(long long soTien, std::string& thongBao) const {
    if (soTien <= 0) {
        thongBao = "Số tiền rút phải lớn hơn 0.";
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
        thongBao = "Số dư không đủ để thực hiện giao dịch.";
        return false;
    }
    m_SoDu -= soTien;
    thongBao = "Rút tiền thành công.";
    return true;
}


void TaiKhoan::HienThiThongTin() const {
    TienIch::InDong("Số tài khoản", m_SoTaiKhoan);
    TienIch::InDong("Chủ tài khoản", m_TenChuTaiKhoan);
    TienIch::InDong("Loại tài khoản", LayLoaiTaiKhoan(), MAU_VANG);
    TienIch::InNganCach();
    TienIch::InDong("Số dư hiện tại", TienIch::DinhDangTien(m_SoDu) + " VND",
        m_SoDu < 0 ? MAU_DO : MAU_XANH_LA);
}

void TaiKhoan::HienThiHanMucRut() const {
}

std::string TaiKhoan::LayLoaiTaiKhoan() const {
    return "Tài khoản";
}

std::string TaiKhoan::LayThongSoRieng() const {
    return "-";
}

std::string TaiKhoan::LayQuyDinhRut() const {
    return "Không được rút quá số dư.";
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

    if (!TienIch::NhapChuoiSo(is, "  Số tài khoản (" + std::to_string(DO_DAI_SO_TAI_KHOAN)
        + " chữ số): ", DO_DAI_SO_TAI_KHOAN, soTaiKhoan)
        || !TienIch::NhapChuoi(is, "  Tên chủ tài khoản (không dấu): ", ten, DO_DAI_TEN_TOI_DA)
        || !TienIch::NhapSoNguyen(is, "  Số dư ban đầu (VND): ", soDu, 0, SO_DU_TOI_DA)) {
        is.setstate(std::ios::failbit);
        return;
    }
    m_SoTaiKhoan = soTaiKhoan;
    m_TenChuTaiKhoan = TienIch::ChuyenInHoa(ten);
    m_SoDu = soDu;
}


// | STK | Ten | Loai | So du | Thong so rieng |

void TaiKhoan::Xuat(std::ostream& os) const {
    const std::string ke = " " + TienIch::KeDoc() + " ";
    os << TienIch::KeDoc() << " " << TienIch::CanTrai(m_SoTaiKhoan, DO_RONG_COT_SO_TAI_KHOAN)
        << ke << TienIch::CanTrai(m_TenChuTaiKhoan, DO_RONG_COT_TEN)
        << ke << TienIch::CanTrai(LayLoaiTaiKhoan(), DO_RONG_COT_LOAI)
        << ke << TienIch::CanPhai(TienIch::DinhDangTien(m_SoDu), DO_RONG_COT_SO_DU)
        << ke << TienIch::CanTrai(LayThongSoRieng(), DO_RONG_COT_THONG_SO)
        << " " << TienIch::KeDoc();
}

std::istream& operator>>(std::istream& is, TaiKhoan& taiKhoan) {
    taiKhoan.Nhap(is);     
    return is;
}

std::ostream& operator<<(std::ostream& os, const TaiKhoan& taiKhoan) {
    taiKhoan.Xuat(os);
    return os;
}

//  Ke bang

void TaiKhoan::InDuongKeBang(std::ostream& os, int loai) {
    os.flush();
    std::vector<int> cot;
    cot.push_back(DO_RONG_COT_STT);
    cot.push_back(DO_RONG_COT_SO_TAI_KHOAN);
    cot.push_back(DO_RONG_COT_TEN);
    cot.push_back(DO_RONG_COT_LOAI);
    cot.push_back(DO_RONG_COT_SO_DU);
    cot.push_back(DO_RONG_COT_THONG_SO);
    TienIch::InKeNgangBang(cot, loai);
}

void TaiKhoan::InTieuDeBang(std::ostream& os) {
    const std::string ke = " " + TienIch::KeDoc() + " ";
    os << TienIch::LeTrai(DO_RONG_BANG) << TienIch::KeDoc() << " " << TienIch::CanTrai("STT", DO_RONG_COT_STT)
        << ke << TienIch::CanTrai("Số tài khoản", DO_RONG_COT_SO_TAI_KHOAN)
        << ke << TienIch::CanTrai("Chủ tài khoản", DO_RONG_COT_TEN)
        << ke << TienIch::CanTrai("Loại", DO_RONG_COT_LOAI)
        << ke << TienIch::CanPhai("Số dư (VND)", DO_RONG_COT_SO_DU)
        << ke << TienIch::CanTrai("Thông số riêng", DO_RONG_COT_THONG_SO)
        << " " << TienIch::KeDoc() << "\n";
}