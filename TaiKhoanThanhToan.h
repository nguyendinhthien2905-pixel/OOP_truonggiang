// =====================================================================
//  TaiKhoanThanhToan.h - Lop con: tai khoan thanh toan (co han muc tin dung)
// =====================================================================
#ifndef TAIKHOANTHANHTOAN_H_INCLUDED_
#define TAIKHOANTHANHTOAN_H_INCLUDED_

#include "TaiKhoan.h"

class TaiKhoanThanhToan : public TaiKhoan {
private:
    long long m_HanMucTinDung;  // So tien duoc phep rut vuot so du (VND)

protected:
    void Nhap(std::istream& is) override;

public:
    TaiKhoanThanhToan();
    TaiKhoanThanhToan(const std::string& soTaiKhoan, const std::string& tenChuTaiKhoan,
                      long long soDu, long long hanMucTinDung);

    long long LayHanMucTinDung() const;
    void DatHanMucTinDung(long long hanMucTinDung);

    long long LaySoTienKhaDung() const;     // = So du + Han muc tin dung
    long long LayDuNo() const;              // = So tien dang ghi no (0 neu khong no)

    // ---- Ghi de cac phuong thuc ao ----
    bool RutTien(long long soTien, std::string& thongBao) override;
    void HienThiThongTin() const override;
    std::string LayLoaiTaiKhoan() const override;
    std::string LayThongSoRieng() const override;
    std::string LayQuyDinhRut() const override;
    std::string LayDongFile() const override;
};

#endif
