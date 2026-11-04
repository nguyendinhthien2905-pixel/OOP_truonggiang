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

    //hằng số nội bộ cấu hình giao diện console (đơn bị: kí tự)
    const int DO_RONG_KHUNG = TienIch::DO_RONG_KHUNG;  // độ rộng khung tiêu đề, menu, bảng thông tin
    const int SO_MUC_HIEN_THI_TOI_DA = 10;  // số lượng mục menu tối đa, dài hơn sẽ tự cuộn
    const int DO_RONG_NHAN = 20;  // độ rộng nhãn trong khung thông tin
    const int SO_CHU_SO_TOI_DA = 15;        // số chữ số tối đa cho phép khi nhập
    const int NHOM_HANG_NGAN = 3;           // cách khoảng hàng nghìn, triệu,...

    const int MA_PHIM_ENTER = 13;           // mã ASCII phím Enter: chọn mục hoặc kết thúc nhập
    const int MA_PHIM_ESC = 27;             // mã ASCII phím ESC: thoát, quay lại hoặc hủy thao tác

    const int MA_PHIM_MO_RONG_1 = 0;        // mã tiền tố cho phím mũi tên và phím chức năng, đọc lần lượt 0, 2, mã thật
    const int MA_PHIM_MO_RONG_2 = 224;

    ////di chuyển con trỏ trong menu
    const int MA_MUI_TEN_LEN = 72;          // mã mũi tên lên
    const int MA_MUI_TEN_XUONG = 80;        // mã mũi tên xuống

    WORD g_mauMacDinh = 7;                  // thuộc tính màu lúc vào màn hình console
    bool g_coMau24Bit = false;              // console có hỗ trợ mã ANSI (màu 24-bit) hay không
    bool g_trongBangThongTin = false;       // InDong đang in vào khung thông tin hay không

    const char* const KHUNG_TREN_TRAI = "╔";
    const char* const KHUNG_TREN_PHAI = "╗";
    const char* const KHUNG_DUOI_TRAI = "╚";
    const char* const KHUNG_DUOI_PHAI = "╝";
    const char* const KHUNG_GIUA_TRAI = "╠";   // dùng cho đường ngăn cách giữa các nhóm
    const char* const KHUNG_GIUA_PHAI = "╣";
    const char* const KHUNG_NGANG = "═";
    const char* const KHUNG_DOC = "║";
    const char* const MUI_TEN_CHON = "►";

    //chữ "ATM BANK" kiểu khối. Ký hiệu tạm: # = █, [ = ╔, ] = ╗, { = ╚, } = ╝, - = ═, | = ║
    const char* const BANNER[] = {
        " #####] ########] ###]   ###]    ######]  #####] ###]   ##] ##]  ##]",
        "##[--##]{--##[--} ####] ####|    ##[--##]##[--##]####]  ##| ##| ##[}",
        "#######|   ##|    ##[####[##|    ######[}#######|##[##] ##| #####[} ",
        "##[--##|   ##|    ##|{##[}##|    ##[--##]##[--##|##|{##]##| ##[-##] ",
        "##|  ##|   ##|    ##| {-} ##|    ######[}##|  ##|##| {####| ##|  ##]",
        "{-}  {-}   {-}    {-}     {-}    {-----} {-}  {-}{-}  {---} {-}  {-}"
    };
    const int SO_DONG_BANNER = 6;

    //đổi ký hiệu tạm trong BANNER thành ký tự khối thật
    const char* KyTuBanner(char c) {
        switch (c) {
        case '#': return "█";
        case '[': return "╔";
        case ']': return "╗";
        case '{': return "╚";
        case '}': return "╝";
        case '-': return "═";
        case '|': return "║";
        default:  return " ";
        }
    }

    //bảng màu RGB tương ứng với từng mã màu
    void LayRGB(Mau mau, int& r, int& g, int& b) {
        switch (mau) {
        case MAU_DEN:        r = 30;  g = 30;  b = 30;  break;
        case MAU_XAM:        r = 190; g = 190; b = 200; break;   // xám sáng cho nhãn dễ đọc
        case MAU_CHU_DAO:    r = 255; g = 121; b = 198; break;   // hồng #ff79c6
        case MAU_XANH_DUONG: r = 97;  g = 175; b = 239; break;
        case MAU_XANH_LA:    r = 80;  g = 250; b = 123; break;   // xanh lá cho số tiền nổi bật
        case MAU_DO:         r = 255; g = 85;  b = 85;  break;
        case MAU_VANG:       r = 241; g = 250; b = 140; break;
        default:             r = 248; g = 248; b = 242; break;   // trắng
        }
    }

    //sinh mã ANSI màu chữ (loai = 38) hoặc màu nền (loai = 48)
    std::string MaANSI(int loai, int r, int g, int b) {
        return "\x1b[" + std::to_string(loai) + ";2;" + std::to_string(r) + ";"
            + std::to_string(g) + ";" + std::to_string(b) + "m";
    }

    //vẽ 1 đường viền ngang của khung, căn giữa màn hình
    void InVienNgang(const char* trai, const char* phai, int doRong) {
        std::cout << TienIch::LeTrai(doRong) << trai << TienIch::LapLai(KHUNG_NGANG, doRong - 2) << phai << "\n";
    }


    //thiết lập menu tương tác (không có thì chỉ là console bình thường)
    HANDLE LayManHinh() {
        return GetStdHandle(STD_OUTPUT_HANDLE);
    }

    //lấy số thứ tự dòng mà con trỏ đang đứng
    int LayDongHienTai() {
        std::cout.flush();
        CONSOLE_SCREEN_BUFFER_INFO thongTin;
        if (GetConsoleScreenBufferInfo(LayManHinh(), &thongTin)) {
            return thongTin.dwCursorPosition.Y;
        }
        return 0;
    }

    //tự động lưu trạng thái và khôi phục nguyên trạng của định dạng luồng xuất (left/right, setw, setfill, ...)
    class BaoVeDinhDang {
    private:
        //ghi lại trạng thái của luồng xuất dữ liệu ->snapshot cho hàm hủy khôi phục lại nguyên trạng
        std::ostream& m_os;                     //tham chiếu đền luồng xuất dữ liệu
        std::ios_base::fmtflags m_coDinhDang;   //kiểu định dạng (hệ cơ số, căn lề,...)
        char m_kyTuDien;                        //kí tự điền vào khoảng trống

    public:
        //lưu lại trạng thái của luồng xuát dữ liệu khi obj được tạo ra
        explicit BaoVeDinhDang(std::ostream& os)
            : m_os(os), m_coDinhDang(os.flags()), m_kyTuDien(os.fill()) {
        }

        //trả lại hiện trạng ban đầu
        ~BaoVeDinhDang() {
            m_os.flags(m_coDinhDang);       //lấy lại thiết lập được lưu lúc khởi tạo
            m_os.fill(m_kyTuDien);          //khôi phục lại lí tự điền khoảng trống
        }
    };


    //kiểm tra chuỗi chỉ bao gồm những chữ số
    bool ChiGomChuSo(const std::string& chuoi) {
        //kiểm tra chuỗi rỗng thì trả về false
        if (chuoi.empty()) {
            return false;
        }
        //chuỗi không trống thì duyệt qua từng kí tự xem có phải số thập phân không
        for (std::string::size_type i = 0; i < chuoi.size(); i++) {
            if (!std::isdigit(static_cast<unsigned char>(chuoi[i]))) {
                return false;
            }
        }
        return true;
    }

    //đọc dữ liệu từ luồng nhập, hiển thị loiNhac, làm sạch khoảng trắng đảm bảo kết quả trả về không phải chuỗi rỗng
    //tránh các lỗi khi std::cin>> :sót kí tự xuống dòng, cin>> ngắt chuỗi,...
    bool DocDong(std::istream& is, const std::string& loiNhac, std::string& dong) {
        std::cout << TienIch::LeTrai(DO_RONG_KHUNG) << loiNhac << std::flush;
        if (!std::getline(is, dong)) {
            return false;
        }
        dong = TienIch::CatKhoangTrang(dong);
        return !dong.empty();
    }

    //chuẩn hóa dấu phẩy thành dấu chấm, đếm số dấu chấm, đếm số chữ số trong chuỗi
    bool DocSoThuc(std::string chuoi, double& ketQua) {
        //chuẩn hóa dấu phẩy thành dấu chấm
        for (std::string::size_type i = 0; i < chuoi.size(); i++) {
            if (chuoi[i] == ',') {
                chuoi[i] = '.';
            }
        }
        //đếm số dấu chấm, số chữ số trong chuỗi
        int soDauCham = 0;
        int soChuSo = 0;
        for (std::string::size_type i = 0; i < chuoi.size(); i++) {
            if (chuoi[i] == '.') {
                soDauCham++;
            }
            else if (std::isdigit(static_cast<unsigned char>(chuoi[i]))) {
                soChuSo++;
            }
            else {
                return false;
            }
        }
        if (soDauCham > 1 || soChuSo == 0 || chuoi.size() > 10) {
            return false;
        }
        ketQua = std::strtod(chuoi.c_str(), NULL);
        return true;
    }

    //chuyển số thực sang chuỗi: double -> string, trả về string, thực hiện trên tiền tệ
    std::string SoThucSangChuoi(double giaTri) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << giaTri;
        return ss.str();
    }

    //vẽ và hiển thị giao diện menu trên màn hình console (cập nhật lại toàn bộ giao diện menu khi người dùng di chuyển)
    void VeMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc, int viTri, int dau, int soDong,
        const std::string& dongTieuDeCot) {
        const int soMuc = static_cast<int>(cacMuc.size());
        const std::string le = TienIch::LeTrai(DO_RONG_KHUNG);

        static int doRongLanTruoc = 0;
        const int doRongHienTai = TienIch::LayDoRongConsole();
        if (doRongHienTai != doRongLanTruoc) {
            TienIch::XoaManHinh();
            doRongLanTruoc = doRongHienTai;
        }

        TienIch::DatViTriCon(0, 0); //đưa con trỏ về góc trên cùng bên trái (x=0,y=0) để khi vẽ lại thì không đè lên các lệnh của màn hình trước
        TienIch::InTieuDe(tieuDe);

        //in thanh hướng dẫn lên/xuống/enter... (căn giữa theo độ dài của chính dòng chữ)
        const std::string huongDan = "↑/↓: di chuyển  -  Enter: chọn  -  Esc: quay lại";
        TienIch::DatMau(MAU_XAM);
        std::cout << TienIch::LeTrai(TienIch::DoRongHienThi(huongDan)) << huongDan << "\n\n";

        if (!dongTieuDeCot.empty()) {
            TienIch::DatMau(MAU_CHU_DAO);
            std::cout << le << TienIch::CanTrai("    " + dongTieuDeCot, DO_RONG_KHUNG) << "\n";
            std::cout << le << TienIch::LapLai("─", DO_RONG_KHUNG) << "\n";
        }

        TienIch::DatMau(MAU_CHU_DAO);
        //kiểm tra danh sách có bị cuộn xuống dưới hay không, nếu có in ra kí tự ▲... để người dùng biết
        std::cout << le << TienIch::CanTrai(dau > 0 ? "      ▲ ..." : "", DO_RONG_KHUNG) << "\n";

        //vòng lặp hiển thị danh sách menu
        for (int i = dau; i < dau + soDong; i++) {
            const std::string& noiDung = cacMuc[i];
            std::cout << le;
            //xử lí vị trí đang chọn: nền hồng, chữ đen và hiển thị ► trước nội dung đang chọn
            if (i == viTri) {
                TienIch::DatMauNen(MAU_DEN, MAU_CHU_DAO);   // chữ đen trên nền hồng
                std::cout << TienIch::CanTrai(std::string("  ") + MUI_TEN_CHON + " " + noiDung, DO_RONG_KHUNG);
            }
            //xử lí vị trí không đang được chọn: màu chữ trắng, thụt lề bình thường
            else {
                TienIch::DatMau(MAU_TRANG);
                std::cout << TienIch::CanTrai("    " + noiDung, DO_RONG_KHUNG);
            }
            TienIch::DatMauMacDinh();
            std::cout << "\n";
        }

        TienIch::DatMau(MAU_CHU_DAO);
        //báo hiệu cuộn xuống ▼.. khi còn các mục phía dưới
        std::cout << le << TienIch::CanTrai(dau + soDong < soMuc ? "      ▼ ..." : "", DO_RONG_KHUNG) << "\n";
        TienIch::DatMauMacDinh();
        //in lập tức để tránh giật màn hình hiển thị
        std::cout.flush();
    }

} // namespace

