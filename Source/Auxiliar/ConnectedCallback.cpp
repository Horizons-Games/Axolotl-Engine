#include "StdAfx.h"

#include "ConnectedCallback.h"

 ConnectedCallback::ConnectedCallback(std::function<void(void)>&& deregisterMethod) : deregisterMethod(deregisterMethod)
{
}

ConnectedCallback::~ConnectedCallback()
{
	deregisterMethod();
}
