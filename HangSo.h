#ifndef HANGSO_H_INCLUDED_
#define HANGSO_H_INCLUDED_

// ---------------------------- Hang so nghiep vu ---------------------------
const long long BUOC_TIEN = 50000;                      // Moi lan rut phai la boi so cua 50.000
const long long SO_DU_TOI_DA = 999999999999999LL;       // 15 chu so
const long long SO_DU_TOI_THIEU_TIET_KIEM = 100000;     // Tai khoan tiet kiem phai giu lai
const long long HAN_MUC_RUT_TIET_KIEM = 20000000;       // Toi da moi lan rut (tiet kiem)
const long long HAN_MUC_TIN_DUNG_TOI_DA = 1000000000LL; // 1 ty
const double LAI_SUAT_TOI_THIEU = 0.1;                  // %/nam
const double LAI_SUAT_TOI_DA = 20.0;                    // %/nam

// ---------------------------- File du lieu --------------------------------
const char* const TEN_FILE_DU_LIEU = "TaiKhoan.txt";    // Nam canh file .exe
const char* const LOAI_FILE_TIET_KIEM = "TIETKIEM";
const char* const LOAI_FILE_THANH_TOAN = "THANHTOAN";
const char KY_TU_NGAN_CACH = '|';

// ---------------------------- Rang buoc nhap lieu -------------------------
const int DO_DAI_SO_TAI_KHOAN = 10;
const int DO_DAI_TEN_TOI_DA = 24;

// ---------------------------- Do rong cot cua bang ------------------------
const int DO_RONG_COT_STT = 3;
const int DO_RONG_COT_SO_TAI_KHOAN = 12;
const int DO_RONG_COT_TEN = DO_DAI_TEN_TOI_DA;
const int DO_RONG_COT_LOAI = 10;
const int DO_RONG_COT_SO_DU = 19;
const int DO_RONG_COT_THONG_SO = 22;

#endif
