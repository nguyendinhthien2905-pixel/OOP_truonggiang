//  NganHang.cpp - Menu chinh va cac chuc nang cua may ATM
#include "NganHang.h"

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


/*@Description Nap danh sach tai khoan tu file TaiKhoan.txt
           - Dong sai trong file: bao loi va bo qua dong do
*/
void NganHang::TaiDuLieu() {
    std::vector<std::string> cacLoi;
    if (!KhoFile::Doc(TEN_FILE_DU_LIEU, m_DanhSach, cacLoi)) {

        if (KhoFile::Ghi(TEN_FILE_DU_LIEU, m_DanhSach)) {
            TienIch::BaoThongTin(std::string("Chưa có file ") + TEN_FILE_DU_LIEU
                                 + ". Đã tạo file với " + std::to_string(m_DanhSach.size())
                                 + " tài khoản mẫu.");
        } else {
            TienIch::BaoLoi(std::string("Không thể tạo file ") + TEN_FILE_DU_LIEU
                            + ". Dữ liệu sẽ không được lưu.");
        }
        TienIch::TamDung();
        return;
    }

    if (!cacLoi.empty()) {
        TienIch::BaoLoi(std::string("File ") + TEN_FILE_DU_LIEU + " có dòng không hợp lệ:");
        for (std::size_t i = 0; i < cacLoi.size(); i++) {
            std::cout << TienIch::LeTrai(TienIch::DO_RONG_KHUNG) << "    " << cacLoi[i] << "\n";
        }
        std::cout << "\n";
        TienIch::BaoThongTin("Đã đọc " + std::to_string(m_DanhSach.size())
                             + " tài khoản. Các dòng lỗi sẽ bị xóa khi chương trình lưu lại file.");
        TienIch::TamDung();
    }
}

//Ghi danh sach hien tai ra file
void NganHang::LuuFile() const {
    if (!KhoFile::Ghi(TEN_FILE_DU_LIEU, m_DanhSach)) {
        TienIch::BaoLoi(std::string("Không thể ghi dữ liệu ra file ") + TEN_FILE_DU_LIEU + ".");
    }
}

// Tim tai khoan theo so tai khoann
// Con tro toi tai khoan, nullptr neu khong co
TaiKhoan* NganHang::TimTaiKhoan(const std::string& soTaiKhoan) const {
    for (std::size_t i = 0; i < m_DanhSach.size(); i++) {
        if (m_DanhSach[i]->LaySoTaiKhoan() == soTaiKhoan) {
            return m_DanhSach[i].get();
        }
    }
    return nullptr;
}

// Hien menu mui ten liet ke tai khoan de nguoi dung chon
//Con tro toi tai khoan da chon, nullptr neu quay lai / khong co
TaiKhoan* NganHang::ChonTaiKhoan(const std::string& tieuDe) const {
    if (m_DanhSach.empty()) {
        TienIch::XoaManHinh();
        TienIch::InTieuDe(tieuDe);
        TienIch::BaoThongTin("Chưa có tài khoản nào trong hệ thống.");
        TienIch::TamDung();
        return nullptr;
    }

    const int COT_STK = DO_DAI_SO_TAI_KHOAN;
    const int COT_TEN = DO_DAI_TEN_TOI_DA;
    const int COT_LOAI = 10;
    const int COT_SO_DU = 16;
    const std::string cach = "  ";      // khoang cach giua 2 cot

    const std::string dongTieuDeCot = TienIch::CanTrai("Số TK", COT_STK) + cach
        + TienIch::CanTrai("Chủ tài khoản", COT_TEN) + cach
        + TienIch::CanTrai("Loại", COT_LOAI) + cach
        + TienIch::CanPhai("Số dư (VND)", COT_SO_DU);

    std::vector<std::string> cacMuc;
    for (std::size_t i = 0; i < m_DanhSach.size(); i++) {
        const TaiKhoan* tk = m_DanhSach[i].get();
        cacMuc.push_back(TienIch::CanTrai(tk->LaySoTaiKhoan(), COT_STK) + cach
            + TienIch::CanTrai(tk->LayTenChuTaiKhoan(), COT_TEN) + cach
            + TienIch::CanTrai(tk->LayLoaiTaiKhoan(), COT_LOAI) + cach
            + TienIch::CanPhai(TienIch::DinhDangTien(tk->LaySoDu()), COT_SO_DU));
    }
    cacMuc.push_back("<< Quay lại");

    const int viTri = TienIch::ChonMenu(tieuDe, cacMuc, dongTieuDeCot);
    if (viTri < 0 || viTri >= static_cast<int>(m_DanhSach.size())) {
        return nullptr;
    }
    return m_DanhSach[viTri].get();
}

