
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "TienIch.h"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include <conio.h>
#include <windows.h>

#include "HangSo.h"

namespace {

// Hang so noi bo 
const int DO_RONG_KHUNG = 60;           // Do rong khung tieu de
const int DO_RONG_MUC_MENU = 56;        // Do rong mot dong menu
const int SO_MUC_HIEN_THI_TOI_DA = 10;  // Menu dai hon se tu cuon
const int DO_RONG_NHAN = 28;            // Do rong cot nhan trong InDong
const int SO_CHU_SO_TOI_DA = 15;
const int NHOM_HANG_NGAN = 3;

const int MA_PHIM_ENTER = 13;
const int MA_PHIM_ESC = 27;
const int MA_PHIM_MO_RONG_1 = 0;        // Phim dieu huong tra ve 2 ma:
const int MA_PHIM_MO_RONG_2 = 224;      // (0 hoac 224) roi den ma that
const int MA_MUI_TEN_LEN = 72;
const int MA_MUI_TEN_XUONG = 80;

WORD g_mauMacDinh = 7;                  // Thuoc tinh mau luc chuong trinh bat dau

HANDLE LayManHinh() {
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

// Luu va khoi phuc dinh dang cua ostream (left/right, setw, setfill, ...)
class BaoVeDinhDang {
private:
    std::ostream& m_os;
    std::ios_base::fmtflags m_coDinhDang;
    char m_kyTuDien;

public:
    explicit BaoVeDinhDang(std::ostream& os)
        : m_os(os), m_coDinhDang(os.flags()), m_kyTuDien(os.fill()) {
    }
    ~BaoVeDinhDang() {
        m_os.flags(m_coDinhDang);
        m_os.fill(m_kyTuDien);
    }
};

bool ChiGomChuSo(const std::string& chuoi) {
    if (chuoi.empty()) {
        return false;
    }
    for (std::string::size_type i = 0; i < chuoi.size(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(chuoi[i]))) {
            return false;
        }
    }
    return true;
}

// Doc mot dong. Dong rong hoac het du lieu => false (nguoi dung huy)
bool DocDong(std::istream& is, const std::string& loiNhac, std::string& dong) {
    std::cout << loiNhac << std::flush;
    if (!std::getline(is, dong)) {
        return false;
    }
    dong = TienIch::CatKhoangTrang(dong);
    return !dong.empty();
}

bool DocSoThuc(std::string chuoi, double& ketQua) {
    for (std::string::size_type i = 0; i < chuoi.size(); i++) {
        if (chuoi[i] == ',') {
            chuoi[i] = '.';
        }
    }
    int soDauCham = 0;
    int soChuSo = 0;
    for (std::string::size_type i = 0; i < chuoi.size(); i++) {
        if (chuoi[i] == '.') {
            soDauCham++;
        } else if (std::isdigit(static_cast<unsigned char>(chuoi[i]))) {
            soChuSo++;
        } else {
            return false;
        }
    }
    if (soDauCham > 1 || soChuSo == 0 || chuoi.size() > 10) {
        return false;
    }
    ketQua = std::strtod(chuoi.c_str(), NULL);
    return true;
}

std::string SoThucSangChuoi(double giaTri) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << giaTri;
    return ss.str();
}