namespace TienIch {

    //khởi tạo màn hình console
    void KhoiTaoConsole() {
        std::system("mode con: cols=120 lines=40");                     //thiết đặt kích thước màn hình console: 120 cột và 40 dòng
        SetConsoleTitleA(TIEU_DE_CONSOLE);      //thiết đặt tiêu đề cho màn hình console
        CONSOLE_SCREEN_BUFFER_INFO thongTin;                            //thiết đặt màu săc mặc định cho hệ thống
        if (GetConsoleScreenBufferInfo(LayManHinh(), &thongTin)) {
            g_mauMacDinh = thongTin.wAttributes;
        }
        SetConsoleOutputCP(CP_UTF8);                                    //in được chữ có dấu, ký tự khung viền, chữ khối
        DWORD cheDo = 0;                                                //bật mã ANSI để dùng màu 24-bit
        if (GetConsoleMode(LayManHinh(), &cheDo)) {
            g_coMau24Bit = SetConsoleMode(LayManHinh(), cheDo | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
        }
    }


    //thay đổi màu sắc chữ mà không thay đổi màu nền trên cửa sổ console
    void DatMau(Mau mauChu) {
        std::cout.flush();
        if (g_coMau24Bit) {
            int r, g, b;
            LayRGB(mauChu, r, g, b);
            std::cout << "\x1b[0m" << MaANSI(38, r, g, b);
            return;
        }
        //thay đổi thuộc tính hiển thị (màu chữ, màu nền)
        SetConsoleTextAttribute(LayManHinh(), static_cast<WORD>(mauChu | (g_mauMacDinh & 0xF0)));
    }

    //thiết lập màu chữ và màu nền trong màn hình console
    void DatMauNen(Mau mauChu, Mau mauNen) {
        std::cout.flush();
        if (g_coMau24Bit) {
            int r, g, b;
            LayRGB(mauChu, r, g, b);
            std::cout << MaANSI(38, r, g, b);
            LayRGB(mauNen, r, g, b);
            std::cout << MaANSI(48, r, g, b);
            return;
        }
        SetConsoleTextAttribute(LayManHinh(), static_cast<WORD>(mauChu | (mauNen << 4)));
    }

    //thiết đặt lại màu gốc của màn hình console: đen (0), trắng (7)
    void DatMauMacDinh() {
        if (g_coMau24Bit) {
            std::cout << "\x1b[0m";
        }
        std::cout.flush();
        SetConsoleTextAttribute(LayManHinh(), g_mauMacDinh);
    }

    //xóa toàn bộ nội dung trên màn hình console và ddauw con trỏ về góc trên bên trái
    void XoaManHinh() {
        std::cout.flush();
        HANDLE hManHinh = LayManHinh();
        CONSOLE_SCREEN_BUFFER_INFO thongTin;
        //kiểm tra thông tin chi tiết về bộ đệm hiện tại của cửa sổ consle (số hàng, số cột), nếu có thì chạy code bên dưới, không thì chạt code cls
        if (!GetConsoleScreenBufferInfo(hManHinh, &thongTin)) {
            std::system("cls");     //gọi một tiến trình con làm mới toàn bộ cửa sổ
            return;
        }

        //tính toán tổng số ô kí tự trên màn hình
        const DWORD soO = static_cast<DWORD>(thongTin.dwSize.X) * thongTin.dwSize.Y;    //tổng số lượng ô ký tự = chiều rộng (số cột) x chiều cao (số dòng), X là số cột, Y là số dòng
        DWORD soDaGhi = 0;
        //thiết lập tọa độ gốc, X = 0, Y = 0, gốc ở vị trí trên cùng bên trái
        COORD goc;
        goc.X = 0;
        goc.Y = 0;

        //xóa kí tự và làm sạch màu sắc
        FillConsoleOutputCharacterA(hManHinh, ' ', soO, goc, &soDaGhi);             //ghi đè khoảng trắng ' ' lên toàn bộ ô ký tự trên của sổ console
        FillConsoleOutputAttribute(hManHinh, g_mauMacDinh, soO, goc, &soDaGhi);     //làm sạch màu sắc: áp dụng màu mặc định lên toàn bộ màn hình console

        //đưa con trỏ về vị trí ban đầu: tọa độ (0,0)
        SetConsoleCursorPosition(hManHinh, goc);
    }

    //di chuyển con trỏ đến một tọa độ (x,y) trên cửa sổ console
    void DatViTriCon(int x, int y) {
        std::cout.flush();
        //cấu trúc tọa độ: x và y
        COORD viTri;
        viTri.X = static_cast<SHORT>(x);
        viTri.Y = static_cast<SHORT>(y);
        //gọi Win API định vị con trỏ: sau khi lệnh này chạy, con trỏ sẽ di chuyển tới vị trí có tọa độ (x,y)
        SetConsoleCursorPosition(LayManHinh(), viTri);
    }

    //ẩn hoặc hiện con trỏ nhấp nháy trên cửa sổ console: dựa vào biến hien để quyết định
    void HienConTro(bool hien) {
        //CONSOLE_CURSOR_INFO là struct chuẩn winAPI lưu giữ thuộc tính (kích thước, bVisible nhấp nháy) của con trỏ console
        CONSOLE_CURSOR_INFO thongTin;
        //lấy thông tin từ con trỏ lưu vào biến thông tin
        if (GetConsoleCursorInfo(LayManHinh(), &thongTin)) {

            thongTin.bVisible = hien ? TRUE : FALSE;            //dựa vào biến hiện đầu vào của hàm để quyết định con trỏ có nhấp nháy hay không
            SetConsoleCursorInfo(LayManHinh(), &thongTin);      //cập nhật hệ thống
        }
    }


    // Lấy số cột của cửa sổ console đang mở.
    // srWindow là vùng đang nhìn thấy trên màn hình => chiều rộng = Right - Left + 1.
    // Nhờ lấy theo cửa sổ thật nên khi người dùng kéo to/nhỏ cửa sổ thì màn hình sau vẫn căn giữa đúng.
    int LayDoRongConsole() {
        CONSOLE_SCREEN_BUFFER_INFO thongTin;
        if (GetConsoleScreenBufferInfo(LayManHinh(), &thongTin)) {
            return thongTin.srWindow.Right - thongTin.srWindow.Left + 1;
        }
        return 120;     // không lấy được thì coi như 120 cột (bằng kích thước đặt trong KhoiTaoConsole)
    }

    // Trả về chuỗi khoảng trắng để nội dung rộng doRongNoiDung nằm giữa màn hình.
    // Ví dụ: cửa sổ 120 cột, khung 60 cột => lề trái = (120 - 60) / 2 = 30 dấu cách.
    std::string LeTrai(int doRongNoiDung) {
        int le = (LayDoRongConsole() - doRongNoiDung) / 2;
        if (le < 0) {
            le = 0;     // nội dung rộng hơn cửa sổ thì không thụt lề
        }
        return std::string(le, ' ');
    }

    // Đếm số ký tự HIỂN THỊ của chuỗi UTF-8.
    // Trong UTF-8, 1 chữ có dấu (vd "ố") gồm nhiều byte: byte đầu + các byte nối có dạng 10xxxxxx.
    // => Chỉ cần đếm những byte KHÔNG phải byte nối là ra số ký tự.
    // (c & 0xC0) != 0x80 nghĩa là 2 bit đầu của byte khác "10".
    int DoRongHienThi(const std::string& chuoi) {
        int dem = 0;
        for (std::string::size_type i = 0; i < chuoi.size(); i++) {
            const unsigned char c = static_cast<unsigned char>(chuoi[i]);
            if ((c & 0xC0) != 0x80) {
                dem++;
            }
        }
        return dem;
    }

    // Căn trái: thêm dấu cách vào bên phải cho đủ doRong ký tự hiển thị
    std::string CanTrai(const std::string& chuoi, int doRong) {
        const int thieu = doRong - DoRongHienThi(chuoi);
        return thieu > 0 ? chuoi + std::string(thieu, ' ') : chuoi;
    }

    // Căn phải: thêm dấu cách vào bên trái cho đủ doRong ký tự hiển thị
    std::string CanPhai(const std::string& chuoi, int doRong) {
        const int thieu = doRong - DoRongHienThi(chuoi);
        return thieu > 0 ? std::string(thieu, ' ') + chuoi : chuoi;
    }

    // Lặp lại chuỗi soLan lần, vd LapLai("═", 3) = "═══".
    // Không dùng std::string(soLan, '═') được vì '═' dài 3 byte, không phải 1 char.
    std::string LapLai(const std::string& chuoi, int soLan) {
        std::string ketQua;
        for (int i = 0; i < soLan; i++) {
            ketQua += chuoi;
        }
        return ketQua;
    }


    //độc dữ liệu nhập vào từ người dùng theo thời gian thực (không cần enter nhờ _getch() ) và dịch nó thành các trạng thái phím có nghĩa
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

    //tạm dừng chương trình và hiển thị một thông báo, chờ đến khi người dùng phản hồi (giống system("pause") nhưng chỉnh được màu và hiển thị tiếng việt)
    void TamDung() {
        const std::string thongBao = "Nhấn phím bất kỳ để tiếp tục...";
        DatMau(MAU_VANG);
        std::cout << "\n" << LeTrai(DoRongHienThi(thongBao)) << thongBao;   // căn giữa dòng nhắc
        DatMauMacDinh();
        std::cout.flush();
        DocPhim();
    }

    //controller chính điều khiển giao diện cửa sổ console
    int ChonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc, const std::string& dongTieuDeCot) {
        //khởi tạo và kiểm tra các điều kiện
        const int soMuc = static_cast<int>(cacMuc.size());
        if (soMuc == 0) {
            return KHONG_CHON;
        }
        const int soDong = (soMuc < SO_MUC_HIEN_THI_TOI_DA) ? soMuc : SO_MUC_HIEN_THI_TOI_DA;

        //thiết lập trạng thái bán đầu: bắt đầu từ dòn 0, từ phần tử số 0 của danh sách
        int viTri = 0;
        int dau = 0;
        int ketQua = KHONG_CHON;
        bool daXong = false;

        ///tắt con trỏ nhắp nháy và xóa màn hình
        HienConTro(false);
        XoaManHinh();

        //vòng lặp cốt lỗi: chạy cho tới khi người dùng nhấn phím ENTER hoặc ESC
        while (!daXong) {

            //nếu dữ liệu bị khuất ở phía trên, màn hình dịch lên trên và ngược lại
            if (viTri < dau) {
                dau = viTri;
            }
            if (viTri >= dau + soDong) {
                dau = viTri - soDong + 1;
            }
            VeMenu(tieuDe, cacMuc, viTri, dau, soDong, dongTieuDeCot);


            //xử lí khi người dùng bấm phím điều hướng
            switch (DocPhim()) {
            case PHIM_LEN:
                viTri = (viTri + soMuc - 1) % soMuc;        //ở mục cuối mà bấm mũi tên xuống thì sẽ nhảy lên đầu và ngược lại
                break;
            case PHIM_XUONG:
                viTri = (viTri + 1) % soMuc;                //ở mục cuối mà bấm mũi tên xuống thì sẽ nhảy lên đầu và ngược lại
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

        //dọn dẹp màn hình và trả về kết quả (dữ liệu định danh phím đã ấn)
        HienConTro(true);
        DatMauMacDinh();
        XoaManHinh();
        return ketQua;
    }

    //vẽ banner + một khung tiêu đề có đườnng viền bao quanh, tất cả căn giữa màn hình console
    void InTieuDe(const std::string& tieuDe) {
        //phép tính để căn chữ tiêu đề vào giữa khung
        int khoangTrong = DO_RONG_KHUNG - 2 - DoRongHienThi(tieuDe);
        if (khoangTrong < 0) {
            khoangTrong = 0;
        }

        const int leTrai = khoangTrong / 2;
        const int lePhai = khoangTrong - leTrai;

        //chữ khối "ATM BANK", màu chuyển dần từ hồng (#ff79c6) sang tím nhạt (#bd93f9) theo từng cột
        std::cout << "\n";
        for (int dong = 0; dong < SO_DONG_BANNER; dong++) {
            const std::string hang = BANNER[dong];
            const int doRong = static_cast<int>(hang.size());   // banner chỉ có ký tự ASCII nên size() = số cột
            std::cout << LeTrai(doRong);   // căn giữa banner
            for (int cot = 0; cot < doRong; cot++) {
                if (g_coMau24Bit) {
                    // nội suy màu: cột 0 là màu đầu, cột cuối là màu cuối
                    const int r = 255 + (189 - 255) * cot / doRong;
                    const int g = 121 + (147 - 121) * cot / doRong;
                    const int b = 198 + (249 - 198) * cot / doRong;
                    std::cout << MaANSI(38, r, g, b);
                }
                else if (cot == 0) {
                    DatMau(MAU_CHU_DAO);
                }
                std::cout << KyTuBanner(hang[cot]);
            }
            DatMauMacDinh();
            std::cout << "\n";
        }

        const std::string phuDe = "Quản lý tài khoản ngân hàng - Máy ATM";
        DatMau(MAU_XAM);
        std::cout << LeTrai(DoRongHienThi(phuDe)) << phuDe << "\n\n";  // căn giữa phụ đề

        //khung viền đôi chứa tiêu đề màn hình hiện tại
        DatMau(MAU_CHU_DAO);
        InVienNgang(KHUNG_TREN_TRAI, KHUNG_TREN_PHAI, DO_RONG_KHUNG);
        std::cout << LeTrai(DO_RONG_KHUNG) << KHUNG_DOC;
        DatMau(MAU_TRANG);
        std::cout << std::string(leTrai, ' ') << tieuDe << std::string(lePhai, ' ');    //tiêu đề ở giữa màu trắng
        DatMau(MAU_CHU_DAO);
        std::cout << KHUNG_DOC << "\n";
        InVienNgang(KHUNG_DUOI_TRAI, KHUNG_DUOI_PHAI, DO_RONG_KHUNG);

        DatMauMacDinh();
    }

    //màn hình loading lúc khởi động: banner + khung tiêu đề (giống hệt các màn hình menu) + thanh tiến trình chạy trong thoiGianMs mili giây
    void ManHinhLoading(const std::string& tieuDe, int thoiGianMs) {
        //số dòng InTieuDe in ra: 1 dòng trống + banner + phụ đề và 1 dòng trống + khung 3 dòng
        const int SO_DONG_TIEU_DE = 1 + SO_DONG_BANNER + 2 + 3;
        const int SO_DONG_NOI_DUNG = SO_DONG_TIEU_DE + 1 + 2;       // + 1 dòng trống + thanh loading + dòng trạng thái
        const int DO_RONG_PHAN_TRAM = 5;                            // chỗ dành cho chữ "100%" ở góc phải dưới thanh
        if (thoiGianMs < 1) {
            thoiGianMs = 1;
        }

        //canh nội dung ra giữa chiều cao cửa sổ console
        int yDau = 0;
        int caoManHinh = 40;
        CONSOLE_SCREEN_BUFFER_INFO thongTin;
        if (GetConsoleScreenBufferInfo(LayManHinh(), &thongTin)) {
            yDau = thongTin.srWindow.Top;
            caoManHinh = thongTin.srWindow.Bottom - thongTin.srWindow.Top + 1;
        }
        const int yTren = yDau + ((caoManHinh > SO_DONG_NOI_DUNG) ? (caoManHinh - SO_DONG_NOI_DUNG) / 2 : 0);
        const int yThanh = yTren + SO_DONG_TIEU_DE + 1;
        const int yTrangThai = yThanh + 1;
        const std::string le = LeTrai(DO_RONG_KHUNG);

        HienConTro(false);
        XoaManHinh();
        DatViTriCon(0, yTren);
        InTieuDe(tieuDe);                   //phần tiêu đề vẽ 1 lần, vòng lặp bên dưới chỉ vẽ lại thanh loading

        //tiến độ tính theo thời gian thật (GetTickCount) nên luôn đúng thoiGianMs dù Sleep bị lệch
        const DWORD batDau = GetTickCount();
        int soOCu = -1;
        int phanTramCu = -1;
        while (true) {
            long long troiQua = static_cast<long long>(GetTickCount() - batDau);
            if (troiQua > thoiGianMs) {
                troiQua = thoiGianMs;
            }
            const int soOday = static_cast<int>(troiQua * DO_RONG_KHUNG / thoiGianMs);   // số ô đã chạy, thanh dài bằng khung
            const int phanTram = static_cast<int>(troiQua * 100 / thoiGianMs);

            if (soOday != soOCu || phanTram != phanTramCu) {
                //thanh loading: ô đã chạy chuyển màu hồng (#ff79c6) -> tím (#bd93f9) giống banner, ô chưa chạy là ░ xám tối
                DatViTriCon(0, yThanh);
                std::cout << le;
                if (g_coMau24Bit) {
                    std::string thanh;
                    for (int i = 0; i < DO_RONG_KHUNG; i++) {
                        if (i < soOday) {
                            const int r = 255 + (189 - 255) * i / DO_RONG_KHUNG;
                            const int g = 121 + (147 - 121) * i / DO_RONG_KHUNG;
                            const int b = 198 + (249 - 198) * i / DO_RONG_KHUNG;
                            thanh += MaANSI(38, r, g, b) + "█";
                        }
                        else {
                            if (i == soOday) {
                                thanh += MaANSI(38, 80, 80, 100);
                            }
                            thanh += "░";
                        }
                    }
                    std::cout << thanh << "\x1b[0m";
                }
                else {
                    DatMau(MAU_CHU_DAO);
                    std::cout << LapLai("█", soOday);
                    DatMau(MAU_XAM);
                    std::cout << LapLai("░", DO_RONG_KHUNG - soOday);
                }
                DatMauMacDinh();

                //dòng trạng thái (chữ xám, bên trái) và phần trăm (chữ trắng, bên phải) ngay dưới thanh
                std::string trangThai;
                if (phanTram < 35) {
                    trangThai = "Đang khởi động hệ thống...";
                }
                else if (phanTram < 75) {
                    trangThai = "Đang tải dữ liệu tài khoản...";
                }
                else if (phanTram < 100) {
                    trangThai = "Sắp xong...";
                }
                else {
                    trangThai = "Sẵn sàng!";
                }
                DatViTriCon(0, yTrangThai);
                std::cout << le;
                DatMau(MAU_XAM);
                std::cout << CanTrai(trangThai, DO_RONG_KHUNG - DO_RONG_PHAN_TRAM);
                DatMau(MAU_TRANG);
                std::cout << CanPhai(std::to_string(phanTram) + "%", DO_RONG_PHAN_TRAM);
                DatMauMacDinh();
                std::cout.flush();

                soOCu = soOday;
                phanTramCu = phanTram;
            }

            if (troiQua >= thoiGianMs) {
                break;
            }
            Sleep(10);
        }

        //bỏ các phím người dùng lỡ bấm trong lúc loading để không ảnh hưởng menu đầu tiên
        while (_kbhit()) {
            _getch();
        }

        DatMauMacDinh();
        XoaManHinh();
        HienConTro(true);
    }

    //in một dòng thông tin dạng: NHÃN : GIÁ TRỊ (nhãn màu xám, giá trị tô màu tùy ý)
    void InDong(const std::string& nhan, const std::string& giaTri, Mau mauGiaTri) {
        const int doRongGiaTri = DO_RONG_KHUNG - 2 - 1 - DO_RONG_NHAN - 2 - 1;  // trừ 2 viền, 2 dấu cách, ": "
        if (g_trongBangThongTin) {
            std::cout << LeTrai(DO_RONG_KHUNG) << KeDoc() << " ";
            DatMau(MAU_XAM);
            std::cout << CanTrai(nhan, DO_RONG_NHAN) << ": ";
            DatMau(mauGiaTri);
            std::cout << CanTrai(giaTri, doRongGiaTri);
            DatMauMacDinh();
            std::cout << " " << KeDoc() << "\n";
            return;
        }
        // in bên ngoài khung (vd dòng "Số dư mới" sau giao dịch)
        std::cout << LeTrai(DO_RONG_KHUNG) << "  ";
        DatMau(MAU_XAM);
        std::cout << CanTrai(nhan, DO_RONG_NHAN) << ": ";
        DatMau(mauGiaTri);
        std::cout << giaTri;
        DatMauMacDinh();
        std::cout << "\n";
    }

    //ký tự kẻ dọc ║ của bảng, tô màu chủ đạo
    std::string KeDoc() {
        if (!g_coMau24Bit) {
            return KHUNG_DOC;
        }
        int r, g, b;
        LayRGB(MAU_CHU_DAO, r, g, b);
        return MaANSI(38, r, g, b) + KHUNG_DOC + "\x1b[0m";
    }

    //kẻ một đường ngang của bảng danh sách theo độ rộng từng cột: loai 0 = trên ╔╦╗, 1 = giữa ╠╬╣, 2 = dưới ╚╩╝
    void InKeNgangBang(const std::vector<int>& doRongCot, int loai) {
        const char* const TRAI[] = { "╔", "╠", "╚" };
        const char* const GIUA[] = { "╦", "╬", "╩" };
        const char* const PHAI[] = { "╗", "╣", "╝" };

        // tổng độ rộng bảng = 1 viền trái + mỗi cột (độ rộng + 2 dấu cách + 1 viền)
        int tongDoRong = 1;
        for (std::vector<int>::size_type i = 0; i < doRongCot.size(); i++) {
            tongDoRong += doRongCot[i] + 3;
        }

        DatMau(MAU_CHU_DAO);
        std::cout << LeTrai(tongDoRong) << TRAI[loai];
        for (std::vector<int>::size_type i = 0; i < doRongCot.size(); i++) {
            std::cout << LapLai(KHUNG_NGANG, doRongCot[i] + 2);   // +2: khoảng trắng hai bên ô
            std::cout << (i + 1 < doRongCot.size() ? GIUA[loai] : PHAI[loai]);
        }
        DatMauMacDinh();
        std::cout << "\n";
    }

    //mở khung thông tin, các lệnh InDong sau đó in thành dòng trong khung
    void BatDauBangThongTin() {
        DatMau(MAU_CHU_DAO);
        InVienNgang(KHUNG_TREN_TRAI, KHUNG_TREN_PHAI, DO_RONG_KHUNG);
        DatMauMacDinh();
        g_trongBangThongTin = true;
    }

    //đường ngăn cách giữa các nhóm thông tin
    void InNganCach() {
        if (!g_trongBangThongTin) {
            return;
        }
        DatMau(MAU_CHU_DAO);
        InVienNgang(KHUNG_GIUA_TRAI, KHUNG_GIUA_PHAI, DO_RONG_KHUNG);
        DatMauMacDinh();
    }

    //đóng khung thông tin
    void KetThucBangThongTin() {
        DatMau(MAU_CHU_DAO);
        InVienNgang(KHUNG_DUOI_TRAI, KHUNG_DUOI_PHAI, DO_RONG_KHUNG);
        DatMauMacDinh();
        g_trongBangThongTin = false;
    }

    //in ra dòng báo lỗi
    void BaoLoi(const std::string& thongBao) {
        DatMau(MAU_DO);
        std::cout << LeTrai(DO_RONG_KHUNG) << "✖ " << thongBao << "\n";
        DatMauMacDinh();
    }

    //in ra dòng báo thành công
    void BaoThanhCong(const std::string& thongBao) {
        DatMau(MAU_XANH_LA);
        std::cout << LeTrai(DO_RONG_KHUNG) << "✔ " << thongBao << "\n";
        DatMauMacDinh();
    }

    //in ra dòng báo thông tin (lưu ý, cảnh báo, thông tin chung,...)
    void BaoThongTin(const std::string& thongBao) {
        DatMau(MAU_VANG);
        std::cout << LeTrai(DO_RONG_KHUNG) << "● " << thongBao << "\n";
        DatMauMacDinh();
    }


    //nhận chuỗi dữ liệu từ người dùng nhập vào, kiểm tra độ dài, ngăn chặn kí tự ngăn cách
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa) {
        std::string dong;
        //vòng lặp kiểm duyệt liên tục cho tới khi chuỗi hợp lệ (không dài hơn doDaiToiDa, không chứa KY_TU_NGAN_CACH)
        while (DocDong(is, loiNhac, dong)) {
            if (static_cast<int>(dong.size()) > doDaiToiDa) {
                BaoLoi("Tối đa " + std::to_string(doDaiToiDa) + " ký tự.");
                continue;
            }
            if (dong.find(KY_TU_NGAN_CACH) != std::string::npos) {      // Tranh lam hong file du lieu
                BaoLoi(std::string("Không được dùng ký tự '") + KY_TU_NGAN_CACH + "'.");
                continue;
            }
            ketQua = dong;
            return true;
        }
        return false;
    }


    //nhận chuỗi dữ liệu từ người dùng nhập vào, kiểm tra là ký tự số VÀ đúng độ dài (doDai) thì hợp lệ
    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua) {
        std::string dong;
        //vòng lặp kiểm duyệt liên tục cho tới khi chuỗi hợp lệ (toàn là kí tự số và dài đúng bằng doDai)
        while (DocDong(is, loiNhac, dong)) {
            if (!ChiGomChuSo(dong) || static_cast<int>(dong.size()) != doDai) {
                BaoLoi("Phải gồm đúng " + std::to_string(doDai) + " chữ số.");
                continue;
            }
            ketQua = dong;
            return true;
        }
        return false;
    }


