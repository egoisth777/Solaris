#include "IUpdateComponent.h"


void IUpdateComponent::setEnable(bool val){
    this->m_isEnabled = val;
}

bool IUpdateComponent::isEnabled() const
{
    return m_isEnabled;
}
