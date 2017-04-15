#include <iostream>
#include "atRenderer.h"
using namespace atRenderEngine;

int main()
{
	atRenderer renderer;
	

	atCamera* camera = new atCamera();
	camera->SetPerspective(Utility::ToRadians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
	camera->SetLookAt(Vector3f(-3, 0.5, 0), Vector3f(0, 0.5, 0), Vector3f(0, 1, 0));

	renderer.AddCamera(camera);
	renderer.Setup("Egg", 1920, 1080);
	//camera->SetModel(Eigen::Matrix4f::Identity());
	atScene* scene1 = new atScene(); 
	scene1->LoadMesh("../data/horse_ref.obj", false, true , Vector3f(0,0,-1));
	
	atScene* scene2 = new atScene();
	scene2->LoadMesh("../data/camel_ref.obj", false, true, Vector3f(0, 0, 1.0));
	/*scene1->LoadMesh("../data/POINT.DAT", false, true);

	atScene* scene3 = new atScene();
	scene3->LoadMesh("../data/BARREL.DAT", true, true);
*/
	renderer.AddScene(scene1);
	renderer.AddScene(scene2);
	/*renderer.AddScene(scene3);*/

	renderer.Draw();
	return 1;
}