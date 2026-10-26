// =====================================================================
//  KhoFile.cpp - Cai dat doc / ghi file du lieu tai khoan
// =====================================================================
#include "KhoFile.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "HangSo.h"
#include "TaiKhoanThanhToan.h"
#include "TaiKhoanTietKiem.h"
#include "TienIch.h"

namespace {

const std::size_t SO_TRUONG = 5;                // LOAI|STK|TEN|SODU|THAM_SO
const int SO_CHU_SO_TOI_DA = 15;

std::vector<std::string> TachDong(const std::string& dong) {
    std::vector<std::string> cacTruong;
    std::string truong;
    for (std::string::size_type i = 0; i < dong.size(); i++) {
        if (dong[i] == KY_TU_NGAN_CACH) {
            cacTruong.push_back(TienIch::CatKhoangTrang(truong));
            truong.clear();
        } else {
            truong += dong[i];
        }
    }
    cacTruong.push_back(TienIch::CatKhoangTrang(truong));
    return cacTruong;
}

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

// So nguyen, cho phep dau '-' o dau (tai khoan thanh toan co the dang no)
bool DocSoNguyen(const std::string& chuoi, long long& ketQua) {
    const bool coDauAm = !chuoi.empty() && chuoi[0] == '-';
    const std::string cacChuSo = coDauAm ? chuoi.substr(1) : chuoi;
    if (!ChiGomChuSo(cacChuSo) || static_cast<int>(cacChuSo.size()) > SO_CHU_SO_TOI_DA) {
        return false;
    }
    ketQua = std::stoll(cacChuSo);
    if (coDauAm) {
        ketQua = -ketQua;
    }
    return true;
}

bool DocSoThuc(const std::string& chuoi, double& ketQua) {
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
    if (soDauCham > 1 || soChuSo == 0 || chuoi.size() > 12) {
        return false;
    }
    ketQua = std::strtod(chuoi.c_str(), NULL);
    return true;
}

// Kiem tra cac truong chung. Tra ve chuoi rong neu hop le, nguoc lai la ly do loi
std::string KiemTraTruongChung(const std::vector<std::string>& truong, long long& soDu) {
    if (!ChiGomChuSo(truong[1]) || static_cast<int>(truong[1].size()) != DO_DAI_SO_TAI_KHOAN) {
        return "so tai khoan phai gom " + std::to_string(DO_DAI_SO_TAI_KHOAN) + " chu so";
    }
    if (truong[2].empty() || static_cast<int>(truong[2].size()) > DO_DAI_TEN_TOI_DA) {
        return "ten chu tai khoan rong hoac dai qua " + std::to_string(DO_DAI_TEN_TOI_DA) + " ky tu";
    }
    if (!DocSoNguyen(truong[3], soDu) || soDu > SO_DU_TOI_DA) {
        return "so du khong hop le";
    }
    return "";
}

// Tao tai khoan tu cac truong da tach. Tra ve nullptr va dat loi neu du lieu sai
std::unique_ptr<TaiKhoan> TaoTaiKhoan(const std::vector<std::string>& truong, std::string& loi) {
    std::unique_ptr<TaiKhoan> ketQua;
    if (truong.size() != SO_TRUONG) {
        loi = "sai so truong (can " + std::to_string(SO_TRUONG) + ")";
        return ketQua;
    }

    long long soDu = 0;
    loi = KiemTraTruongChung(truong, soDu);
    if (!loi.empty()) {
        return ketQua;
    }

    if (truong[0] == LOAI_FILE_TIET_KIEM) {
        double laiSuat = 0.0;
        if (!DocSoThuc(truong[4], laiSuat) || laiSuat < LAI_SUAT_TOI_THIEU || laiSuat > LAI_SUAT_TOI_DA) {
            loi = "lai suat khong hop le";
        } else if (soDu < 0) {
            loi = "tai khoan tiet kiem khong duoc co so du am";
        } else {
            ketQua.reset(new TaiKhoanTietKiem(truong[1], truong[2], soDu, laiSuat));
        }
    } else if (truong[0] == LOAI_FILE_THANH_TOAN) {
        long long hanMuc = 0;
        if (!DocSoNguyen(truong[4], hanMuc) || hanMuc < 0 || hanMuc > HAN_MUC_TIN_DUNG_TOI_DA) {
            loi = "han muc tin dung khong hop le";
        } else if (soDu < -hanMuc) {
            loi = "so du am vuot qua han muc tin dung";
        } else {
            ketQua.reset(new TaiKhoanThanhToan(truong[1], truong[2], soDu, hanMuc));
        }
    } else {
        loi = "loai tai khoan '" + truong[0] + "' khong hop le (TIETKIEM hoac THANHTOAN)";
    }
    return ketQua;
}

bool DaTonTai(const std::vector<std::unique_ptr<TaiKhoan> >& danhSach, const std::string& soTaiKhoan) {
    for (std::size_t i = 0; i < danhSach.size(); i++) {
        if (danhSach[i]->LaySoTaiKhoan() == soTaiKhoan) {
            return true;
        }
    }
    return false;
}

}

namespace KhoFile {

    /**********************************************************
     * @Description Doc file van ban, moi dong la mot tai khoan
     * @return      false neu khong mo duoc file
     * @attention   Tu bo qua dau BOM UTF-8 (do Notepad them vao),
     *              dong trong va dong chu thich bat dau bang '#'
     **********************************************************/
    bool Doc(const std::string& duongDan,
        std::vector<std::unique_ptr<TaiKhoan> >& danhSach,
        std::vector<std::string>& cacLoi) {
        std::ifstream file(duongDan.c_str());
        if (!file.is_open()) {
            return false;
        }

        std::string dong;
        int dongHienTai = 0;
        while (std::getline(file, dong)) {
            dongHienTai++;
            dong = TienIch::CatKhoangTrang(dong);

            // Chi bo qua dong trong, khong kiem tra BOM hoac '#' nua
            if (dong.empty()) {
                continue;
            }

            std::string loi;
            std::unique_ptr<TaiKhoan> taiKhoan = TaoTaiKhoan(TachDong(dong), loi);
            if (taiKhoan && DaTonTai(danhSach, taiKhoan->LaySoTaiKhoan())) {
                loi = "trung so tai khoan " + taiKhoan->LaySoTaiKhoan();
                taiKhoan.reset();
            }
            if (taiKhoan) {
                danhSach.push_back(std::move(taiKhoan));
            }
            else {
                cacLoi.push_back("Dong " + std::to_string(dongHienTai) + ": " + loi);
            }
        }
        return true;
    }

    /**********************************************************
     * @Description Ghi danh sach ra file
     * @attention   Ghi vao file tam roi doi ten, de khong mat du lieu
     *              neu bi loi giua chung
     **********************************************************/
    bool Ghi(const std::string& duongDan,
        const std::vector<std::unique_ptr<TaiKhoan> >& danhSach) {
        const std::string duongDanTam = duongDan + ".tmp";
        {
            std::ofstream file(duongDanTam.c_str());
            if (!file.is_open()) {
                return false;
            }


            for (std::size_t i = 0; i < danhSach.size(); i++) {
                file << danhSach[i]->LayDongFile() << "\n";
            }
            file.flush();
            if (!file) {
                return false;
            }
        }
        std::remove(duongDan.c_str());
        return std::rename(duongDanTam.c_str(), duongDan.c_str()) == 0;
    } // namespace KhoFile
}