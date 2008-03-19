//*****************************************************************************
//**
//**   File:               TerrainRenderer.h
//**   Author:             $Author$
//**   Last Change:        $Date$
//**   Last Revision:      $Revision$
//**
//*****************************************************************************

#pragma once
#include "../utility/EventManager.h"
#include "../GameLogic/GameEvents.h"
#include "../gfx/FBO/FramebufferObject.h"
#include "../gfx/Material.h"
#include "CoordinateModel.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

#include <vector>

class Terrain;
class Graphics;
class Model;
class VBO;
class Texture;
class Camera;

class TerrainRenderer :
	public EventListener< Terrain_Changed >,
	public EventListener< Level_Load >,
	public EventListener< Player_Spawned >,
	public EventListener< Enemy_Spawned >,
	public EventListener< Enemy_Despawned >,
	public EventListener< Player_Despawned >

{
public:
	TerrainRenderer();
	~TerrainRenderer();

	void render(Graphics& g) const;

	void onEvent(Level_Load&);
	void onEvent(Terrain_Changed&);
	void onEvent(Player_Spawned&);
	void onEvent(Enemy_Spawned&);
	void onEvent(Enemy_Despawned&);
	void onEvent(Player_Despawned&);

	void setCamera(const Camera * cam) {m_cameraRef = cam;}
	void update(const float dt);

private:

	// STRUCT DECLARATIONS

	struct TerraformInfo_t
	{
		Vector3 center;
		float radius;
		float currentTimeOffset;
	};

	// PRIVATE FUNCS

	void _clearResources();
	void _loadResources(const std::string& id,
		                const std::string& gfxlevelfile);

	void _updateTerraform(const float dt); // updates the terraform contribution map
	void _updateTerraformContribution() const;

	void _addShadowCaster(const CoordinateModel& model);
	void _removeShadowCaster(const CoordinateFrame * cf);
	void _renderShadows() const;
	void _initShadows(const Vector4& lightdir);


	// VARIABLES
	const Camera * m_cameraRef;
	Vector4		   m_lightColor;

	Terrain* m_terrain;
	float	* m_heights;
	unsigned m_terrainDimension;
	Vector3  m_mapExtents;
	Model     * m_terrainModel;			// The terrain 'model'
	VBO       * m_vbo;					// The terrain will always have it's own VBO

	// For terraform contribution computation
	FramebufferObject m_tformFBO;
	Texture * m_tformContribTex;
	std::vector<TerraformInfo_t> m_tformInfo;

	// For the lake
	Texture * m_lakeTexture;

	// For the shadows
	std::vector<CoordinateModel> m_shadowCasters;
	Texture * m_shadowTexture;
	FramebufferObject m_shadowFBO;
	Vector3 m_lightCameraVectorUp;
	Vector3 m_lightCameraVectorView;
	Vector3 m_lightCameraEye;
	float m_lightCameraProjSettings[6];
	Vector3 m_lightDir;

};
