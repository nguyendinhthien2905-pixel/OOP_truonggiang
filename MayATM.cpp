// =====================================================================
//  MayATM.cpp - Menu chinh va cac chuc nang cua may ATM
// =====================================================================
#include "MayATM.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

#include "HangSo.h"
#include "KhoFile.h"
#include "TaiKhoanThanhToan.h"
#include "TaiKhoanTietKiem.h"
#include "TienIch.h"

namespace {

enum LuaChonMenuChinh {
    MENU_XEM_DANH_SACH = 0,
    MENU_XEM_CHI_TIET,
    MENU_THEM_TAI_KHOAN,
    MENU_NAP_TIEN,
    MENU_RUT_TIEN,
    MENU_THOAT
};

enum LuaChonLoaiTaiKhoan {
    LOAI_TIET_KIEM = 0,
    LOAI_THANH_TOAN
};

} // namespace


/**********************************************************
 * @Description Nap danh sach tai khoan tu file TaiKhoan.txt
 * @attention   - Chua co file: tao file voi du lieu mau
 *              - Dong sai trong file: bao loi va bo qua dong do
 **********************************************************/
void MayATM::TaiDuLieu() {
    std::vector<std::string> cacLoi;
    if (!KhoFile::Doc(TEN_FILE_DU_LIEU, m_DanhSach, cacLoi)) {
        
        if (KhoFile::Ghi(TEN_FILE_DU_LIEU, m_DanhSach)) {
            TienIch::BaoThongTin(std::string("Chua co file ") + TEN_FILE_DU_LIEU
                                 + ". Da tao file voi " + std::to_string(m_DanhSach.size())
                                 + " tai khoan mau.");
        } else {
            TienIch::BaoLoi(std::string("Khong the tao file ") + TEN_FILE_DU_LIEU
                            + ". Du lieu se khong duoc luu.");
        }
        TienIch::TamDung();
        return;
    }

    if (!cacLoi.empty()) {
        TienIch::BaoLoi(std::string("File ") + TEN_FILE_DU_LIEU + " co dong khong hop le:");
        for (std::size_t i = 0; i < cacLoi.size(); i++) {
            std::cout << "      " << cacLoi[i] << "\n";
        }
        std::cout << "\n";
        TienIch::BaoThongTin("Da doc " + std::to_string(m_DanhSach.size())
                             + " tai khoan. Cac dong loi se bi xoa khi chuong trinh luu lai file.");
        TienIch::TamDung();
    }
}

/**********************************************************
 * @Description Ghi danh sach hien tai ra file
 **********************************************************/
void MayATM::LuuFile() const {
    if (!KhoFile::Ghi(TEN_FILE_DU_LIEU, m_DanhSach)) {
        TienIch::BaoLoi(std::string("Khong the ghi du lieu ra file ") + TEN_FILE_DU_LIEU + ".");
    }
}

/**********************************************************
 * @Description Tim tai khoan theo so tai khoan
 * @return      Con tro toi tai khoan, nullptr neu khong co
 **********************************************************/
TaiKhoan* MayATM::TimTaiKhoan(const std::string& soTaiKhoan) const {
    for (std::size_t i = 0; i < m_DanhSach.size(); i++) {
        if (m_DanhSach[i]->LaySoTaiKhoan() == soTaiKhoan) {
            return m_DanhSach[i].get();
        }
    }
    return nullptr;
}

/**********************************************************
 * @Description Hien menu mui ten liet ke tai khoan de nguoi dung chon
 * @return      Con tro toi tai khoan da chon, nullptr neu quay lai / khong co
 **********************************************************/
TaiKhoan* MayATM::ChonTaiKhoan(const std::string& tieuDe) const {
    if (m_DanhSach.empty()) {
        TienIch::XoaManHinh();
        TienIch::InTieuDe(tieuDe);
        TienIch::BaoThongTin("Chua co tai khoan nao trong he thong.");
        TienIch::TamDung();
        return nullptr;
    }

    std::vector<std::string> cacMuc;
    for (std::size_t i = 0; i < m_DanhSach.size(); i++) {
        std::ostringstream dong;
        dong << std::left
             << std::setw(DO_DAI_SO_TAI_KHOAN) << m_DanhSach[i]->LaySoTaiKhoan() << " | "
             << std::setw(DO_DAI_TEN_TOI_DA) << m_DanhSach[i]->LayTenChuTaiKhoan() << " | "
             << m_DanhSach[i]->LayLoaiTaiKhoan();
        cacMuc.push_back(dong.str());
    }
    cacMuc.push_back("<< Quay lai");

    const int viTri = TienIch::ChonMenu(tieuDe, cacMuc);
    if (viTri < 0 || viTri >= static_cast<int>(m_DanhSach.size())) {
        return nullptr;
    }
    return m_DanhSach[viTri].get();
}

