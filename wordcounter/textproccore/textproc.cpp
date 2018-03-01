#include "stdafx.h"
#include "textproc.h"

TextProc::SharedData::SharedData(size_t a_subBufSize, const SP_IStream &a_file) :
    m_shutdown(false), m_proc(S_DEFAULT), m_read(S_DEFAULT),
    m_sub1read(0), m_sub2read(0),
    m_sub1(a_subBufSize), m_sub2(a_subBufSize),
    m_state1(S_DEFAULT), m_state2(S_DEFAULT),
    m_file(a_file)
{
    assert(m_file != nullptr);
}

TextProc::TextProc(size_t a_subBufSize, size_t a_wordBufSize, EncParams &a_encParams, const SP_IStream &a_file, F_VoidVoidP &a_done) :
    m_uniqCount(0), m_lineCount(0), m_lineCountOverflow(0), m_wordCount(0), m_wordCountOverflow(0),
    m_encParams(std::move(a_encParams)),
    m_word(a_wordBufSize), m_wordBufBegin(m_word.data()), m_wordBufClose(m_word.data() + m_word.size()),
    m_switch(0), m_start(nullptr), m_skip(nullptr), m_endOfData(false),
    m_done(std::move(a_done)), m_sharedDataSP(std::make_shared<SharedData>(a_subBufSize, a_file)),
    m_proc(&TextProc::proc, this)
{
    assert(a_subBufSize > 0 && a_wordBufSize > 0);
    assert(m_encParams.m_alpha != nullptr && m_encParams.m_isLower != nullptr && m_encParams.m_toLower != nullptr);
    try { m_read = std::thread(&TextProc::read, this, m_sharedDataSP); }
    catch (const std::exception &) { m_proc.join(); throw; }
}

TextProc::~TextProc()
{
    assert(m_sharedDataSP != nullptr);
    m_sharedDataSP->m_shutdown = true;
    m_sharedDataSP->m_sub1cvProc.notify_one();
    m_sharedDataSP->m_sub1cvRead.notify_one();
    m_sharedDataSP->m_sub2cvProc.notify_one();
    m_sharedDataSP->m_sub2cvRead.notify_one();
    m_proc.join();
    m_read.detach();
}

std::vector<size_t> TextProc::notSyncStat() const
{
    return std::vector<size_t>{ m_uniqCount, m_lineCount, m_lineCountOverflow, m_wordCount, m_wordCountOverflow };
}

void TextProc::initWord()
{
    m_wordFix = m_wordSkip = m_wordBufBegin; m_wordBufSkip = m_wordBufClose;
}

void TextProc::word()
{
    const std::vector<char> l_word(m_word.data(), m_wordFix);
    m_wordSkip = m_word.data(); m_wordBufSkip = m_word.data() + m_word.size();
    m_dict.insert(l_word);
    m_uniqCount = m_dict.size(); wordCountConveniency();
}

void TextProc::action()
{
    switch (m_machState)
    {
    case MS_0_START:
        initWord(); m_sym = readSym(); break;
    case MS_1_WORD1:
        fixSym(); break;
    case MS_2_WORD2:
        m_sym = readSym(); break;
    case MS_3:
        word(); break;
    case MS_4_SPACER:
        initWord(); m_sym = readSym(); break;
    case MS_5:
        word(); break;
    case MS_6_NEWLINE:
        lineCountConveniency(); break;
    case MS_7_UNUSED:
    case MS_8_INVALID:
        initWord(); m_sym = readSym(); break;
    case MS_9:
        word(); break;
    case MS_10:
        word(); break;
    case MS_11:
        lineCountConveniency(); break;
    case MS_12_END:
        break;
    case MS_13:
        initWord(); m_sym = readSym(); break;
        break;
    case MS_14:
        initWord(); m_sym = readSym(); break;
        break;
    }
}

