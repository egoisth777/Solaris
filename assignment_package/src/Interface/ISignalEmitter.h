#ifndef ISIGNALEMITTER_H
#define ISIGNALEMITTER_H

class ISignalEmitter
{
protected:
    bool m_isSigBlocked = false;

public:
    bool IsSigBlocked();
    void SetSignalBlock(bool);
};



/// Inline implementation

inline bool ISignalEmitter::IsSigBlocked()
{
    return m_isSigBlocked;
}

inline void ISignalEmitter::SetSignalBlock(bool val)
{
    m_isSigBlocked = val;
}


#endif // ISIGNALEMITTER_H