    //nhận dữ liệu số nguyên từ người dùng và kiểm tra định dạng chữ số (không âm, tối đa 15 chữ số) và không vượt quá giới hạn (giaTriNhoNhat, giaTriLonNhat)
    bool NhapSoNguyen(std::istream& is, const std::string& loiNhac, long long& ketQua,
        long long giaTriNhoNhat, long long giaTriLonNhat) {
        std::string dong;
        while (DocDong(is, loiNhac, dong)) {
            if (!ChiGomChuSo(dong) || static_cast<int>(dong.size()) > SO_CHU_SO_TOI_DA) {
                BaoLoi("Chỉ được nhập số nguyên không âm (tối đa 15 chữ số).");
                continue;
            }
            const long long giaTri = std::stoll(dong);
            if (giaTri < giaTriNhoNhat || giaTri > giaTriLonNhat) {
                BaoLoi("Giá trị phải từ " + DinhDangTien(giaTriNhoNhat) + " đến " + DinhDangTien(giaTriLonNhat) + ".");
                continue;
            }
            ketQua = giaTri;
            return true;
        }
        return false;
    }


    //nhận dữ liệu số thực từ người dùng và kiểm tra định dạng chữ số (không âm, tối đa 15 chữ số) và không vượt quá giới hạn (giaTriNhoNhat, giaTriLonNhat)
    bool NhapSoThuc(std::istream& is, const std::string& loiNhac, double& ketQua, double giaTriNhoNhat, double giaTriLonNhat) {
        std::string dong;
        while (DocDong(is, loiNhac, dong)) {
            double giaTri = 0.0;
            if (!DocSoThuc(dong, giaTri)) {
                BaoLoi("Chỉ được nhập số (ví dụ 5.5).");
                continue;
            }
            if (giaTri < giaTriNhoNhat || giaTri > giaTriLonNhat) {
                BaoLoi("Giá trị phải từ " + SoThucSangChuoi(giaTriNhoNhat) + " đến " + SoThucSangChuoi(giaTriLonNhat) + ".");
                continue;
            }
            ketQua = giaTri;
            return true;
        }
        return false;
    }


