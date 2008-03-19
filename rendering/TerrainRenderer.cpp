//*****************************************************************************
//**
//**   File:               TerrainRenderer.cpp
//**   Author:             $Author$
//**   Last Change:        $Date$
//**   Last Revision:      $Revision$
//**
//*****************************************************************************

#include <string>

#include "TerrainRenderer.h"
#include "../GameLogic/Objects/Terrain.h"
#include "../GameLogic/GameEvents.h"
#include "../GameLogic/GameLogic.h"

#include "../rendering/RenderEngine.h"
#include "../gfxutils/ConfParser/ParserSection.h"
#include "../gfxutils/MemManager/MemMgr_RawData.h"
#include "../gfx/Model/ModelMgr.h"
#include "../gfx/Texture/TextureIO.h"
#include "../gfx/Texture/Texture.h"
#include "../gfx/Shaders/VertexAttrib.h"
#include "../gfx/Shaders/ShaderManager.h"
#include "../gfx/Model/Model.h"
#include "../gfx/Model/VBODesc.h"
#include "../gfx/Texture/Texture2D.h"
#include "../gfx/vbo/vbo.h"
#include "../Math/Vector4.h"
#include "../Math/Vector2.h"

#include "../GameLogic/Enemies/Enemyship.h"
#include "../GameLogic/Objects/Playership.h"
#include "../gfx/Camera.h"

using namespace std;

TerrainRenderer :: TerrainRenderer() :
	m_terrainModel(0),
	m_vbo(0),
	m_tformContribTex(0),
	m_heights(0),
	m_shadowTexture(0)
{
	EventManager::instance().registerEventListener< Terrain_Changed >(this);
	EventManager::instance().registerEventListener< Level_Load >(this);
	EventManager::instance().registerEventListener< Player_Spawned >(this);
	EventManager::instance().registerEventListener< Enemy_Spawned >(this);
	EventManager::instance().registerEventListener< Player_Despawned >(this);
	EventManager::instance().registerEventListener< Enemy_Despawned >(this);
}

TerrainRenderer :: ~TerrainRenderer()
{
	_clearResources();
}

void TerrainRenderer :: _clearResources()
{
	if(m_terrainModel)
	{
		const std::string texname =  m_terrainModel->matGroup(0).getTextureList()[0]->getName();
		TextureIO::instance()->deleteTexture(texname);
		delete m_terrainModel;
		m_terrainModel = 0;
	}
	if(m_vbo)
	{
		delete m_vbo;
		m_vbo = 0;
	}
	if(m_heights)
	{
		MemMgrRaw::instance()->free(m_heights);
		m_heights = 0;
	}
	if(m_tformContribTex)
		delete m_tformContribTex;
	if(m_shadowTexture)
		delete m_shadowTexture;
}


