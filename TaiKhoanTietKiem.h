// =====================================================================
//  TaiKhoanTietKiem.h - Lop con: tai khoan tiet kiem (co lai suat)
// =====================================================================
#ifndef TAIKHOANTIETKIEM_H_INCLUDED_
#define TAIKHOANTIETKIEM_H_INCLUDED_

#include "TaiKhoan.h"

class TaiKhoanTietKiem : public TaiKhoan {
private:
    double m_LaiSuat;           // Don vi: %/nam

protected:
    void Nhap(std::istream& is) override;

public:
    TaiKhoanTietKiem();
    TaiKhoanTietKiem(const std::string& soTaiKhoan, const std::string& tenChuTaiKhoan,
                     long long soDu, double laiSuat);

    double LayLaiSuat() const;
    void DatLaiSuat(double laiSuat);

    long long TinhTienLaiThang() const;     // Lai don 1 thang
    long long TinhTienLaiNam() const;       // Lai don 1 nam

    // ---- Ghi de cac phuong thuc ao ----
    bool RutTien(long long soTien, std::string& thongBao) override;
    void HienThiThongTin() const override;
    std::string LayLoaiTaiKhoan() const override;
    std::string LayThongSoRieng() const override;
    std::string LayQuyDinhRut() const override;
    std::string LayDongFile() const override;
};

#endif