    //nhập số tiền trong 1 khung, dùng cho màn hình nạp / rút tiền
    bool NhapSoTienTrongKhung(const std::string& loiNhac, long long& ketQua,
        long long giaTriNhoNhat, long long giaTriLonNhat) {
        const std::string le = LeTrai(DO_RONG_KHUNG);
        const int xKhung = static_cast<int>(le.size());                 // cột của viền trái │
        const int xNhap = xKhung + 2 + DoRongHienThi(loiNhac);          // cột bắt đầu gõ số (sau "│ " và lời nhắc)
        const int y = LayDongHienTai();                                  // dòng của viền trên ┌

        while (true) {
            // Bước 2: vẽ khung
            DatViTriCon(0, y);
            DatMau(MAU_CHU_DAO);
            std::cout << le << "┌" << LapLai("─", DO_RONG_KHUNG - 2) << "┐\n";
            std::cout << le << "│" << std::string(DO_RONG_KHUNG - 2, ' ') << "│\n";
            std::cout << le << "└" << LapLai("─", DO_RONG_KHUNG - 2) << "┘\n";
            DatViTriCon(xKhung + 2, y + 1);
            DatMau(MAU_TRANG);
            std::cout << loiNhac;
            DatMau(MAU_XANH_LA);            // số tiền người dùng gõ có màu xanh lá

            std::string dong;
            if (!std::getline(std::cin, dong)) {
                DatMauMacDinh();
                return false;
            }
            DatMauMacDinh();
            dong = CatKhoangTrang(dong);

            // xóa dòng báo lỗi cũ (nếu có) rồi để con trỏ ở đầu dòng đó
            DatViTriCon(0, y + 3);
            std::cout << std::string(LayDoRongConsole() - 1, ' ');
            DatViTriCon(0, y + 3);

            if (dong.empty()) {
                return false;               // để trống = hủy giao dịch
            }

            // bỏ dấu chấm phân cách hàng nghìn mà người dùng có thể gõ
            std::string chuSo;
            for (std::string::size_type i = 0; i < dong.size(); i++) {
                if (dong[i] != '.') {
                    chuSo += dong[i];
                }
            }

            // Bước 3: kiểm tra
            if (!ChiGomChuSo(chuSo) || static_cast<int>(chuSo.size()) > SO_CHU_SO_TOI_DA) {
                BaoLoi("Chỉ được nhập số nguyên dương (tối đa 15 chữ số).");
                continue;
            }
            const long long giaTri = std::stoll(chuSo);
            if (giaTri < giaTriNhoNhat || giaTri > giaTriLonNhat) {
                BaoLoi("Số tiền phải từ " + DinhDangTien(giaTriNhoNhat) + " đến " + DinhDangTien(giaTriLonNhat) + " VND.");
                continue;
            }

            // Bước 4: in lại số đã định dạng, phần thừa tô bằng dấu cách để không đè lên viền phải
            DatViTriCon(xNhap, y + 1);
            DatMau(MAU_XANH_LA);
            std::cout << CanTrai(DinhDangTien(giaTri), DO_RONG_KHUNG - 3 - DoRongHienThi(loiNhac));
            DatMau(MAU_CHU_DAO);
            std::cout << "│";
            DatMauMacDinh();
            DatViTriCon(0, y + 3);
            ketQua = giaTri;
            return true;
        }
    }