void TextProc::transition()
{
    switch (m_machState)
    {
    case MS_0_START:
        if (m_endOfData) m_machState = MS_12_END;
        else if (alpha(m_sym)) m_machState = MS_1_WORD1;
        else if (newline(m_sym)) m_machState = MS_6_NEWLINE;
        else if (invalid(m_sym)) m_machState = MS_8_INVALID;
        else m_machState = MS_4_SPACER; break;
    case MS_1_WORD1:
        m_machState = MS_2_WORD2; break;
    case MS_2_WORD2:
        if (m_endOfData) m_machState = MS_10;
        else if (alpha(m_sym)) m_machState = MS_1_WORD1;
        else if (newline(m_sym)) m_machState = MS_5;
        else if (invalid(m_sym)) m_machState = MS_9;
        else m_machState = MS_3; break;
    case MS_3:
        m_machState = MS_4_SPACER; break;
    case MS_4_SPACER:
        if (m_endOfData) m_machState = MS_11;
        else if (alpha(m_sym)) m_machState = MS_1_WORD1;
        else if (newline(m_sym)) m_machState = MS_6_NEWLINE;
        else if (invalid(m_sym)) m_machState = MS_8_INVALID;
        break;
    case MS_5:
        m_machState = MS_6_NEWLINE; break;
    case MS_6_NEWLINE:
        assert(!m_endOfData && newline(m_sym));
        if (m_sym == '\r') m_machState = MS_13;
        else m_machState = MS_14;
        break;
    case MS_7_UNUSED:
    case MS_8_INVALID:
        if (m_endOfData) m_machState = MS_11;
        else if (alpha(m_sym)) m_machState = MS_1_WORD1;
        else if (newline(m_sym)) m_machState = MS_6_NEWLINE;
        else if (invalid(m_sym));
        else m_machState = MS_4_SPACER; break;
    case MS_9:
        m_machState = MS_8_INVALID; break;
    case MS_10:
        m_machState = MS_11; break;
    case MS_11:
        m_machState = MS_12_END; break;
    case MS_12_END:
        break;
    case MS_13:
        if (m_endOfData) m_machState = MS_12_END;
        else if (alpha(m_sym)) m_machState = MS_1_WORD1;
        else if (m_sym == '\n') m_machState = MS_14;
        else if (newline(m_sym)) m_machState = MS_6_NEWLINE;
        else if (invalid(m_sym)) m_machState = MS_8_INVALID;
        else m_machState = MS_4_SPACER; break;
        break;
    case MS_14:
        if (m_endOfData) m_machState = MS_12_END;
        else if (alpha(m_sym)) m_machState = MS_1_WORD1;
        else if (newline(m_sym)) m_machState = MS_6_NEWLINE;
        else if (invalid(m_sym)) m_machState = MS_8_INVALID;
        else m_machState = MS_4_SPACER; break;
        break;
    }
}

void TextProc::procX()
{
    m_machState = MS_0_START;
    for (size_t q = 0; m_machState != MS_12_END; q++)
    {
        machine();
        if (q < s_shutdownOvercount) continue;
        q = 0;
        if (m_sharedDataSP->m_shutdown) break;
    }
}

void TextProc::machine()
{
    action();
    transition();
}

char TextProc::nextByte()
{
    if (m_start < m_skip) return *m_start++;
    if (m_endOfData) return 0;
    switchSub();
    if (m_start >= m_skip) { m_endOfData = true; return 0; }
    return *m_start++;
}

void TextProc::switchSub()
{
    assert(m_sharedDataSP != nullptr);
    assert(m_sharedDataSP->m_sub1.size() > 0 && m_sharedDataSP->m_sub2.size() > 0);
    if (m_switch % 2 == 0)
    {
        if (m_procUl.owns_lock())
        {
            m_sharedDataSP->m_state2 = S_DEFAULT;
            m_procUl.unlock();
            m_sharedDataSP->m_sub2cvRead.notify_one();
        }
        m_procUl = decltype(m_procUl)(m_sharedDataSP->m_sub1mtx);
        m_sharedDataSP->m_sub1cvProc.wait(m_procUl, [&] { return m_sharedDataSP->m_shutdown || m_sharedDataSP->m_state1 == S_READY; });
        m_start = m_sharedDataSP->m_sub1.data();
        m_skip = m_sharedDataSP->m_shutdown ? m_start : m_start + m_sharedDataSP->m_sub1read;
    }
    else
    {
        if (m_procUl.owns_lock())
        {
            m_sharedDataSP->m_state1 = S_DEFAULT;
            m_procUl.unlock();
            m_sharedDataSP->m_sub1cvRead.notify_one();
        }
        m_procUl = decltype(m_procUl)(m_sharedDataSP->m_sub2mtx);
        m_sharedDataSP->m_sub2cvProc.wait(m_procUl, [&] { return m_sharedDataSP->m_shutdown || m_sharedDataSP->m_state2 == S_READY; });
        m_start = m_sharedDataSP->m_sub2.data();
        m_skip = m_sharedDataSP->m_shutdown ? m_start : m_start + m_sharedDataSP->m_sub2read;
    }
    m_switch++;
}