/**********************************************************
 * @Description In bang danh sach tat ca tai khoan
 **********************************************************/
void MayATM::XemDanhSach() const {
    TienIch::XoaManHinh();
    TienIch::InTieuDe("DANH SACH TAI KHOAN");
    if (m_DanhSach.empty()) {
        TienIch::BaoThongTin("Chua co tai khoan nao trong he thong.");
        TienIch::TamDung();
        return;
    }

    long long tongSoDu = 0;
    std::cout << "\n";
    TienIch::DatMau(MAU_XANH_NGOC);
    TaiKhoan::InDuongKeBang(std::cout);
    TaiKhoan::InTieuDeBang(std::cout);
    TaiKhoan::InDuongKeBang(std::cout);
    TienIch::DatMauMacDinh();

    for (std::size_t i = 0; i < m_DanhSach.size(); i++) {
        // Phan "| STT " in o day, phan con lai do operator<< cua tai khoan in
        std::cout << "| " << std::right << std::setw(DO_RONG_COT_STT) << (i + 1) << " "
                  << *m_DanhSach[i] << "\n";
        tongSoDu += m_DanhSach[i]->LaySoDu();
    }

    TienIch::DatMau(MAU_XANH_NGOC);
    TaiKhoan::InDuongKeBang(std::cout);
    TienIch::DatMauMacDinh();

    std::cout << "\n";
    TienIch::InDong("Tong so tai khoan", std::to_string(m_DanhSach.size()), MAU_VANG);
    TienIch::InDong("Tong so du", TienIch::DinhDangTien(tongSoDu) + " VND",
                    tongSoDu < 0 ? MAU_DO : MAU_XANH_LA);
    TienIch::TamDung();
}

/**********************************************************
 * @Description Xem chi tiet mot tai khoan (goi HienThiThongTin da hinh)
 **********************************************************/
void MayATM::XemChiTiet() const {
    TaiKhoan* taiKhoan = ChonTaiKhoan("XEM THONG TIN CHI TIET");
    if (taiKhoan == nullptr) {
        return;
    }
    TienIch::XoaManHinh();
    TienIch::InTieuDe("THONG TIN CHI TIET");
    std::cout << "\n";
    taiKhoan->HienThiThongTin();
    TienIch::TamDung();
}

/**********************************************************
 * @Description Them tai khoan moi: chon loai, nhap bang operator>>
 **********************************************************/
void MayATM::ThemTaiKhoan() {
    std::vector<std::string> cacLoai;
    cacLoai.push_back("Tai khoan tiet kiem");
    cacLoai.push_back("Tai khoan thanh toan");
    cacLoai.push_back("<< Quay lai");

    std::unique_ptr<TaiKhoan> taiKhoanMoi;
    switch (TienIch::ChonMenu("THEM TAI KHOAN MOI", cacLoai)) {
    case LOAI_TIET_KIEM:
        taiKhoanMoi.reset(new TaiKhoanTietKiem());
        break;
    case LOAI_THANH_TOAN:
        taiKhoanMoi.reset(new TaiKhoanThanhToan());
        break;
    default:
        return;
    }

    TienIch::XoaManHinh();
    TienIch::InTieuDe("NHAP THONG TIN TAI KHOAN");
    TienIch::BaoThongTin("De trong va nhan Enter bat ky luc nao de huy.");
    std::cout << "\n";

    // operator>> goi phuong thuc ao Nhap() => tu nhap dung thuoc tinh cua tung loai
    if (!(std::cin >> *taiKhoanMoi)) {
        std::cin.clear();
        TienIch::BaoThongTin("Da huy them tai khoan.");
        TienIch::TamDung();
        return;
    }

    while (TimTaiKhoan(taiKhoanMoi->LaySoTaiKhoan()) != nullptr) {
        TienIch::BaoLoi("So tai khoan nay da ton tai. Vui long nhap so khac.");
        std::string soMoi;
        if (!TienIch::NhapChuoiSo(std::cin, "  So tai khoan (" + std::to_string(DO_DAI_SO_TAI_KHOAN)
                                            + " chu so): ", DO_DAI_SO_TAI_KHOAN, soMoi)) {
            std::cin.clear();
            TienIch::BaoThongTin("Da huy them tai khoan.");
            TienIch::TamDung();
            return;
        }
        taiKhoanMoi->DatSoTaiKhoan(soMoi);
    }

    m_DanhSach.push_back(std::move(taiKhoanMoi));
    LuuFile();
    std::cout << "\n";
    TienIch::BaoThanhCong("Them tai khoan thanh cong.");
    std::cout << "\n";
    m_DanhSach.back()->HienThiThongTin();
    TienIch::TamDung();
}

