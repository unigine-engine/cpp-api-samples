#ifndef __APP_WORLD_LOGIC_H__
#define __APP_WORLD_LOGIC_H__

#include <UnigineLogic.h>
#include <UnigineStreams.h>
#include <UnigineInterface.h>

class AppWorldLogic : public Unigine::WorldLogic
{
public:
	AppWorldLogic();
	virtual ~AppWorldLogic() override;

	virtual int init() override;
	virtual int update() override;
	virtual int shutdown() override;
};

#endif // __APP_WORLD_LOGIC_H__