    //hỏi xác nhận giao dịch: Enter = xác nhận, Esc = hủy
    bool XacNhanGiaoDich() {
        const std::string huongDan = "[Enter]: Xác nhận giao dịch    |    [Esc]: Hủy giao dịch";
        DatMau(MAU_VANG);
        std::cout << "\n" << LeTrai(DoRongHienThi(huongDan)) << huongDan << "\n\n";
        DatMauMacDinh();
        HienConTro(false);
        Phim phim = DocPhim();
        while (phim != PHIM_ENTER && phim != PHIM_ESC) {
            phim = DocPhim();
        }
        HienConTro(true);
        return phim == PHIM_ENTER;
    }


    //chuẩn hóa một giá trị sotien ngời dùng nhập vào thành chuỗi tiền tệ được định dạng (có dấu chấm phân cách hàng nghìn) và xử lí giá trị âm
    std::string DinhDangTien(long long soTien) {
        const bool laSoAm = soTien < 0;

        //toán tử ba ngôi để lấy giá trị tuyệt đối của sotien
        const unsigned long long triTuyetDoi = laSoAm ? 0ULL - static_cast<unsigned long long>(soTien) : static_cast<unsigned long long>(soTien);

        //chuyển số thành chuỗi
        const std::string cacChuSo = std::to_string(triTuyetDoi);
        const int doDai = static_cast<int>(cacChuSo.size());


        std::string ketQua;
        //chèn dấu chấm phân cách hàng nghìn
        for (int i = 0; i < doDai; i++) {
            ketQua += cacChuSo[i];
            const int conLai = doDai - 1 - i;
            if (conLai > 0 && conLai % NHOM_HANG_NGAN == 0) {
                ketQua += '.';
            }
        }
        return laSoAm ? "-" + ketQua : ketQua;
    }


