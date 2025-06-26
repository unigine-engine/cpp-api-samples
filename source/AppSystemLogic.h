#ifndef __APP_SYSTEM_LOGIC_H__
#define __APP_SYSTEM_LOGIC_H__

#include <UnigineLogic.h>

class AppSystemLogic : public Unigine::SystemLogic
{
public:
	AppSystemLogic();
	virtual ~AppSystemLogic();

	virtual int init();

	virtual int update();
	virtual int render();

	virtual int shutdown();
	virtual int destroy();
};

#endif // __APP_SYSTEM_LOGIC_H__
