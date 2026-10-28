// =====================================================================
//  TaiKhoan.h - Lop co so (Base Class) cua he thong tai khoan ngan hang
// =====================================================================
#ifndef TAIKHOAN_H_INCLUDED_
#define TAIKHOAN_H_INCLUDED_

#include <iostream>
#include <string>

class TaiKhoan {
private:
    std::string m_SoTaiKhoan;
    std::string m_TenChuTaiKhoan;
    long long m_SoDu;

    void Xuat(std::ostream& os) const;      // Mot dong cua bang danh sach

protected:
    // Lop con ghi de de nhap them thuoc tinh rieng (goi TaiKhoan::Nhap truoc)
    virtual void Nhap(std::istream& is);

    // Kiem tra rang buoc chung cua moi lan rut: so tien phai lon hon 0
    bool KiemTraSoTienRut(long long soTien, std::string& thongBao) const;

    // "SoTaiKhoan|Ten|SoDu" - phan chung ma lop con noi them thuoc tinh rieng
    std::string LayDongChung() const;

public:
    // ---- Constructor / Destructor ----
    TaiKhoan();
    TaiKhoan(const std::string& soTaiKhoan, const std::string& tenChuTaiKhoan, long long soDu);
    virtual ~TaiKhoan();

    // ---- Getter ----
    const std::string& LaySoTaiKhoan() const;
    const std::string& LayTenChuTaiKhoan() const;
    long long LaySoDu() const;

    // ---- Setter ----
    void DatSoTaiKhoan(const std::string& soTaiKhoan);
    void DatTenChuTaiKhoan(const std::string& tenChuTaiKhoan);
    void DatSoDu(long long soDu);

    // ---- Nghiep vu ----
    bool NapTien(long long soTien, std::string& thongBao);

    // ---- Da hinh: moi loai tai khoan cai dat rieng ----
    virtual bool RutTien(long long soTien, std::string& thongBao);
    virtual void HienThiThongTin() const;
    virtual std::string LayLoaiTaiKhoan() const;
    virtual std::string LayThongSoRieng() const;    // Cot "Thong so rieng" cua bang
    virtual std::string LayQuyDinhRut() const;      // Mo ta ngan quy dinh rut tien
    virtual std::string LayDongFile() const;        // Mot dong ghi ra file TaiKhoan.txt

    // ---- Ke bang danh sach (dung chung do rong cot voi operator<<) ----
    static void InDuongKeBang(std::ostream& os);
    static void InTieuDeBang(std::ostream& os);

    // ---- Qua tai toan tu nhap / xuat ----
    friend std::istream& operator>>(std::istream& is, TaiKhoan& taiKhoan);
    friend std::ostream& operator<<(std::ostream& os, const TaiKhoan& taiKhoan);
};

#endif