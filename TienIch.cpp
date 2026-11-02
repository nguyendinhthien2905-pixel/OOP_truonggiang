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
    const int DO_RONG_KHUNG = 60;           // Độ rộng khung tiêu đề
    const int DO_RONG_MUC_MENU = 56;        // độ rộng 1 dòng menu
    const int SO_MUC_HIEN_THI_TOI_DA = 10;  // số lượng mục menu tối đa, dài hơn sẽ tự cuộn
    const int DO_RONG_NHAN = 28;            // độ rộng của cột nhãn
    const int DO_RONG_GIA_TRI = 34;         // độ rộng của cột giá trị trong bảng thông tin
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
    bool g_trongBangThongTin = false;       // InDong đang in vào bảng thông tin 2 cột hay không

    //các ký tự khung viền (viết dạng byte UTF-8 để không phụ thuộc bảng mã khi biên dịch)
    const char* const KHUNG_TREN_TRAI = "\xE2\x95\xAD";    // ╭
    const char* const KHUNG_TREN_PHAI = "\xE2\x95\xAE";    // ╮
    const char* const KHUNG_DUOI_TRAI = "\xE2\x95\xB0";    // ╰
    const char* const KHUNG_DUOI_PHAI = "\xE2\x95\xAF";    // ╯
    const char* const KHUNG_NGANG = "\xE2\x94\x80";        // ─
    const char* const KHUNG_DOC = "\xE2\x94\x82";          // │
    const char* const MUI_TEN_CHON = "\xE2\x96\xBA";       // ►

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
        case '#': return "\xE2\x96\x88";
        case '[': return "\xE2\x95\x94";
        case ']': return "\xE2\x95\x97";
        case '{': return "\xE2\x95\x9A";
        case '}': return "\xE2\x95\x9D";
        case '-': return "\xE2\x95\x90";
        case '|': return "\xE2\x95\x91";
        default:  return " ";
        }
    }

    //bảng màu RGB tương ứng với từng mã màu
    void LayRGB(Mau mau, int& r, int& g, int& b) {
        switch (mau) {
        case MAU_DEN:        r = 24;  g = 24;  b = 24;  break;
        case MAU_XAM:        r = 140; g = 140; b = 140; break;
        case MAU_CHU_DAO:    r = 236; g = 128; b = 62;  break;   // #ec803e (cam sáng)
        case MAU_XANH_DUONG: r = 97;  g = 175; b = 239; break;
        case MAU_XANH_LA:    r = 152; g = 195; b = 121; break;
        case MAU_DO:         r = 224; g = 108; b = 117; break;
        case MAU_TIM:        r = 198; g = 120; b = 221; break;
        case MAU_VANG:       r = 229; g = 192; b = 123; break;
        default:             r = 230; g = 230; b = 230; break;
        }
    }

    //sinh mã ANSI màu chữ (loai = 38) hoặc màu nền (loai = 48)
    std::string MaANSI(int loai, int r, int g, int b) {
        return "\x1b[" + std::to_string(loai) + ";2;" + std::to_string(r) + ";"
            + std::to_string(g) + ";" + std::to_string(b) + "m";
    }

    //in một dòng khung bo góc: ╭──────╮ hoặc ╰──────╯
    void InVienNgang(const char* trai, const char* phai, int doRong) {
        std::cout << "  " << trai;
        for (int i = 0; i < doRong - 2; i++) {
            std::cout << KHUNG_NGANG;
        }
        std::cout << phai << "\n";
    }


    //thiết lập menu tương tác (không có thì chỉ là console bình thường)
    HANDLE LayManHinh() {
        return GetStdHandle(STD_OUTPUT_HANDLE);
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
        std::cout << loiNhac << std::flush;
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
    void VeMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc, int viTri, int dau, int soDong) {
        BaoVeDinhDang baoVe(std::cout);
        const int soMuc = static_cast<int>(cacMuc.size());

        TienIch::DatViTriCon(0, 0); //đưa con trỏ về góc trên cùng bên trái (x=0,y=0) để khi vẽ lại thì không đè lên các lệnh của màn hình trước
        TienIch::InTieuDe(tieuDe);

        //in thanh hướng dẫn lên/xuống/enter...
        std::cout << std::left;
        TienIch::DatMau(MAU_XAM);
        std::cout << std::setw(DO_RONG_KHUNG)
            << "  Len/Xuong: di chuyen  -  Enter: chon  -  Esc: quay lai" << "\n";
        std::cout << std::setw(DO_RONG_KHUNG) << "" << "\n";

        TienIch::DatMau(MAU_CHU_DAO);
        //kiểm tra danh sách có bị cuộc xuống dưới hay không, nếu có in ra kí tự ^... để người dùng biết
        std::cout << std::setw(DO_RONG_MUC_MENU) << (dau > 0 ? "        ^ ..." : "") << "\n";

        //vòng lặp hiển thị danh sách menu
        for (int i = dau; i < dau + soDong; i++) {
            std::string noiDung = cacMuc[i];
            if (static_cast<int>(noiDung.size()) > DO_RONG_MUC_MENU - 4) {
                noiDung = noiDung.substr(0, DO_RONG_MUC_MENU - 4);  //cắt bớt nếu nội dung dài tránh làm tràn khung giao diện
            }
            //xử lí vị trí đang chọn: đổi màu nền xanh ngọc, màu chữ đen và hiển thị >> trước nội dung đang chọn
            if (i == viTri) {
                std::cout << "  ";
                TienIch::DatMauNen(MAU_TRANG, MAU_CHU_DAO);
                std::cout << " " << MUI_TEN_CHON << std::setw(DO_RONG_MUC_MENU - 4) << (" " + noiDung);
                TienIch::DatMauMacDinh();
            }
            //xử lí vị trí không đang được chọn: màu chữ trắng, thụt lề bình thường
            else {
                TienIch::DatMau(MAU_TRANG);
                std::cout << std::setw(DO_RONG_MUC_MENU) << ("    " + noiDung);
            }
            TienIch::DatMauMacDinh();
            std::cout << "\n";
        }

        TienIch::DatMau(MAU_CHU_DAO);
        //báo hiệu cuộn xuống v.. khi còn các mục phía dưới
        std::cout << std::setw(DO_RONG_MUC_MENU) << (dau + soDong < soMuc ? "        v ..." : "") << "\n";
        TienIch::DatMauMacDinh();
        //in lập tức để tránh giật màn hình hiển thị
        std::cout.flush();
    }

} // namespace

