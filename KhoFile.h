#ifndef KHOFILE_H_INCLUDED_
#define KHOFILE_H_INCLUDED_

#include <memory>
#include <string>
#include <vector>

#include "TaiKhoan.h"

namespace KhoFile {

    // Doc file vao danhSach. Dong sai khong lam dung chuong trinh:
    // dong do bi bo qua va mo ta loi duoc them vao cacLoi.
    // Tra ve false neu khong mo duoc file (vi du file chua ton tai).
    bool Doc(const std::string& duongDan,
             std::vector<std::unique_ptr<TaiKhoan> >& danhSach,
             std::vector<std::string>& cacLoi);

    // Ghi toan bo danh sach ra file. Tra ve false neu ghi that bai.
    bool Ghi(const std::string& duongDan,
             const std::vector<std::unique_ptr<TaiKhoan> >& danhSach);

} // namespace KhoFile

#endif
