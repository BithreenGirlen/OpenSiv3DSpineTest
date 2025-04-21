#ifndef SIV3D_SPINE_PLAYER_H_
#define SIV3D_SPINE_PLAYER_H_

#include "spine_player.h"

class CSiv3dSpinePlayer : public CSpinePlayer
{
public:
	CSiv3dSpinePlayer();
	virtual ~CSiv3dSpinePlayer();

	virtual void Redraw(float fDelta);
private:
	virtual void WorkOutDefaultScale();
};

#endif // !SIV3D_SPINE_PLAYER_H_