//In bang danh sach tat ca tai khoan
void NganHang::XemDanhSach() const {
    TienIch::XoaManHinh();
    TienIch::InTieuDe("DANH SÁCH TÀI KHOẢN");
    if (m_DanhSach.empty()) {
        TienIch::BaoThongTin("Chưa có tài khoản nào trong hệ thống.");
        TienIch::TamDung();
        return;
    }

    long long tongSoDu = 0;
    std::cout << "\n";
    TaiKhoan::InDuongKeBang(std::cout, 0);
    TaiKhoan::InTieuDeBang(std::cout);
    TaiKhoan::InDuongKeBang(std::cout, 1);

    for (std::size_t i = 0; i < m_DanhSach.size(); i++) {
        // Phan "║ STT " in o day, phan con lai do operator<< cua tai khoan in

        std::cout << TienIch::LeTrai(DO_RONG_BANG) << TienIch::KeDoc() << " " << std::right << std::setw(DO_RONG_COT_STT) << (i + 1) << " "
                  << *m_DanhSach[i] << "\n";
        tongSoDu += m_DanhSach[i]->LaySoDu();
    }

    TaiKhoan::InDuongKeBang(std::cout, 2);

    std::cout << "\n";
    TienIch::BatDauBangThongTin();
    TienIch::InDong("Tổng số tài khoản", std::to_string(m_DanhSach.size()), MAU_VANG);
    TienIch::InDong("Tổng số dư", TienIch::DinhDangTien(tongSoDu) + " VND",
                    tongSoDu < 0 ? MAU_DO : MAU_XANH_LA);
    TienIch::KetThucBangThongTin();
    TienIch::TamDung();
}

//n Xem chi tiet mot tai khoan (goi HienThiThongTin da hinh)
void NganHang::XemChiTiet() const {
    TaiKhoan* taiKhoan = ChonTaiKhoan("XEM THÔNG TIN CHI TIẾT");
    if (taiKhoan == nullptr) {
        return;
    }
    TienIch::XoaManHinh();
    TienIch::InTieuDe("THÔNG TIN CHI TIẾT");
    TienIch::BatDauBangThongTin();
    taiKhoan->HienThiThongTin();
    TienIch::KetThucBangThongTin();
    TienIch::TamDung();
}