namespace TienIch {

    //khởi tạo màn hình console
    void KhoiTaoConsole() {
        std::system("mode con: cols=120 lines=36");                     //thiết đặt kích thước màn hình console: 120 cột và 36 dòng
        SetConsoleTitleA("QUAN LY TAI KHOAN NGAN HANG - MAY ATM");      //thiết đặt tiêu đề cho màn hình console
        CONSOLE_SCREEN_BUFFER_INFO thongTin;                            //thiết đặt màu săc mặc định cho hệ thống
        if (GetConsoleScreenBufferInfo(LayManHinh(), &thongTin)) {
            g_mauMacDinh = thongTin.wAttributes;
        }
        SetConsoleOutputCP(CP_UTF8);                                    //in được ký tự khung viền, chữ khối
        DWORD cheDo = 0;                                                //bật mã ANSI để dùng màu 24-bit (#b4532a)
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
        DatMau(MAU_VANG);
        std::cout << "\n  Nhan phim bat ky de tiep tuc...";
        DatMauMacDinh();
        std::cout.flush();
        DocPhim();
    }

    //controller chính điều khiển giao diện cửa sổ console
    int ChonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc) {
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
            VeMenu(tieuDe, cacMuc, viTri, dau, soDong);


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

    //vẽ một khung tiêu đề có đườnng viền bao quanh, canh giữa dòng tiêu đề trên màn hình console
    void InTieuDe(const std::string& tieuDe) {
        //phép tính để căn lề giữa
        int khoangTrong = DO_RONG_KHUNG - 2 - static_cast<int>(tieuDe.size());
        if (khoangTrong < 0) {
            khoangTrong = 0;
        }

        const int leTrai = khoangTrong / 2;
        const int lePhai = khoangTrong - leTrai;

        //chữ khối "ATM BANK", màu chuyển dần từ vàng (#f7c948) sang cam (#ec803e) theo từng cột
        std::cout << "\n";
        for (int dong = 0; dong < SO_DONG_BANNER; dong++) {
            const std::string hang = BANNER[dong];
            const int doRong = static_cast<int>(hang.size());
            std::cout << "  ";
            for (int cot = 0; cot < doRong; cot++) {
                if (g_coMau24Bit) {
                    const int r = 247 + (236 - 247) * cot / doRong;
                    const int g = 201 + (128 - 201) * cot / doRong;
                    const int b = 72 + (62 - 72) * cot / doRong;
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

        DatMau(MAU_XAM);
        std::cout << "  Quan ly tai khoan ngan hang - May ATM\n\n";

        //khung bo góc chứa tiêu đề màn hình hiện tại
        DatMau(MAU_CHU_DAO);
        InVienNgang(KHUNG_TREN_TRAI, KHUNG_TREN_PHAI, DO_RONG_KHUNG);
        std::cout << "  " << KHUNG_DOC;
        DatMau(MAU_TRANG);
        std::cout << std::string(leTrai, ' ') << tieuDe << std::string(lePhai, ' ');    //tiêu đề ở giữa màu trắng
        DatMau(MAU_CHU_DAO);
        std::cout << KHUNG_DOC << "\n";
        InVienNgang(KHUNG_DUOI_TRAI, KHUNG_DUOI_PHAI, DO_RONG_KHUNG);

        DatMauMacDinh();
    }

    //in một dòng thông tin dạng: THÔNG_TIN: GIÁ_TRỊ, được căn lề trái, thông tin được tô màu xanh ngọc, giá trị được tô màu tùy ý
    //nếu đang ở trong bảng thông tin (BatDauBangThongTin) thì in thành một hàng của bảng 2 cột
    void InDong(const std::string& nhan, const std::string& giaTri, Mau mauGiaTri) {
        BaoVeDinhDang baoVe(std::cout);
        if (g_trongBangThongTin) {
            std::cout << "  " << KeDoc() << " ";
            DatMau(MAU_CHU_DAO);
            std::cout << std::left << std::setw(DO_RONG_NHAN) << nhan;
            DatMauMacDinh();
            std::cout << " " << KeDoc() << " ";
            DatMau(mauGiaTri);
            std::cout << std::setw(DO_RONG_GIA_TRI) << giaTri;
            DatMauMacDinh();
            std::cout << " " << KeDoc() << "\n";
            return;
        }
        DatMau(MAU_CHU_DAO);
        std::cout << "  " << std::left << std::setw(DO_RONG_NHAN) << nhan << ": ";
        DatMau(mauGiaTri);
        std::cout << giaTri;
        DatMauMacDinh();
        std::cout << "\n";
    }

    //ký tự kẻ dọc │ của bảng, tô màu chủ đạo
    std::string KeDoc() {
        if (!g_coMau24Bit) {
            return KHUNG_DOC;
        }
        int r, g, b;
        LayRGB(MAU_CHU_DAO, r, g, b);
        return MaANSI(38, r, g, b) + KHUNG_DOC + "\x1b[0m";
    }

    //kẻ một đường ngang của bảng theo độ rộng từng cột: loai 0 = trên ┌┬┐, 1 = giữa ├┼┤, 2 = dưới └┴┘
    void InKeNgangBang(const std::vector<int>& doRongCot, int loai) {
        const char* const TRAI[] = { "\xE2\x94\x8C", "\xE2\x94\x9C", "\xE2\x94\x94" };
        const char* const GIUA[] = { "\xE2\x94\xAC", "\xE2\x94\xBC", "\xE2\x94\xB4" };
        const char* const PHAI[] = { "\xE2\x94\x90", "\xE2\x94\xA4", "\xE2\x94\x98" };

        DatMau(MAU_CHU_DAO);
        std::cout << "  " << TRAI[loai];
        for (std::vector<int>::size_type i = 0; i < doRongCot.size(); i++) {
            for (int j = 0; j < doRongCot[i] + 2; j++) {   // +2: khoảng trắng hai bên ô
                std::cout << KHUNG_NGANG;
            }
            std::cout << (i + 1 < doRongCot.size() ? GIUA[loai] : PHAI[loai]);
        }
        DatMauMacDinh();
        std::cout << "\n";
    }

    //mở bảng thông tin 2 cột: các lệnh InDong sau đó sẽ in thành hàng của bảng
    void BatDauBangThongTin() {
        std::vector<int> cot;
        cot.push_back(DO_RONG_NHAN);
        cot.push_back(DO_RONG_GIA_TRI);
        InKeNgangBang(cot, 0);
        BaoVeDinhDang baoVe(std::cout);
        std::cout << std::left << "  " << KeDoc() << " " << std::setw(DO_RONG_NHAN) << "Thong tin"
            << " " << KeDoc() << " " << std::setw(DO_RONG_GIA_TRI) << "Gia tri" << " " << KeDoc() << "\n";
        InKeNgangBang(cot, 1);
        g_trongBangThongTin = true;
    }

    //đóng bảng thông tin 2 cột
    void KetThucBangThongTin() {
        std::vector<int> cot;
        cot.push_back(DO_RONG_NHAN);
        cot.push_back(DO_RONG_GIA_TRI);
        InKeNgangBang(cot, 2);
        g_trongBangThongTin = false;
    }

    //in ra dòng báo lỗi
    void BaoLoi(const std::string& thongBao) {
        DatMau(MAU_DO);
        std::cout << "  \xE2\x9C\x96 " << thongBao << "\n";
        DatMauMacDinh();
    }

    //in ra dòng báo thành công
    void BaoThanhCong(const std::string& thongBao) {
        DatMau(MAU_XANH_LA);
        std::cout << "  \xE2\x9C\x94 " << thongBao << "\n";
        DatMauMacDinh();
    }

    //in ra dòng báo thông tin (lưu ý, cảnh báo, thông tin chung,...)
    void BaoThongTin(const std::string& thongBao) {
        DatMau(MAU_VANG);
        std::cout << "  \xE2\x97\x8F " << thongBao << "\n";
        DatMauMacDinh();
    }


    //nhận chuỗi dữ liệu từ người dùng nhập vào, kiểm tra độ dài, ngăn chặn kí tự ngăn cách
    bool NhapChuoi(std::istream& is, const std::string& loiNhac, std::string& ketQua, int doDaiToiDa) {
        std::string dong;
        //vòng lặp kiểm duyệt liên tục cho tới khi chuỗi hợp lệ (không dài hơn doDaiToiDa, không chứa KY_TU_NGAN_CACH)
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


    //nhận chuỗi dữ liệu từ người dùng nhập vào, kiểm tra là ký tự số VÀ đúng độ dài (doDai) thì hợp lệ
    bool NhapChuoiSo(std::istream& is, const std::string& loiNhac, int doDai, std::string& ketQua) {
        std::string dong;
        //vòng lặp kiểm duyệt liên tục cho tới khi chuỗi hợp lệ (toàn là kí tự số và dài đúng bằng doDai)
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


    //nhận dữ liệu số nguyên từ người dùng và kiểm tra định dạng chữ số (không âm, tối đa 15 chữ số) và không vượt quá giới hạn (giaTriNhoNhat, giaTriLonNhat)
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
                BaoLoi("Gia tri phai tu " + DinhDangTien(giaTriNhoNhat) + " den " + DinhDangTien(giaTriLonNhat) + ".");
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
                BaoLoi("Chi duoc nhap so (vi du 5.5).");
                continue;
            }
            if (giaTri < giaTriNhoNhat || giaTri > giaTriLonNhat) {
                BaoLoi("Gia tri phai tu " + SoThucSangChuoi(giaTriNhoNhat) + " den " + SoThucSangChuoi(giaTriLonNhat) + ".");
                continue;
            }
            ketQua = giaTri;
            return true;
        }
        return false;
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