// Ve toan bo menu tai goc tren-trai man hinh (ghi de, khong xoa -> khong nhap nhay)
void VeMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc,
            int viTri, int dau, int soDong) {
    BaoVeDinhDang baoVe(std::cout);
    const int soMuc = static_cast<int>(cacMuc.size());

    TienIch::DatViTriCon(0, 0);
    TienIch::InTieuDe(tieuDe);

    std::cout << std::left;
    TienIch::DatMau(MAU_XAM);
    std::cout << std::setw(DO_RONG_KHUNG)
              << "  [Len/Xuong] Di chuyen    [Enter] Chon    [Esc] Quay lai" << "\n";
    std::cout << std::setw(DO_RONG_KHUNG) << "" << "\n";

    TienIch::DatMau(MAU_XANH_NGOC);
    std::cout << std::setw(DO_RONG_MUC_MENU) << (dau > 0 ? "        ^ ..." : "") << "\n";

    for (int i = dau; i < dau + soDong; i++) {
        std::string noiDung = cacMuc[i];
        if (static_cast<int>(noiDung.size()) > DO_RONG_MUC_MENU - 4) {
            noiDung = noiDung.substr(0, DO_RONG_MUC_MENU - 4);
        }
        if (i == viTri) {
            std::cout << "  ";
            TienIch::DatMauNen(MAU_DEN, MAU_XANH_NGOC);
            std::cout << std::setw(DO_RONG_MUC_MENU - 2) << (" >> " + noiDung);
            TienIch::DatMauMacDinh();
        } else {
            TienIch::DatMau(MAU_TRANG);
            std::cout << std::setw(DO_RONG_MUC_MENU) << ("    " + noiDung);
        }
        TienIch::DatMauMacDinh();
        std::cout << "\n";
    }

    TienIch::DatMau(MAU_XANH_NGOC);
    std::cout << std::setw(DO_RONG_MUC_MENU) << (dau + soDong < soMuc ? "        v ..." : "") << "\n";
    TienIch::DatMauMacDinh();
    std::cout.flush();
}

} // namespace

namespace TienIch {

    void KhoiTaoConsole() {
        std::system("mode con: cols=110 lines=36");
        SetConsoleTitleA("QUAN LY TAI KHOAN NGAN HANG - MAY ATM");
        CONSOLE_SCREEN_BUFFER_INFO thongTin;
        if (GetConsoleScreenBufferInfo(LayManHinh(), &thongTin)) {
            g_mauMacDinh = thongTin.wAttributes;
        }
    }

    void DatMau(Mau mauChu) {
        std::cout.flush();
        SetConsoleTextAttribute(LayManHinh(),
            static_cast<WORD>(mauChu | (g_mauMacDinh & 0xF0)));
    }


    void DatMauNen(Mau mauChu, Mau mauNen) {
        std::cout.flush();
        SetConsoleTextAttribute(LayManHinh(), static_cast<WORD>(mauChu | (mauNen << 4)));
    }


    void DatMauMacDinh() {
        std::cout.flush();
        SetConsoleTextAttribute(LayManHinh(), g_mauMacDinh);
    }


    void XoaManHinh() {
        std::cout.flush();
        HANDLE hManHinh = LayManHinh();
        CONSOLE_SCREEN_BUFFER_INFO thongTin;
        if (!GetConsoleScreenBufferInfo(hManHinh, &thongTin)) {
            std::system("cls");
            return;
        }
        const DWORD soO = static_cast<DWORD>(thongTin.dwSize.X) * thongTin.dwSize.Y;
        DWORD soDaGhi = 0;
        COORD goc;
        goc.X = 0;
        goc.Y = 0;
        FillConsoleOutputCharacterA(hManHinh, ' ', soO, goc, &soDaGhi);
        FillConsoleOutputAttribute(hManHinh, g_mauMacDinh, soO, goc, &soDaGhi);
        SetConsoleCursorPosition(hManHinh, goc);
    }


    void DatViTriCon(int x, int y) {
        std::cout.flush();
        COORD viTri;
        viTri.X = static_cast<SHORT>(x);
        viTri.Y = static_cast<SHORT>(y);
        SetConsoleCursorPosition(LayManHinh(), viTri);
    }


    void HienConTro(bool hien) {
        CONSOLE_CURSOR_INFO thongTin;
        if (GetConsoleCursorInfo(LayManHinh(), &thongTin)) {
            thongTin.bVisible = hien ? TRUE : FALSE;
            SetConsoleCursorInfo(LayManHinh(), &thongTin);
        }
    }


    //  PHIM VA MENU

