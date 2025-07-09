#ifndef __APP_SYSTEM_LOGIC_H__
#define __APP_SYSTEM_LOGIC_H__

#include <UnigineLogic.h>

class AppSystemLogic : public Unigine::SystemLogic
{
public:
	AppSystemLogic();
	virtual ~AppSystemLogic() override;

	virtual int init() override;
	virtual int update() override;
	virtual int shutdown() override;
};

#endif // __APP_SYSTEM_LOGIC_H__
