// =====================================================================
//  MayATM.h - Lop quan ly danh sach tai khoan va dieu khien menu ATM
// =====================================================================
#ifndef MAYATM_H_INCLUDED_
#define MAYATM_H_INCLUDED_

#include <memory>
#include <string>
#include <vector>

#include "TaiKhoan.h"

class MayATM {
private:
    // Con tro tren lop co so => chua duoc ca TaiKhoanTietKiem va TaiKhoanThanhToan (da hinh).
    // unique_ptr tu giai phong bo nho, khong can viet destructor.
    std::vector<std::unique_ptr<TaiKhoan> > m_DanhSach;

    void TaiDuLieu();                       // Doc TaiKhoan.txt (chua co thi tao file mau)
    void LuuFile() const;                   // Ghi lai TaiKhoan.txt sau moi thay doi
    TaiKhoan* TimTaiKhoan(const std::string& soTaiKhoan) const;
    TaiKhoan* ChonTaiKhoan(const std::string& tieuDe) const;

    void XemDanhSach() const;
    void XemChiTiet() const;
    void ThemTaiKhoan();
    void NapTien();
    void RutTien();

public:
    void Chay();
};

#endif