    //chuyển đổi một số thực sang dạng phầm trăm (ví dụ: 3.6 (double) -> 3.6% (string) )
    std::string DinhDangPhanTram(double giaTri) {
        return SoThucSangChuoi(giaTri) + "%";
    }


    //cắt bỏ khoảng trắng thừa ở 2 dầu chuỗi
    std::string CatKhoangTrang(const std::string& chuoi) {
        const std::string khoangTrang = " \t\r\n";                                  // -t-r-n: dấu cách, tab, kí tự về đầu dòng, kí tự xuống dòng

        //tìm vị trí kí tự hợp lệ đầu tiên bên trái
        const std::string::size_type dau = chuoi.find_first_not_of(khoangTrang);
        if (dau == std::string::npos) {
            return "";
        }
        //tìm vị trí kí tự hợp lệ
        const std::string::size_type cuoi = chuoi.find_last_not_of(khoangTrang);

        //trả về chuỗi chuẩn hóa
        return chuoi.substr(dau, cuoi - dau + 1);
    }

    //chuyển đổi toàn bộ ký tự trong một chuỗi thành chữ in hoa
    std::string ChuyenInHoa(const std::string& chuoi) {
        std::string ketQua = chuoi;                 //tạo bản sao để xử lí an toàn, không làm thay đổi chuỗi gốc

        for (std::string::size_type i = 0; i < ketQua.size(); i++) {
            //lớp trong (std::toupper(static_cast<unsigned char>) ): ép kiểu thành unsigned char rồi toupper để chuyển thành in hoa
            //lớp ngoài (static_cast<char> ): ép kiểu thành char
            ketQua[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(ketQua[i])));
        }
        return ketQua;
    }
}