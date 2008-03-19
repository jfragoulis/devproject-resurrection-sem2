#include "ConstRenderSettings.h"
#include "../gfxutils/ConfParser/ParserSection.h"
#include "../gfx/Model/Model.h"
#include "../gfx/Shaders/ShaderManager.h"
#include "../gfx/Model/ModelMgr.h"
#include "../gfx/Texture/TextureIO.h"
#include "../GameLogic/WorldObjectTypeManager.h"
#include <assert.h>

void ConstRenderSettings :: init(const ParserSection * parsec)
{
	std::vector<const ParserSection *> entities = parsec->getChildren();
	for(size_t i=0;i<entities.size();++i)
	{
		if(entities[i]->getName() == "Misc")
		{
			_parseMiscEntities(entities[i]);
			continue;
		}
		EntitySettings_t es;
		es.entityName = entities[i]->getName();
		es.modelName = entities[i]->getVal("ModelHook");
		es.type = WorldObjectTypeManager::instance().getTypeFromName(es.entityName);
		// Load the model here & figure out which shader to use
		Model * model = ModelMgr::instance().getModel(es.modelName);
		assert(model);
		if(model->matGroup(0).getTextureList().empty())
			es.shaderIndex = ShaderManager::instance()->getShaderIndex("PerPixelNoTex");
		else if(model->matGroup(0).getTextureList().size() == 1)
			es.shaderIndex = ShaderManager::instance()->getShaderIndex("PerPixelTex");
		else
			;// ..more?? another shader then!

		// push back the settings
		m_entities.push_back(es);

		// Assign the shader to all the material groups, just for reference
		for(unsigned i=0;i<model->getMatGroup().size();++i)
			model->matGroup(i).setShaderIndex(es.shaderIndex);
	}
}

const EntitySettings_t& ConstRenderSettings :: getEntitySettings(const int type) const
{
	for(std::vector<EntitySettings_t>::const_iterator it = m_entities.begin();
		it != m_entities.end();
		++it)
	{
		if(it->type == type)
			return (*it);
	}
	assert(0);
	return (*m_entities.begin());	// shut up compiler!
}

void ConstRenderSettings :: _parseMiscEntities(const ParserSection * parsec)
{
	// read spawn point info
	m_spawnpointInterval = FromString<float>(parsec->getVal("SpawnPointInterval"));
	m_spawnpointSize = FromString<float>(parsec->getVal("SpawnPointSize"));
	m_spawnpointTexture = TextureIO::instance()->getTexture(parsec->getVal("SpawnPointTexture"));
	m_shadowTextureSize = FromString<unsigned>(parsec->getVal("ShadowTextureSize"));
	m_tformTimePerUnit = FromString<float>(parsec->getVal("TerraformingTimePerUnit"));
	m_laserTexName = parsec->getVal("LaserTexture");

	m_waveChangeRate = FromString<float>(parsec->getVal("WaveChangeRate"));
	m_waveSpeed = FromString<float>(parsec->getVal("WaveSpeed"));
	m_waveRepeats = FromString<float>(parsec->getVal("WaveRepeats"));
	m_cloudRepeats = FromString<float>(parsec->getVal("CloudRepeats"));
	m_lakeTexture = parsec->getVal("LakeTexture");
	m_cloudTexture = parsec->getVal("CloudTexture");
	
}