#ifndef IUPDATECOMPONENT_H
#define IUPDATECOMPONENT_H

class IUpdateComponent{

private:
    bool m_isEnabled = true;

public:
    virtual void awake() {};
    virtual void start() {};
    virtual void onGameStart() {};
    virtual void update() {};

    virtual void setEnable(bool enable);
    bool isEnabled() const;
};

#endif // IUPDATECOMPONENT_H