void TerrainRenderer :: render(Graphics& g) const
{
	_renderShadows();

	//creating the texture projection matrix
	float texProjMat[16];
	float viewMat[16];
	glPushMatrix();
		glLoadIdentity();
		// First scale and bias into [0..1] range.
		glTranslatef(0.5, 0.5, 0);
		glScalef(0.5, 0.5, 1);
		glOrtho(m_lightCameraProjSettings[0],
			m_lightCameraProjSettings[1],
			m_lightCameraProjSettings[2],
			m_lightCameraProjSettings[3],
			m_lightCameraProjSettings[4],
			m_lightCameraProjSettings[5]);
		gluLookAt(m_lightCameraEye.getX(),
			m_lightCameraEye.getY(),
			m_lightCameraEye.getZ(),
			m_lightCameraEye.getX() + m_lightCameraVectorView.getX(),
			m_lightCameraEye.getY() + m_lightCameraVectorView.getY(),
			m_lightCameraEye.getZ() + m_lightCameraVectorView.getZ(),
			m_lightCameraVectorUp.getX(),
			m_lightCameraVectorUp.getY(),
			m_lightCameraVectorUp.getZ());
		/* ~ MY WAY */
		glGetFloatv(GL_MODELVIEW_MATRIX, texProjMat);
	glPopMatrix();

	glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);

	Matrix44 invViewMatrix(viewMat);
	invViewMatrix.invert();

	// Do some magic to render the terrain
	ShaderManager::instance()->begin("TerrainShader");
	m_terrainModel->matGroup(0).getTextureList()[0]->bind(0);
	ShaderManager::instance()->setUniform1i("texmap0",0);
	m_terrainModel->matGroup(0).getTextureList()[1]->bind(1);
	ShaderManager::instance()->setUniform1i("texmap1",1);
	m_tformContribTex->bind(2);
	ShaderManager::instance()->setUniform1i("contribMap",2);
	m_shadowTexture->bind(3);
	ShaderManager::instance()->setUniform1i("shadowTex",3);
	ShaderManager::instance()->setUniformMatrix4fv("TexGenMat", texProjMat);
	ShaderManager::instance()->setUniformMatrix4fv("InvViewMat", invViewMatrix.cfp());

	// FIXME : fetch it from m_terrain
	const float mapsize = 129.0f;
	ShaderManager::instance()->setUniform1fv("mapCellNum",&mapsize);

	m_terrainModel->matGroup(0).vboDesc().call();

	TextureMgr::instance()->setTextureUnit(3);
	TextureMgr::instance()->setBoundTexture(0,3);
	TextureMgr::instance()->setTextureUnit(2);
	TextureMgr::instance()->setBoundTexture(0,2);
	TextureMgr::instance()->setTextureUnit(1);
	TextureMgr::instance()->setBoundTexture(0,1);

	CHECK_GL_ERROR();

	// Draw the lake
	// FIXME : do it appropriately
	
	ShaderManager::instance()->end();
	Vector3 ll(-m_mapExtents.getX()*0.5f,0.0f,m_mapExtents.getX()*0.5f);
	Vector3 right(m_mapExtents.getX(),0,0);
	Vector3 up(0,0,-m_mapExtents.getX());
	m_lakeTexture->bind();
	RenderEngine::drawTexturedQuad(ll,right,up,Vector2(0,0),Vector2(10,10));


	// Draw the trees
	// FIXME : do it appropriately

	// set tha material
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	m_treeMaterial.Apply();

	// For each tree type
	for(std::vector<ForestInfo_t>::const_iterator it = m_trees.begin();
		it!= m_trees.end();
		++it)
	{
		// for it's instance
		for(std::vector<TreeInfo_t>::const_iterator it2 = it->trees.begin();
			it2 != it->trees.end();
			++it2)
		{
			glPushMatrix();
			glTranslatef(it2->position.getX(),it2->position.getY(),it2->position.getZ());
			for(std::vector<MaterialGroup>::const_iterator matit = it->modelGeom->getMatGroup().begin();
				matit != it->modelGeom->getMatGroup().end();
				++matit)
			{
				matit->getTextureList()[0]->bind();
				matit->getVboDesc().call();
			}
			glPopMatrix();
		}
	}

	// Enable Alpha Test & draw textures
	glAlphaFunc(GL_GREATER,0.5);
	glEnable(GL_ALPHA_TEST);
	for(std::vector<ForestInfo_t>::const_iterator it = m_trees.begin();
		it!= m_trees.end();
		++it)
	{
		// for it's instance
		for(std::vector<TreeInfo_t>::const_iterator it2 = it->trees.begin();
			it2 != it->trees.end();
			++it2)
		{
			glPushMatrix();
			glTranslatef(it2->position.getX(),it2->position.getY(),it2->position.getZ());
			for(std::vector<MaterialGroup>::const_iterator matit = it->modelTex->getMatGroup().begin();
				matit != it->modelTex->getMatGroup().end();
				++matit)
			{
				matit->getTextureList()[0]->bind();
				matit->getVboDesc().call();
			}
			glPopMatrix();
		}
	}
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
}


void TerrainRenderer :: onEvent(Level_Load& evt)
{
	const ParserSection* psRoot = evt.getValue1();
	const ParserSection* psGraphics = psRoot->getSection("Graphics");

	std::string gfxlevelfile = psGraphics->getVal("file");

	_clearResources();

	_loadResources(evt.getValue2(), gfxlevelfile);
}

