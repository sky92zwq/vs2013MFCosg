// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"
#include <osg/ShapeDrawable>
//#define NumWavePlan 20
const int xCount_Waveplane = 20;
const int yCount_Waveplane = 20;
const float xMin_Waveplane = -10.0;
const float yMin_Waveplane = -10.0;
const float xdelta_Waveplane = 1.0;
const float ydelta_Waveplane = 1.0;
//***************************shader***********************************
static const char* vertSource = {
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position=ftransform();\n"
	"}\n"
};
static const char* fragSource = {
	"uniform vec4 color;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor=color;\n"
	"}\n"
};
//**************************回调所用更新数据****************************
//****************************更新回调**********************************
class WaveplaneCallback : public osg::Drawable::UpdateCallback
{
public:
	WaveplaneCallback():height(1.0){}
	~WaveplaneCallback(){}
	virtual void update(osg::NodeVisitor *nv, osg::Drawable *drawabel)
	{
		osg::HeightField *pheightfield = dynamic_cast<osg::HeightField*>(drawabel->getShape());
		if (pheightfield)
		{

			float x, y, z;
			for (int i = 0; i < yCount_Waveplane; i++)
			{
				for (int j = 0; j < xCount_Waveplane; j++)
				{
					x = xMin_Waveplane + j * xdelta_Waveplane;
					y = yMin_Waveplane + i * ydelta_Waveplane;
					z = x;
					pheightfield->setHeight(j, yCount_Waveplane - i - 1, sin(z+height));//循环得到每个顶点，然后为其设置z值
				}
			}
			if(height<20)height += 0.1;
			else height = 1;
		}
		
	}

protected:
	float height;
};//*****************************回调end*********************************


cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd) 
{
}

cOSG::~cOSG()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();

    delete mViewer;
}

void cOSG::InitOSG(std::string modelname)
{
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void cOSG::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}


void cOSG::InitSceneGraph(void)//改造******************
{
    // Init the main Root Node/Group
    mRoot  = new osg::Group;

    //// Load the Model from the model name
    //mModel = osgDB::readNodeFile(m_ModelName);

    //// Optimize the model
    //osgUtil::Optimizer optimizer;
    //optimizer.optimize(mModel.get());
    //optimizer.reset();

	////vertices
	//osg::ref_ptr<osg::Vec3Array > vertices = new osg::Vec3Array;
	//for (unsigned int i = 0; i < NumWavePlan;)
	//{
	//	vertices->push_back(osg::Vec3(-4.0 + float(i/2), 4.0, 0.0));//0,2,4,6,8
	//	vertices->push_back(osg::Vec3(-4.0 + float(i/2), 0.0, 0.0));//1,3,5,7,9
	//	i += 2;
	//}

	////color
	//osg::ref_ptr<osg::Vec4Array > colors = new osg::Vec4Array;
	//colors->push_back(osg::Vec4(1.0, 0.0, 0.0, 1.0));

	//osg::ref_ptr<osg::Geometry> planeGeom = new osg::Geometry;
	//planeGeom->setVertexArray(vertices.get());
	//planeGeom->setColorArray(colors.get());
	//planeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	//planeGeom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::TRIANGLE_STRIP, 0, NumWavePlan));

	//planeGeom->setInitialBound(osg::BoundingBox(osg::Vec3(-10.0, -10.0, -10.0), osg::Vec3(10.0, 10.0, 10.0)) );
	//planeGeom->setUpdateCallback(new DynamicLineCallback);
	//planeGeom->setUseDisplayList(false);
	//planeGeom->setUseVertexBufferObjects(true);

	//osg::HeightField,come from Class shape
	osg::ref_ptr<osg::HeightField> pHeightField = new osg::HeightField();
	pHeightField->allocate(xCount_Waveplane, yCount_Waveplane);
	pHeightField->setOrigin(osg::Vec3(xMin_Waveplane, yMin_Waveplane, 0));
	pHeightField->setXInterval(xdelta_Waveplane);
	pHeightField->setYInterval(ydelta_Waveplane);
	float x, y ,z;
	for (int i = 0; i < yCount_Waveplane; i++)
	{
		for (int j = 0; j < xCount_Waveplane; j++)
		{
			x = xMin_Waveplane + j * xdelta_Waveplane;
			y = yMin_Waveplane + i * ydelta_Waveplane;
			z = 0;
			pHeightField->setHeight(j, yCount_Waveplane - i - 1, z);//循环得到每个顶点，然后为其设置z值
		}
	}
	//osg::Cylinder
	osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder(osg::Vec3(1.0f, 0.0f, 0.0f), 10.8f, 10.0f);
	//createshader
	osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX,vertSource);
	osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT, fragSource);
	osg::ref_ptr<osg::Program> program = new osg::Program;
	program->addShader(vertShader.get());
	program->addShader(fragShader.get());
	osg::ref_ptr<osg::Uniform> color = new osg::Uniform("color",osg::Vec4(1.0,0.5,0.5,0.5));
	

	osg::ref_ptr<osg::ShapeDrawable> shapedrawable = new osg::ShapeDrawable(cylinder.get());
	shapedrawable->setInitialBound(osg::BoundingBox(osg::Vec3(-10.0, -10.0, -10.0), osg::Vec3(10.0, 10.0, 10.0)));
	//shapedrawable->setUpdateCallback(new WaveplaneCallback);
	shapedrawable->setUseDisplayList(false);
	shapedrawable->setUseVertexBufferObjects(true);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	/*geode->addDrawable(planeGeom.get());*/
	geode->addDrawable(shapedrawable);
	geode->getOrCreateStateSet()->addUniform(color.get());
	geode->getOrCreateStateSet()->setAttributeAndModes(program.get());
	
	

    // Add the model to the scene
    mRoot->addChild(geode.get());
}

void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    RECT rect;

    // Create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);
    
    // Get the current window size
    ::GetWindowRect(m_hWnd, &rect);

    // Init the GraphicsContext Traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // Create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Init a new Camera (Master for this View)
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());

    // Set the Scene Data
    mViewer->setSceneData(mRoot.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;

    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

    //viewer->run();
    while(!viewer->done())
    {
        osg->PreFrameUpdate();
        viewer->frame();
        osg->PostFrameUpdate();
        //Sleep(10);         // Use this command if you need to allow other processes to have cpu time
    }

    // For some reason this has to be here to avoid issue: 
    // if you have multiple OSG windows up 
    // and you exit one then all stop rendering
    //AfxMessageBox(_T("Exit Rendering Thread"));

    _endthread();
}
