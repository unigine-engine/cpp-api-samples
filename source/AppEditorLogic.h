#ifndef __APP_EDITOR_LOGIC_H__
#define __APP_EDITOR_LOGIC_H__

#include <UnigineLogic.h>

class AppEditorLogic : public Unigine::EditorLogic
{
public:
	AppEditorLogic();
	virtual ~AppEditorLogic();

	virtual int init();

	virtual int update();
	virtual int render();

	virtual int shutdown();
	virtual int destroy();

	virtual int worldInit();
	virtual int worldShutdown();
	virtual int worldSave();
};

#endif // __APP_EDITOR_LOGIC_H__