    Phim DocPhim() {
        const int maPhim = _getch();
        if (maPhim == MA_PHIM_MO_RONG_1 || maPhim == MA_PHIM_MO_RONG_2) {
            const int maThat = _getch();
            if (maThat == MA_MUI_TEN_LEN) {
                return PHIM_LEN;
            }
            if (maThat == MA_MUI_TEN_XUONG) {
                return PHIM_XUONG;
            }
            return PHIM_KHAC;
        }
        if (maPhim == MA_PHIM_ENTER) {
            return PHIM_ENTER;
        }
        if (maPhim == MA_PHIM_ESC) {
            return PHIM_ESC;
        }
        return PHIM_KHAC;
    }

    \
        void TamDung() {
        DatMau(MAU_VANG);
        std::cout << "\n  Nhan phim bat ky de tiep tuc...";
        DatMauMacDinh();
        std::cout.flush();
        DocPhim();
    }

    \
        int ChonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc) {
        const int soMuc = static_cast<int>(cacMuc.size());
        if (soMuc == 0) {
            return KHONG_CHON;
        }
        const int soDong = (soMuc < SO_MUC_HIEN_THI_TOI_DA) ? soMuc : SO_MUC_HIEN_THI_TOI_DA;

        int viTri = 0;
        int dau = 0;
        int ketQua = KHONG_CHON;
        bool daXong = false;

        HienConTro(false);
        XoaManHinh();
        while (!daXong) {
            if (viTri < dau) {
                dau = viTri;
            }
            if (viTri >= dau + soDong) {
                dau = viTri - soDong + 1;
            }
            VeMenu(tieuDe, cacMuc, viTri, dau, soDong);

            switch (DocPhim()) {
            case PHIM_LEN:
                viTri = (viTri + soMuc - 1) % soMuc;
                break;
            case PHIM_XUONG:
                viTri = (viTri + 1) % soMuc;
                break;
            case PHIM_ENTER:
                ketQua = viTri;
                daXong = true;
                break;
            case PHIM_ESC:
                daXong = true;
                break;
            default:
                break;
            }
        }
        HienConTro(true);
        DatMauMacDinh();
        XoaManHinh();
        return ketQua;
    }


    // In khung tieu de, chu nam giua
    void InTieuDe(const std::string& tieuDe) {
        const std::string vien = "+" + std::string(DO_RONG_KHUNG - 2, '=') + "+";
        int khoangTrong = DO_RONG_KHUNG - 2 - static_cast<int>(tieuDe.size());
        if (khoangTrong < 0) {
            khoangTrong = 0;
        }
        const int leTrai = khoangTrong / 2;
        const int lePhai = khoangTrong - leTrai;

        DatMau(MAU_XANH_NGOC);
        std::cout << vien << "\n|";
        DatMau(MAU_VANG);
        std::cout << std::string(leTrai, ' ') << tieuDe << std::string(lePhai, ' ');
        DatMau(MAU_XANH_NGOC);
        std::cout << "|\n" << vien << "\n";
        DatMauMacDinh();
    }


    void InDong(const std::string& nhan, const std::string& giaTri, Mau mauGiaTri) {
        BaoVeDinhDang baoVe(std::cout);
        DatMau(MAU_XANH_NGOC);
        std::cout << "  " << std::left << std::setw(DO_RONG_NHAN) << nhan << ": ";
        DatMau(mauGiaTri);
        std::cout << giaTri;
        DatMauMacDinh();
        std::cout << "\n";
    }

    void BaoLoi(const std::string& thongBao) {
        DatMau(MAU_DO);
        std::cout << "  [LOI] " << thongBao << "\n";
        DatMauMacDinh();
    }

    void BaoThanhCong(const std::string& thongBao) {
        DatMau(MAU_XANH_LA);
        std::cout << "  [OK] " << thongBao << "\n";
        DatMauMacDinh();
    }

    void BaoThongTin(const std::string& thongBao) {
        DatMau(MAU_VANG);
        std::cout << "  [!] " << thongBao << "\n";
        DatMauMacDinh();
    }

    //  NHAP LIEU
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa) {
        std::string dong;
        while (DocDong(is, loiNhac, dong)) {
            if (static_cast<int>(dong.size()) > doDaiToiDa) {
                BaoLoi("Toi da " + std::to_string(doDaiToiDa) + " ky tu.");
                continue;
            }
            if (dong.find(KY_TU_NGAN_CACH) != std::string::npos) {      // Tranh lam hong file du lieu
                BaoLoi(std::string("Khong duoc dung ky tu '") + KY_TU_NGAN_CACH + "'.");
                continue;
            }
            ketQua = dong;
            return true;
        }
        return false;
    }


    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua) {
        std::string dong;
        while (DocDong(is, loiNhac, dong)) {
            if (!ChiGomChuSo(dong) || static_cast<int>(dong.size()) != doDai) {
                BaoLoi("Phai gom dung " + std::to_string(doDai) + " chu so.");
                continue;
            }
            ketQua = dong;
            return true;
        }
        return false;
    }

    bool NhapSoNguyen(std::istream& is, const std::string& loiNhac, long long& ketQua,
        long long giaTriNhoNhat, long long giaTriLonNhat) {
        std::string dong;
        while (DocDong(is, loiNhac, dong)) {
            if (!ChiGomChuSo(dong) || static_cast<int>(dong.size()) > SO_CHU_SO_TOI_DA) {
                BaoLoi("Chi duoc nhap so nguyen khong am (toi da 15 chu so).");
                continue;
            }
            const long long giaTri = std::stoll(dong);
            if (giaTri < giaTriNhoNhat || giaTri > giaTriLonNhat) {
                BaoLoi("Gia tri phai tu " + DinhDangTien(giaTriNhoNhat)
                    + " den " + DinhDangTien(giaTriLonNhat) + ".");
                continue;
            }
            ketQua = giaTri;
            return true;
        }
        return false;
    }


    bool NhapSoThuc(std::istream& is, const std::string& loiNhac, double& ketQua,
        double giaTriNhoNhat, double giaTriLonNhat) {
        std::string dong;
        while (DocDong(is, loiNhac, dong)) {
            double giaTri = 0.0;
            if (!DocSoThuc(dong, giaTri)) {
                BaoLoi("Chi duoc nhap so (vi du 5.5).");
                continue;
            }
            if (giaTri < giaTriNhoNhat || giaTri > giaTriLonNhat) {
                BaoLoi("Gia tri phai tu " + SoThucSangChuoi(giaTriNhoNhat)
                    + " den " + SoThucSangChuoi(giaTriLonNhat) + ".");
                continue;
            }
            ketQua = giaTri;
            return true;
        }
        return false;
    }

    //  DINH DANG

    std::string DinhDangTien(long long soTien) {
        const bool laSoAm = soTien < 0;
        const unsigned long long triTuyetDoi = laSoAm
            ? 0ULL - static_cast<unsigned long long>(soTien)
            : static_cast<unsigned long long>(soTien);
        const std::string cacChuSo = std::to_string(triTuyetDoi);
        const int doDai = static_cast<int>(cacChuSo.size());

        std::string ketQua;
        for (int i = 0; i < doDai; i++) {
            ketQua += cacChuSo[i];
            const int conLai = doDai - 1 - i;
            if (conLai > 0 && conLai % NHOM_HANG_NGAN == 0) {
                ketQua += '.';
            }
        }
        return laSoAm ? "-" + ketQua : ketQua;
    }


    std::string DinhDangPhanTram(double giaTri) {
        return SoThucSangChuoi(giaTri) + "%";
    }


    std::string CatKhoangTrang(const std::string& chuoi) {
        const std::string khoangTrang = " \t\r\n";
        const std::string::size_type dau = chuoi.find_first_not_of(khoangTrang);
        if (dau == std::string::npos) {
            return "";
        }
        const std::string::size_type cuoi = chuoi.find_last_not_of(khoangTrang);
        return chuoi.substr(dau, cuoi - dau + 1);
    }


    std::string ChuyenInHoa(const std::string& chuoi) {
        std::string ketQua = chuoi;
        for (std::string::size_type i = 0; i < ketQua.size(); i++) {
            ketQua[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(ketQua[i])));
        }
        return ketQua;
    }

}
