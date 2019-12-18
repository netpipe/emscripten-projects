#include "Framework.h"

int screenWidth = 800, screenHeight = 600;

Program program;
Camera camera;
Light light;

AnimatedModel player, knight;
bool attack = false;
float attackWeight = 0;

void OnUpdate();

int main(int argc, char** argv)
{
	printf("Horde3D test\n");

	if (program.Init("Test 1", screenWidth, screenHeight) == 0)
	{
		return 0;
	}

	camera.Init(screenWidth, screenHeight);
	camera.pos.y = 3;
	camera.pos.z = 5;
	camera.rot.x = -20;
	camera.SetSkyBox("models/skybox/skybox.scene.xml");

	light.Init(H3DRootNode, "light_1", Horde3D::Vec3f(1, 10, 20), Horde3D::Vec3f(-1, -1, -1));

	Model obj1;
	obj1.Load(H3DRootNode, "models/platform/platform.scene.xml");

	Model man, sphere;
	for (int q = 0; q < 20; q++)
	{
		man.Load(H3DRootNode, "models/man/man.scene.xml");
		man.pos.x = rand() % 40 - 20;
		man.pos.z = rand() % 40 - 20;
		man.rot.y = rand() % 360;
		man.Update();

		sphere.Load(H3DRootNode, "models/sphere/sphere.scene.xml");
		sphere.pos.x = rand() % 40 - 20;
		sphere.pos.z = rand() % 40 - 20;
		sphere.rot.x = rand() % 360;
		sphere.rot.y = rand() % 360;
		sphere.rot.y = rand() % 360;
		sphere.Update();
	}

	player.Load(H3DRootNode, "models/man/man.scene.xml");
	player.pos.x = 0;
	player.pos.z = 0;
	player.Update();
	const char *anims[] = { "animations/man.anim" };
	player.LoadAnimations(anims, 1);

	knight.Load(H3DRootNode, "models/knight/knight.scene.xml");
	knight.scale.x = 0.03f;
	knight.scale.y = 0.03f;
	knight.scale.z = 0.03f;
	knight.pos.z = -4;
	knight.pos.x = 3;
	knight.rot.y = 185;
	knight.Update();
	const char *anims2[] = { "animations/knight_order.anim", "animations/knight_attack.anim" };
	knight.LoadAnimations(anims2, 2);


	h3dutLoadResourcesFromDisk(contentDir);
	h3dutDumpMessages();

	program.EnterLoop(&OnUpdate);

	return 0;
}


void OnUpdate()
{
	float d = program.OnUpdate();

	float spd = 2 * d;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_W])
		camera.Forward(spd);
	if (state[SDL_SCANCODE_S])
		camera.Forward(-spd);
	if (state[SDL_SCANCODE_A])
		camera.Strafe(-spd);
	if (state[SDL_SCANCODE_D])
		camera.Strafe(spd);

	camera.rot.y -= program.mouseDX;
	camera.rot.x -= program.mouseDY;
	camera.Update();

	// printf("camPos: %8.4f %8.4f %8.4f   camRot: %8.4f %8.4f %8.4f \n", camera.pos.x, camera.pos.y, camera.pos.z, camera.rot.x, camera.rot.y, camera.rot.z);

	int anim = 0;
	if (state[SDL_SCANCODE_UP])
	{
		player.Forward(-spd);
		anim = 1;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		player.Forward(spd);
		anim = 1;
	}
	if (state[SDL_SCANCODE_LEFT])
	{
		player.rot.y += spd * 50;
	}
	if (state[SDL_SCANCODE_RIGHT])
	{
		player.rot.y -= spd * 50;
	}
	if (anim)
	{
		player.UpdateAnimTime(30 * d);
		player.UpdateAnim(0, 1.0f);
	}

	player.Update();


	// random attack
	if (attack == false && rand() % 100 < 1)
	{
		attack = true;
		attackWeight = 0;
	}


	knight.UpdateAnimTime(30 * d);
	if (attack)
	{
		// to attack
		if (attackWeight < 1.0f)
		{
			knight.UpdateAnim(0, 1.0f - attackWeight);
			knight.UpdateAnim(1, attackWeight);
		}
		else // to order
		{
			float temp = 2.0f - attackWeight;
			knight.UpdateAnim(0, 1.0f - temp);
			knight.UpdateAnim(1, temp);
		}
		attackWeight += d;
		if (attackWeight >= 2)
			attack = false;
	}


	h3dRender(camera.node);

	h3dFinalizeFrame();
	h3dutDumpMessages();

	program.SwapWindow();
}