void TerrainRenderer :: _loadResources(const std::string& id,
									   const std::string& gfxlevelfile)
{
	// Fetch the parser sections

	ConfParser parser(std::string("config/levels/") + gfxlevelfile);

	unsigned dimension;
	unsigned dataSize,indexSize;
	Vector4 ldir;

	// Open the file
	string filepath = ModelMgr::instance().getModelDir() + parser.getSection("DataFiles")->getVal("TerrainData");
	FILE * fp = fopen(filepath.c_str(),"rb");

	// Read the dimension & light direction
	fread(&dimension,sizeof(unsigned),1,fp);
	//dimension = FromString<unsigned>(parser.getSection("Misc")->getVal("MapCellDim"));
	ldir = FromString<Vector4>(parser.getSection("Misc")->getVal("LightDir"));

	// Set the GL Light 0
	glPushAttrib(GL_MATRIX_MODE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLightfv(GL_LIGHT0,GL_POSITION,ldir.cfp());
	glPopMatrix();
	glPopAttrib();

	// Compute the index - data sizes
	dataSize = dimension * dimension;
	indexSize = 2*(dimension + 1)*(dimension - 1);		// tristrip index formula

	// Read the rest data ( vertex - texcoord - indices )
	Vector4 * vertexData = MemMgrRaw::instance()->allocate<Vector4>(dataSize);
	Vector2 * texcoordData = MemMgrRaw::instance()->allocate<Vector2>(dataSize);
	unsigned * indexData = MemMgrRaw::instance()->allocate<unsigned>(indexSize);

	fread(vertexData,sizeof(Vector4),dataSize,fp);
	fread(texcoordData,sizeof(Vector2),dataSize,fp);
	fread(indexData,sizeof(unsigned),indexSize,fp);
	fclose(fp);

	std::vector<void *> attribData;
	attribData.push_back((void *)vertexData);
	attribData.push_back((void *)texcoordData);
	
	// - We need only "Vertex" & "Texcoord" Attributes
	std::vector<const VertexAttribute *> vattrs;
	vattrs.push_back(ShaderManager::instance()->vertexAttribute("Vertex"));
	vattrs.push_back(ShaderManager::instance()->vertexAttribute("Texcoord"));

	// Create the vbo
	m_vbo = new VBO(vattrs,				// Specify attribs
					dataSize,			// now we don't need any more space for vertex data in this vbo
					indexSize,		// now we don't need any more space for index data in this vbo
					GL_STATIC_DRAW,		// usage hint, we won't change VBO contents
					"UselessString");	// This is a crappy classification attempt & will probably be removed

	m_terrainModel = new Model(string("Terrain_")+id,m_vbo);

	std::vector<Texture *> texvector;
	texvector.push_back(TextureIO::instance()->loadImage(parser.getSection("DataFiles")->getVal("BarrenTexture")));
	texvector.push_back(TextureIO::instance()->loadImage(parser.getSection("DataFiles")->getVal("TerraformTexture")));
	m_terrainModel->addMatGroup(MaterialGroup(Material(),
									 texvector,
									 VBODesc(m_vbo,vattrs,attribData,dataSize,indexData,indexSize,GL_TRIANGLE_STRIP),
									 -1));	

	// Get the heights & stuff for Terrain *, at the moment assume the scale is 1 & ymax = 100
	m_mapExtents = FromString<Vector3>(parser.getSection("Misc")->getVal("MapExtent"));
	m_heights = MemMgrRaw::instance()->allocate<float>(dimension*dimension);
	for(unsigned i=0;i<dataSize;++i)
		m_heights[i] = vertexData[i].getY();
	m_terrainDimension = dimension;
	

	// Free our data
	MemMgrRaw::instance()->free(vertexData);
	MemMgrRaw::instance()->free(texcoordData);
	MemMgrRaw::instance()->free(indexData);



	// TERRAFORM STUFF

	if(m_tformContribTex)
		delete m_tformContribTex;
	std::vector<MipmapLevel> ml;
	ml.push_back(MipmapLevel(0,0));
	m_tformContribTex = new Texture2D(dimension,dimension,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE,
									  ml,GL_TEXTURE_RECTANGLE_ARB,"Terraform contribution",false,true);
	m_tformFBO.Bind();
	m_tformFBO.UnattachAll();
	m_tformFBO.AttachTexture(m_tformContribTex->getTarget(),m_tformContribTex->getId(),GL_COLOR_ATTACHMENT0_EXT);
	bool res = m_tformFBO.IsValid();
	assert(res);
	FramebufferObject::Disable();


	// LAKE STUFF
	
	m_lakeTexture = TextureIO::instance()->getTexture("LakeTexture.dds");

	// TREE STUFF
	const string treefile = string("config/levels/") + parser.getSection("Misc")->getVal("TreePositions");
	const vector<string> treeModelNames = parser.getSection("Misc")->getValVector("TreeModels");
	fp = fopen(treefile.c_str(),"rb");
	unsigned treenum;
	fread(&treenum,sizeof(unsigned),1,fp);
	Vector4 * treepos = MemMgrRaw::instance()->allocate<Vector4>(treenum);
	fread(treepos,sizeof(Vector4),treenum,fp);
	fclose(fp);
	for(size_t i=0;i<treeModelNames.size();++i)
	{
		Model * m1 = ModelMgr::instance().getModel(treeModelNames[i] + string("_geom.obj"));
		Model * m2 = ModelMgr::instance().getModel(treeModelNames[i] + string("_tex.obj"));
		m_trees.push_back(ForestInfo_t(m1,m2));
	}
	for(unsigned i=0;i<treenum;++i)
	{
		const unsigned random_i = rand() % unsigned(m_trees.size());
		m_trees[random_i].trees.push_back(TreeInfo_t(Vector3(treepos[i].cfp())));
	}
	MemMgrRaw::instance()->free(treepos);

	// SHADOW STUFF
	_initShadows(ldir);


}

void TerrainRenderer :: onEvent(Terrain_Changed& evt)
{
	m_terrain = evt.getValue();
	m_terrain->fillData(m_heights,
						m_mapExtents.getX(),
						m_mapExtents.getY(),
						m_terrainDimension);	
	// Not allowed to do file loading here!
	// Keep that restricted to Level_Load please
	// If not possible at all: tell Joep :)
}

void TerrainRenderer :: _addShadowCaster(const CoordinateModel& model)
{
	m_shadowCasters.push_back(model);
}

void TerrainRenderer :: _removeShadowCaster(const CoordinateFrame * cf)
{
	std::vector<CoordinateModel>::iterator it = m_shadowCasters.begin();
	while(it != m_shadowCasters.end())
	{
		if(it->coordframe == cf)
		{
			m_shadowCasters.erase(it);
			break;
		}
		++it;
	}
}


void TerrainRenderer :: _updateTerraform(const float dt)
{
	const float terraform_interval = 2.0f; // FIXME : apply appropriate
	// For every active terraform process, add the dt
	for(size_t i=0; i< m_tformInfo.size();++i)
	{
		m_tformInfo[i].currentTimeOffset += dt;
		if(m_tformInfo[i].currentTimeOffset > terraform_interval)
		{
			m_tformInfo.erase(m_tformInfo.begin() + i);
			--i;
		}
	}

	// adjust tree heights!
}

void TerrainRenderer :: _updateTerraformContribution() const
{
	// save settings
	// bind the FBO
	// make settings
	// bind shader & texture
	// for every active terraform , run the shader in the appropriate quad ( for starters the whole map)
	// the shader makes the terraformed cells white
	// restore settings , unbind FBO
}


void TerrainRenderer :: onEvent(Player_Spawned& evt)
{
	// Get the const render settings of the ship
	const EntitySettings_t& settings = RenderEngine::instance().getConstRenderSettings().getEntitySettings(evt.getValue()->getType());
	const CoordinateFrame * cf = &(evt.getValue()->getCoordinateFrame());
	Model * model = ModelMgr::instance().getModel(settings.modelName);
	_addShadowCaster(CoordinateModel(model,cf));
}

void TerrainRenderer :: onEvent(Enemy_Spawned& evt)
{
	// Get the const render settings of the ship
	const EntitySettings_t& settings = RenderEngine::instance().getConstRenderSettings().getEntitySettings(evt.getValue()->getType());
	const CoordinateFrame * cf = &(evt.getValue()->getCoordinateFrame());
	Model * model = ModelMgr::instance().getModel(settings.modelName);
	_addShadowCaster(CoordinateModel(model,cf));
}

void TerrainRenderer :: onEvent(Enemy_Despawned& evt)
{
	// Fetch the enemy & remove
	const CoordinateFrame * cf = &(evt.getValue()->getCoordinateFrame());
	_removeShadowCaster(cf);
}

void TerrainRenderer :: onEvent(Player_Despawned& evt)
{
	// Fetch the player & remove, based on coordinate frame address
	const CoordinateFrame * cf = &(evt.getValue()->getCoordinateFrame());
	_removeShadowCaster(cf);
}

void TerrainRenderer :: _renderShadows() const
{
	// switches VBOs to vertex-only mode
	// set's up light camera
	// renders ships
	// fetches - stores texture
	// switches camera
	// computes 

	int drawbuf;
	glGetIntegerv(GL_DRAW_BUFFER,&drawbuf);
	int vp[4];
	glGetIntegerv(GL_VIEWPORT,vp);
	const unsigned ts = RenderEngine::instance().getConstRenderSettings().getShadowTextureSize();
	glViewport(0,0,ts,ts);
	VAttribStatus vstatus = VBOMgr::instance().getCurrentFlags();
	m_shadowFBO.Bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	VAttribStatus vonly;
	const VertexAttribute * vattr = ShaderManager::instance()->vertexAttribute("Vertex");
	vonly.attrib[ShaderManager::instance()->vertexAttributeIndex(vattr)] = 0xFF;
	VBOMgr::instance().setCurrentFlags(vonly);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(m_lightCameraProjSettings[0],
		m_lightCameraProjSettings[1],
		m_lightCameraProjSettings[2],
		m_lightCameraProjSettings[3],
		m_lightCameraProjSettings[4],
		m_lightCameraProjSettings[5]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(m_lightCameraEye.getX(),
			m_lightCameraEye.getY(),
			m_lightCameraEye.getZ(),
			m_lightCameraEye.getX() + m_lightCameraVectorView.getX(),
			m_lightCameraEye.getY() + m_lightCameraVectorView.getY(),
			m_lightCameraEye.getZ() + m_lightCameraVectorView.getZ(),
			m_lightCameraVectorUp.getX(),
			m_lightCameraVectorUp.getY(),
			m_lightCameraVectorUp.getZ());

	// Disable Color Writes Here??
	// Render the ships minimally - set the minimal shader

	glClear(GL_COLOR_BUFFER_BIT);
	ShaderManager::instance()->begin("ShadowWriteShader");
	
	for(std::vector<CoordinateModel>::const_iterator it = m_shadowCasters.begin();
		it != m_shadowCasters.end();
		++it)
	{
		glPushMatrix();
		glMultMatrixf(it->coordframe->getMatrix().cfp());
		for(size_t i=0;i<it->model->getMatGroup().size();++i)
			it->model->getMatGroup()[i].getVboDesc().call();
		glPopMatrix();
	}

	// Restore stuff

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	FramebufferObject::Disable();
	glDrawBuffer(drawbuf);
	VBOMgr::instance().setCurrentFlags(vstatus);
	glViewport(vp[0],vp[1],vp[2],vp[3]);
}

void TerrainRenderer :: _initShadows(const Vector4& lightdir)
{
	// get camera projector settings
	float w,h,n,f;
	m_cameraRef->getProjSettings(w,h,n,f);
	
	// compute the max quad dims shown in height = 0
	const float gameplanedepth = 1500.0f;
	float maxLeftRight = w* gameplanedepth/ n;
	float maxTopDown = h*gameplanedepth / n;

	// compute the light camera vectors
	Vector3 ldir(-lightdir.getX(),-lightdir.getY()*10,-lightdir.getZ());
	ldir.normalize();
	Vector3 yAxis(0.0f,1.0f,0.0f);
	Vector3 right(Vector3::cross(ldir,yAxis));
	right.normalize();
	m_lightCameraVectorUp = Vector3::cross(right,ldir);
	m_lightCameraVectorUp.normalize();

	// Adjust maxleft & maxright so that they cover the whole screen
	const Vector3 X_AXIS(1.0f,0.0f,0.0f);
	const Vector3 Z_AXIS(0.0f,0.0f,1.0f);
	float right_d = max(abs(right.dot(X_AXIS)), abs(m_lightCameraVectorUp.dot(X_AXIS)));
	float up_d = max(abs(right.dot(Z_AXIS)), abs(m_lightCameraVectorUp.dot(Z_AXIS)));

	maxLeftRight /= right_d;
	maxTopDown /= up_d;

	// set a safe distance for the light eye
	m_lightCameraEye = Vector3(0.0f,gameplanedepth + GameLogic::instance().getGamePlaneHeight(),0.0f) - 500.0f*ldir;
	m_lightCameraVectorView = ldir;

	m_lightCameraProjSettings[0] = -maxLeftRight;
	m_lightCameraProjSettings[1] = maxLeftRight;
	m_lightCameraProjSettings[2] = -maxTopDown;
	m_lightCameraProjSettings[3] = maxTopDown;
	m_lightCameraProjSettings[4] = n;
	m_lightCameraProjSettings[5] = f;

	m_lightDir = ldir;

	// initialize the texture & FBO
	const unsigned ts = RenderEngine::instance().getConstRenderSettings().getShadowTextureSize();
	std::vector<MipmapLevel> ml;
	ml.push_back(MipmapLevel(0,0));
	m_shadowFBO.Bind();
	m_shadowTexture = new Texture2D(ts,ts,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE,ml,GL_TEXTURE_2D,"Shadow",false,false);
	glGenerateMipmapEXT(GL_TEXTURE_2D);
	m_shadowFBO.AttachTexture(GL_TEXTURE_2D,m_shadowTexture->getId(),GL_COLOR_ATTACHMENT0_EXT);
	m_shadowFBO.IsValid();
	FramebufferObject::Disable();
}

void TerrainRenderer :: update(const float dt)
{
	// Also update trees & contrib tex
	
	m_lightCameraEye = m_cameraRef->getEye() - 500.0f*m_lightDir;
	_updateTerraform(dt);
}