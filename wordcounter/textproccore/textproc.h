#ifndef __TEXTPROC_H
#define __TEXTPROC_H

#include <vector>
#include <set>
#include <iostream>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

class TextProc
{
public:
    enum Status
    {
        S_DEFAULT = 0,
        S_PREFIN, S_ERR_PREFIN, S_OUT_OF_MEMORY,
        S_READ, S_READY,
        S_INVALID = 0xd800, S_READ_END
    };

    enum Encoding { E_8BIT, E_UTF8, E_UTF16LE, E_UTF16BE };

    enum MachState
    {
        MS_0_START, MS_1_WORD1, MS_2_WORD2, MS_3, MS_4_SPACER, MS_5, MS_6_NEWLINE,
        MS_7_UNUSED, MS_8_INVALID, MS_9, MS_10, MS_11, MS_12_END, MS_13, MS_14
    };

    typedef std::shared_ptr<std::istream> SP_IStream;
    typedef std::function<bool(uint32_t)> F_BoolUint32;
    typedef std::function<uint32_t(uint32_t)> F_Uint32Uint32;
    typedef std::function<void(void *)> F_VoidVoidP;

    struct EncParams
    {
        Encoding m_enc;
        F_BoolUint32 m_alpha, m_isLower;
        F_Uint32Uint32 m_toLower;
    };

    enum StatIndex
    {
        SI_UNIQ_COUNT = 0, SI_LINE_COUNT, SI_LINE_COUNT_OVERFLOW, SI_WORD_COUNT, SI_WORD_COUNT_OVERFLOW
    };

    typedef unsigned char td_uchar;

private:
    struct SharedData
    {
        std::atomic_bool m_shutdown;
        std::atomic<Status> m_proc, m_read;
        size_t m_sub1read, m_sub2read;
        std::vector<char> m_sub1, m_sub2;
        Status m_state1, m_state2;
        SP_IStream m_file;
        std::mutex m_sub1mtx, m_sub2mtx;
        std::condition_variable m_sub1cvProc, m_sub1cvRead, m_sub2cvProc, m_sub2cvRead;
        SharedData(size_t a_subBufSize, const SP_IStream &a_file);
    };

    const size_t s_shutdownOvercount = 1000;

public:
    TextProc(size_t a_subBufSize, size_t a_wordBufSize, EncParams &a_encParams, const SP_IStream &a_file, F_VoidVoidP &a_done);
    ~TextProc();

    TextProc(const TextProc &) = delete;
    TextProc &operator =(const TextProc &) = delete;

    std::vector<size_t> notSyncStat() const;

private:
    bool alpha(uint32_t a) { return m_encParams.m_alpha(a); }
    bool newline(uint32_t a) { return a == '\r' || a == '\n'; }
    bool invalid(uint32_t a) { return (m_encParams.m_enc == E_UTF8 ||
        m_encParams.m_enc == E_UTF16LE || m_encParams.m_enc == E_UTF16BE) && a == S_INVALID; }

    uint32_t read8bit();
    uint32_t readUtf8();
    uint32_t readUtf16le();
    uint32_t readUtf16be();
    uint32_t readSym()
    {
        if (m_encParams.m_enc == E_UTF8) return readUtf8();
        if (m_encParams.m_enc == E_UTF16LE) return readUtf16le();
        if (m_encParams.m_enc == E_UTF16BE) return readUtf16be();
        return read8bit();
    }

    void fix8bit();
    void fixUtf8();
    void fixUtf16le();
    void fixUtf16be();
    void fixSym()
    {
        if (m_encParams.m_enc == E_UTF8) fixUtf8();
        else if (m_encParams.m_enc == E_UTF16LE) fixUtf16le();
        else if (m_encParams.m_enc == E_UTF16BE) fixUtf16be();
        else fix8bit();
    }

    void initWord();
    void word();
    void lineCountConveniency() { m_lineCount++; if (m_lineCount == 0) m_lineCountOverflow++; }
    void wordCountConveniency() { m_wordCount++; if (m_wordCount == 0) m_wordCountOverflow++; }

    void action();
    void transition();
    void machine();     //mach.jpg
    void procX();

    char nextByte();
    void switchSub();

    void proc();
    bool readXoe(const std::shared_ptr<SharedData> &a_sharedDataSP, size_t a_switch);
    void readX(const std::shared_ptr<SharedData> &a_sharedDataSP);
    void read(const std::shared_ptr<SharedData> &a_sharedDataSP);

    std::atomic_size_t m_uniqCount;
    std::atomic_size_t m_lineCount, m_lineCountOverflow;
    std::atomic_size_t m_wordCount, m_wordCountOverflow;
    std::set<std::vector<char>> m_dict;

    EncParams m_encParams;

    uint32_t m_sym;
    char *m_wordFix, *m_wordSkip, *m_wordBufSkip;
    std::vector<char> m_word;
    char *const m_wordBufBegin, *const m_wordBufClose;

    MachState m_machState;

    size_t m_switch;
    char *m_start, *m_skip;
    bool m_endOfData;

    F_VoidVoidP m_done;
    std::shared_ptr<SharedData> m_sharedDataSP;
    std::unique_lock<std::mutex> m_procUl;
    std::thread m_proc, m_read;
};

#endif //__TEXTPROC_H
