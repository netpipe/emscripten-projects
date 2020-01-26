#ifndef BOX2D_ENGINE_H_INCLUDED
#define BOX2D_ENGINE_H_INCLUDED

#include "header.h"
//BOX2D//

// Глобальные переменные обьектов для их управления

b2Body *playerBody;



void setWall(float x,float y,int w,int h)
{
	b2PolygonShape gr;
	gr.SetAsBox(w/SCALE,h/SCALE);
	b2BodyDef bdefw;
	bdefw.position.Set(x/SCALE, y/SCALE);
	b2Body *b_ground = World.CreateBody(&bdefw);
	b_ground->CreateFixture(&gr,1);
}


void setBox(int x,int y,int w, int h )
{

	b2PolygonShape gr;
	gr.SetAsBox(0.5*w/SCALE,0.5*h/SCALE);
	b2BodyDef bdefw;
	bdefw.type=b2_dynamicBody;
	bdefw.position.Set(x/SCALE, y/SCALE);
	b2Body *body = World.CreateBody(&bdefw);
	body->CreateFixture(&gr,1);
	body->SetUserData((void *)("box"));
}

void setBoxIron(int x,int y,int w,int h )
{
	b2PolygonShape gr;
	gr.SetAsBox(0.5*w/SCALE,0.5*h/SCALE);
	b2BodyDef bdefw;
	bdefw.type=b2_dynamicBody;
	bdefw.position.Set(x/SCALE, y/SCALE);
	b2Body *body = World.CreateBody(&bdefw);
	body->CreateFixture(&gr,2);
	body->SetUserData((void *)("boxiron"));
}

void setSwing(int x,int y)
{

	b2BodyDef bdef;
	bdef.type=b2_dynamicBody;
	/////////////////////////
	bdef.position.Set(x,y);
	bdef.angle=90*DEG;
	b2Body *b = World.CreateBody(&bdef);

	b2PolygonShape gr;
	gr.SetAsBox(5/SCALE,120/SCALE);
	b->CreateFixture(&gr,1);

    b2CircleShape circle;
	circle.m_radius=2/SCALE;
	circle.m_p.Set(-20/SCALE,0);
	b->CreateFixture(&circle,1);
	b->SetUserData((void *)("swing"));
}

void setStone(int x,int y)
{
    b2BodyDef bdef;
	bdef.type=b2_dynamicBody;
    /////////////////////////
	bdef.position.Set(x/SCALE,y/SCALE);

	b2CircleShape circle;
	circle.m_radius=40/SCALE;
	b2Body *b = World.CreateBody(&bdef);
	b->CreateFixture(&circle,1);
	b->SetUserData((void *) ("stone"));

	b2FixtureDef boxFigure;
    boxFigure.shape =&circle;
    boxFigure.restitution=0.5;
    boxFigure.density=0;

    b->CreateFixture(&boxFigure);
}



void setBullet(int x, int y, float vx, float vy)
{
    b2BodyDef bdef;
	bdef.type=b2_dynamicBody;
    /////////////////////////
	bdef.position.Set(x/SCALE,y/SCALE);
	b2CircleShape circle;
	circle.m_radius=3/SCALE;
	b2Body *bul1 = World.CreateBody(&bdef);
 //   bul1->SetBullet(true);
	bul1->CreateFixture(&circle,0.05);
	bul1->SetUserData((void *) ("bullet1"));
	bul1->ApplyForceToCenter( b2Vec2( vx,vy ),1);
}

void setBullet2(int x, int y, float vx, float vy, float ang)
{
	b2PolygonShape gr;
	gr.SetAsBox(1/SCALE,3/SCALE);
	b2BodyDef bdefw;
	bdefw.type=b2_dynamicBody;
	bdefw.angle=-ang*M_PI/180;
	bdefw.position.Set(x/SCALE, y/SCALE);
	b2Body *bul2 = World.CreateBody(&bdefw);
//	bul2->SetBullet(true);
	bul2->CreateFixture(&gr,0.05);
	bul2->SetUserData((void *)("bullet2"));
    bul2->ApplyForceToCenter( b2Vec2( vx,vy ),1);
}

void setPlayer(int x,int y)
{

    b2BodyDef bdef;
	bdef.type=b2_dynamicBody;
	///////////////////////
	bdef.position.Set(x/SCALE,y/SCALE);
	b2CircleShape circle;
	circle.m_radius=16/SCALE;
	playerBody = World.CreateBody(&bdef);
	playerBody->CreateFixture(&circle,2);
	playerBody->SetUserData((void *) ("player"));
}

//end-BOX2D//

#endif // BOX2D_ENGINE_H_INCLUDED
