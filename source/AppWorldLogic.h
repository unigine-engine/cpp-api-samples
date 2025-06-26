#ifndef __APP_WORLD_LOGIC_H__
#define __APP_WORLD_LOGIC_H__

#include <UnigineLogic.h>
#include <UnigineStreams.h>
#include <UnigineInterface.h>

class AppWorldLogic : public Unigine::WorldLogic
{
public:
	AppWorldLogic();
	virtual ~AppWorldLogic();

	virtual int init();

	virtual int update();
	virtual int render();
	virtual int flush();

	virtual int shutdown();
	virtual int destroy();

	virtual int save(const Unigine::StreamPtr &stream);
	virtual int restore(const Unigine::StreamPtr &stream);
};

#endif // __APP_WORLD_LOGIC_H__