void TextProc::proc()
{
    try { procX(); }
    catch (const std::bad_alloc &) { m_sharedDataSP->m_proc = S_OUT_OF_MEMORY; }
    catch (const std::exception &) { m_sharedDataSP->m_proc = S_ERR_PREFIN; }
    try
    {
        m_procUl = decltype(m_procUl)();
        if (m_sharedDataSP->m_proc == S_DEFAULT) m_sharedDataSP->m_proc = S_PREFIN;
    }
    catch (const std::exception &) { m_sharedDataSP->m_proc = S_ERR_PREFIN; }
    try { if (m_done != nullptr) m_done(this); } catch (const std::exception &) {}
}

bool TextProc::readXoe(const std::shared_ptr<SharedData> &a_sharedDataSP, size_t a_switch)
{
    assert(a_sharedDataSP != nullptr && a_sharedDataSP->m_file != nullptr);
    assert(a_sharedDataSP->m_sub1.size() > 0 && a_sharedDataSP->m_sub2.size() > 0);
    if (a_switch % 2 == 0)
    {
        std::unique_lock<std::mutex> l_lock1(a_sharedDataSP->m_sub1mtx);
        a_sharedDataSP->m_sub1cvRead.wait(l_lock1, [&] { return a_sharedDataSP->m_shutdown || a_sharedDataSP->m_state1 == S_DEFAULT; });
        if (a_sharedDataSP->m_shutdown) return true;
        a_sharedDataSP->m_state1 = S_READ;
        l_lock1.unlock();
        a_sharedDataSP->m_file->read(a_sharedDataSP->m_sub1.data(), a_sharedDataSP->m_sub1.size());
        a_sharedDataSP->m_sub1read = static_cast<size_t>(a_sharedDataSP->m_file->gcount());
        std::unique_lock<std::mutex> l_lock2(a_sharedDataSP->m_sub1mtx);
        a_sharedDataSP->m_state1 = S_READY;
        l_lock2.unlock();
        a_sharedDataSP->m_sub1cvProc.notify_one();
        if (a_sharedDataSP->m_sub1read == 0) return true;
    }
    else
    {
        std::unique_lock<std::mutex> l_lock1(a_sharedDataSP->m_sub2mtx);
        a_sharedDataSP->m_sub2cvRead.wait(l_lock1, [&] { return a_sharedDataSP->m_shutdown || a_sharedDataSP->m_state2 == S_DEFAULT; });
        if (a_sharedDataSP->m_shutdown) return true;
        a_sharedDataSP->m_state2 = S_READ;
        l_lock1.unlock();
        a_sharedDataSP->m_file->read(a_sharedDataSP->m_sub2.data(), a_sharedDataSP->m_sub2.size());
        a_sharedDataSP->m_sub2read = static_cast<size_t>(a_sharedDataSP->m_file->gcount());
        std::unique_lock<std::mutex> l_lock2(a_sharedDataSP->m_sub2mtx);
        a_sharedDataSP->m_state2 = S_READY;
        l_lock2.unlock();
        a_sharedDataSP->m_sub2cvProc.notify_one();
        if (a_sharedDataSP->m_sub2read == 0) return true;
    }
    return false;
}

void TextProc::readX(const std::shared_ptr<SharedData> &a_sharedDataSP)
{
    assert(a_sharedDataSP != nullptr && a_sharedDataSP->m_file != nullptr);
    assert(a_sharedDataSP->m_sub1.size() > 0 && a_sharedDataSP->m_sub2.size() > 0);
    for (size_t l_switch = 0; !readXoe(a_sharedDataSP, l_switch); l_switch++);
}

void TextProc::read(const std::shared_ptr<SharedData> &a_sharedDataSP)
{
    try { readX(a_sharedDataSP); a_sharedDataSP->m_read = S_PREFIN; }
    catch (const std::exception &) { a_sharedDataSP->m_read = S_ERR_PREFIN; }
}