// Them tai khoan moi: chon loai, nhap bang operator>>
void NganHang::ThemTaiKhoan() {
    std::vector<std::string> cacLoai;
    cacLoai.push_back("Tài khoản tiết kiệm");
    cacLoai.push_back("Tài khoản thanh toán");
    cacLoai.push_back("<< Quay lại");

    std::unique_ptr<TaiKhoan> taiKhoanMoi;
    switch (TienIch::ChonMenu("THÊM TÀI KHOẢN MỚI", cacLoai)) {
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
    TienIch::InTieuDe("NHẬP THÔNG TIN TÀI KHOẢN");
    TienIch::BaoThongTin("Để trống và nhấn Enter bất kỳ lúc nào để hủy.");
    std::cout << "\n";

    // operator>> goi phuong thuc ao Nhap() => tu nhap dung thuoc tinh cua tung loai
    if (!(std::cin >> *taiKhoanMoi)) {
        std::cin.clear();
        TienIch::BaoThongTin("Đã hủy thêm tài khoản.");
        TienIch::TamDung();
        return;
    }

    while (TimTaiKhoan(taiKhoanMoi->LaySoTaiKhoan()) != nullptr) {
        TienIch::BaoLoi("Số tài khoản này đã tồn tại. Vui lòng nhập số khác.");
        std::string soMoi;
        if (!TienIch::NhapChuoiSo(std::cin, "  Số tài khoản (" + std::to_string(DO_DAI_SO_TAI_KHOAN)
                                            + " chữ số): ", DO_DAI_SO_TAI_KHOAN, soMoi)) {
            std::cin.clear();
            TienIch::BaoThongTin("Đã hủy thêm tài khoản.");
            TienIch::TamDung();
            return;
        }
        taiKhoanMoi->DatSoTaiKhoan(soMoi);
    }

    m_DanhSach.push_back(std::move(taiKhoanMoi));
    LuuFile();
    std::cout << "\n";
    TienIch::BaoThanhCong("Thêm tài khoản thành công.");
    std::cout << "\n";
    TienIch::BatDauBangThongTin();
    m_DanhSach.back()->HienThiThongTin();
    TienIch::KetThucBangThongTin();
    TienIch::TamDung();
}

// Nap tien vao tai khoan da chon
void NganHang::NapTien() {
    TaiKhoan* taiKhoan = ChonTaiKhoan("NẠP TIỀN - CHỌN TÀI KHOẢN");
    if (taiKhoan == nullptr) {
        return;
    }
    TienIch::XoaManHinh();
    TienIch::InTieuDe("NẠP TIỀN");
    TienIch::BatDauBangThongTin();
    TienIch::InDong("Tài khoản nhận", taiKhoan->LaySoTaiKhoan());
    TienIch::InDong("Chủ tài khoản", taiKhoan->LayTenChuTaiKhoan());
    TienIch::InDong("Số dư hiện tại", TienIch::DinhDangTien(taiKhoan->LaySoDu()) + " VND",
                    taiKhoan->LaySoDu() < 0 ? MAU_DO : MAU_XANH_LA);
    TienIch::KetThucBangThongTin();
    std::cout << "\n";

    // Buoc 1: nhap so tien (de trong + Enter = huy)
    long long soTien = 0;
    if (!TienIch::NhapSoTienTrongKhung("Nhập số tiền cần nạp (VND): ", soTien, 1, SO_DU_TOI_DA)) {
        std::cin.clear();
        TienIch::BaoThongTin("Đã hủy giao dịch.");
        TienIch::TamDung();
        return;
    }

    // Buoc 2: hoi xac nhan: Enter = thuc hien, Esc = huy
    if (!TienIch::XacNhanGiaoDich()) {
        TienIch::BaoThongTin("Đã hủy giao dịch.");
        TienIch::TamDung();
        return;
    }

    // Buoc 3: thuc hien giao dich
    std::string thongBao;
    if (taiKhoan->NapTien(soTien, thongBao)) {
        LuuFile();
        TienIch::BaoThanhCong(thongBao);
        TienIch::InDong("Số dư mới", TienIch::DinhDangTien(taiKhoan->LaySoDu()) + " VND", MAU_XANH_LA);
    } else {
        TienIch::BaoLoi(thongBao);
    }
    TienIch::TamDung();
}

// Rut tien tu tai khoan da chon (da hinh: RutTien cua tung loai)
void NganHang::RutTien() {
    TaiKhoan* taiKhoan = ChonTaiKhoan("RÚT TIỀN - CHỌN TÀI KHOẢN");
    if (taiKhoan == nullptr) {
        return;
    }
    TienIch::XoaManHinh();
    TienIch::InTieuDe("RÚT TIỀN");
    TienIch::BatDauBangThongTin();
    TienIch::InDong("Số tài khoản", taiKhoan->LaySoTaiKhoan());
    TienIch::InDong("Chủ tài khoản", taiKhoan->LayTenChuTaiKhoan());
    TienIch::InDong("Số dư hiện tại", TienIch::DinhDangTien(taiKhoan->LaySoDu()) + " VND",
                    taiKhoan->LaySoDu() < 0 ? MAU_DO : MAU_XANH_LA);
    taiKhoan->HienThiHanMucRut();       // goi dung phien ban cua lop con (da hinh)
    TienIch::KetThucBangThongTin();
    std::cout << "\n";

    // Buoc 1: nhap so tien (de trong + Enter = huy)
    long long soTien = 0;
    if (!TienIch::NhapSoTienTrongKhung("Nhập số tiền cần rút (VND): ", soTien, 1, SO_DU_TOI_DA)) {
        std::cin.clear();
        TienIch::BaoThongTin("Đã hủy giao dịch.");
        TienIch::TamDung();
        return;
    }

    // Buoc 2: hoi xac nhan: Enter = thuc hien, Esc = huy
    if (!TienIch::XacNhanGiaoDich()) {
        TienIch::BaoThongTin("Đã hủy giao dịch.");
        TienIch::TamDung();
        return;
    }

    // Buoc 3: thuc hien giao dich
    std::string thongBao;
    if (taiKhoan->RutTien(soTien, thongBao)) {      // Goi dung phien ban cua lop con
        LuuFile();
        TienIch::BaoThanhCong(thongBao);
        TienIch::InDong("Số dư mới", TienIch::DinhDangTien(taiKhoan->LaySoDu()) + " VND",
                        taiKhoan->LaySoDu() < 0 ? MAU_DO : MAU_XANH_LA);
    } else {
        TienIch::BaoLoi(thongBao);
    }
    TienIch::TamDung();
}

// Vong lap menu chinh, ket thuc khi chon "Thoat"
void NganHang::Chay() {
    TaiDuLieu();

    std::vector<std::string> cacMuc;
    cacMuc.push_back("Xem danh sách tài khoản");
    cacMuc.push_back("Xem thông tin chi tiết");
    cacMuc.push_back("Thêm tài khoản mới");
    cacMuc.push_back("Nạp tiền");
    cacMuc.push_back("Rút tiền");
    cacMuc.push_back("Thoát chương trình");

    bool daThoat = false;
    while (!daThoat) {
        switch (TienIch::ChonMenu("MÁY ATM - QUẢN LÝ TÀI KHOẢN", cacMuc)) {
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
    const std::string loiChao = "Cảm ơn bạn đã sử dụng dịch vụ ATM. Tạm biệt!";
    TienIch::DatMau(MAU_CHU_DAO);
    std::cout << "\n" << TienIch::LeTrai(TienIch::DoRongHienThi(loiChao)) << loiChao << "\n\n";
    TienIch::DatMauMacDinh();
}