/**********************************************************
 * @Description Nap tien vao tai khoan da chon
 **********************************************************/
void MayATM::NapTien() {
    TaiKhoan* taiKhoan = ChonTaiKhoan("NAP TIEN - CHON TAI KHOAN");
    if (taiKhoan == nullptr) {
        return;
    }
    TienIch::XoaManHinh();
    TienIch::InTieuDe("NAP TIEN");
    std::cout << "\n";
    taiKhoan->HienThiThongTin();
    std::cout << "\n";

    long long soTien = 0;
    if (!TienIch::NhapSoNguyen(std::cin, "  So tien can nap (VND, Enter de huy): ", soTien, 1, SO_DU_TOI_DA)) {
        std::cin.clear();
        TienIch::BaoThongTin("Da huy giao dich.");
        TienIch::TamDung();
        return;
    }

    std::string thongBao;
    std::cout << "\n";
    if (taiKhoan->NapTien(soTien, thongBao)) {
        LuuFile();
        TienIch::BaoThanhCong(thongBao);
        TienIch::InDong("So du moi", TienIch::DinhDangTien(taiKhoan->LaySoDu()) + " VND", MAU_XANH_LA);
    } else {
        TienIch::BaoLoi(thongBao);
    }
    TienIch::TamDung();
}

/**********************************************************
 * @Description Rut tien tu tai khoan da chon (da hinh: RutTien cua tung loai)
 **********************************************************/
void MayATM::RutTien() {
    TaiKhoan* taiKhoan = ChonTaiKhoan("RUT TIEN - CHON TAI KHOAN");
    if (taiKhoan == nullptr) {
        return;
    }
    TienIch::XoaManHinh();
    TienIch::InTieuDe("RUT TIEN");
    std::cout << "\n";
    taiKhoan->HienThiThongTin();
    std::cout << "\n";
    TienIch::BaoThongTin("Quy dinh: " + taiKhoan->LayQuyDinhRut());

    long long soTien = 0;
    if (!TienIch::NhapSoNguyen(std::cin, "  So tien can rut (VND, Enter de huy): ", soTien, 1, SO_DU_TOI_DA)) {
        std::cin.clear();
        TienIch::BaoThongTin("Da huy giao dich.");
        TienIch::TamDung();
        return;
    }

    std::string thongBao;
    std::cout << "\n";
    if (taiKhoan->RutTien(soTien, thongBao)) {      // Goi dung phien ban cua lop con
        LuuFile();
        TienIch::BaoThanhCong(thongBao);
        TienIch::InDong("So du moi", TienIch::DinhDangTien(taiKhoan->LaySoDu()) + " VND",
                        taiKhoan->LaySoDu() < 0 ? MAU_DO : MAU_XANH_LA);
    } else {
        TienIch::BaoLoi(thongBao);
    }
    TienIch::TamDung();
}

/**********************************************************
 * @Description Vong lap menu chinh, ket thuc khi chon "Thoat"
 **********************************************************/
void MayATM::Chay() {
    TaiDuLieu();

    std::vector<std::string> cacMuc;
    cacMuc.push_back("Xem danh sach tai khoan");
    cacMuc.push_back("Xem thong tin chi tiet");
    cacMuc.push_back("Them tai khoan moi");
    cacMuc.push_back("Nap tien");
    cacMuc.push_back("Rut tien");
    cacMuc.push_back("Thoat chuong trinh");

    bool daThoat = false;
    while (!daThoat) {
        switch (TienIch::ChonMenu("MAY ATM - QUAN LY TAI KHOAN", cacMuc)) {
        case MENU_XEM_DANH_SACH:
            XemDanhSach();
            break;
        case MENU_XEM_CHI_TIET:
            XemChiTiet();
            break;
        case MENU_THEM_TAI_KHOAN:
            ThemTaiKhoan();
            break;
        case MENU_NAP_TIEN:
            NapTien();
            break;
        case MENU_RUT_TIEN:
            RutTien();
            break;
        case MENU_THOAT:
            daThoat = true;
            break;
        default:        // Nhan Esc o menu chinh: o lai menu
            break;
        }
    }

    TienIch::XoaManHinh();
    TienIch::DatMau(MAU_XANH_NGOC);
    std::cout << "\n  Cam on ban da su dung dich vu ATM. Tam biet!\n\n";
    TienIch::DatMauMacDinh();
}
