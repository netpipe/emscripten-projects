#include "Game_settings.h"
#include "SDL_engine.h"
#include "Box2d_engine.h"
#include "Music_engine.h"
#include "level.h"

// Глобальные переменные карты
 std::vector<Object> obj;//вектор объектов карты




struct BL
{
    float name;
    float x;
    float y;
    float vx;
    float vy;
    float ang;
    float massa;

};

 std::vector<BL> oo;  // пули не бокс2д


// Глобальный класс объектов
class Global : public SDL_game  {
public:


    float x, y, z = NULL; // Координаты всех объектов
    int w, h = NULL;
    int a;

	float dx, dy, speed, moveTimer = NULL; //

	int health; //

	bool life, isMove, onGround;

	enum { b, c, d, e, f } var;


	Global()
	{	}
};







//Класс игрока Box2d
class Player : public Global {
public:

	Player(Level &lev,float x, float y){
    //Создание BOx2d обьекта
    setPlayer(x,y);

    //Создание текстуры обьекту
    text = loadImg ("images/player.png");
    setTextureRect(0,0,50,100);
	setOrigin(25,50);
	}
};


//Статичный пол типа Solid обьекты Box2d
class GroundS : public Global {
public:

	GroundS(float x, float y, int w, int h, int a){
    setWall(x,y,w,h,a);
 	}


};


//Качели обьекты Box2d
class Swing : public Global {
public:

	Swing(int var, int x,int y,int w,int h){

       //Выбор и создание текстуры
    switch (var)
        {
        case b:
            //Создание Box2D обьекта

            setSwing(x,y);

            text = loadImg ("images/objects2.png");
            setTextureRect(378,0,28,242);
            setOrigin(20,121);
            break;
        case c:
            //Создание Box2D обьекта
            setSwing(x,y);

            text = loadImg ("images/objects2.png");
            setTextureRect(200,0,28,200);
            setOrigin(20,121);
            break;

        case d: break;
        case e: break;
        case f: break;
        }
	}


};




//Динамичные ящики обьекты Box2d
class BoxD : public Global {
public:

	BoxD(int var, int x,int y,int w,int h, int a){
    //Выбор и создание текстуры
    switch (var)
        {
        case b:
            //Создание Box2D обьекта
            setBox(x,y,w,h,a);
            text = loadImg ("images/objects1.png");
            setTextureRect(81,0,61,61);
            setOrigin(30,30);
            break;
        case c:
            //Создание Box2D обьекта
            setBoxIron(x,y,w,h,a);
            text = loadImg ("images/met.jpg");
            setTextureRect(0,0,100,100);
            setScale(0.6,0.6);
            setOrigin(50,50);
            break;
        case d: break;
        case e: break;
        case f: break;
        }



	}


};

//Камни обьекты Box2d
class Stone : public Global {
public:


	Stone(int var, int x, int y){

	   //Выбор и создание текстуры
    switch (var)
        {
        case b:
            //Создание Box2D обьекта
            setStone (x, y);

            text = loadImg ("images/objects1.png");
            setTextureRect(0,0,80,80);
            setOrigin(40,40);
            break;
        case c: break;
        case d: break;
        case e: break;
        case f: break;
        }
	}
};





//Динамичные обьекты на заднем фоне
class BackD : public Global {
public:

	BackD( ){


	}


};


int main(int argc, char** argv){

   if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_AUDIO) < 0){std::cout << SDL_GetError() << std::endl; return 1;}

    // Подгружаем Настройки игры
    InitWindow();
    InitMedia();
    InitMusic();

    // Запуск таймера
    Timer timer;
    float times;
    std::stringstream fps;


    // Создание карты
    Level lvl;//создали экземпляр класса уровень
    lvl.LoadFromFile("map2.tmx");
    obj = lvl.GetAllObjects();//инициализируем.получаем все объекты для взаимодействия персонажа с картой
    Object player=lvl.GetObject("player");//объект игрока на нашей карте.задаем координаты игроку в начале при помощи него
	//Object easyEnemyObject = lvl.GetObject("easyEnemy");//объект легкого врага на нашей карте.задаем координаты игроку в начале при помощи него





    // Создание обьектов мира



    Player      sPlayer(lvl, player.rect.x, player.rect.y);


BoxD *b, *bi;
Stone *st;
Swing *sw;
GroundS *solid;


   for (int i = 0; i < obj.size(); i++)//проходимся по объектам
   {

        if (obj[i].name == "solid")//находим все обьекты с именем solid
        {solid = new GroundS (obj[i].rect.x,obj[i].rect.y,obj[i].rect.w,obj[i].rect.h,obj[i].rotation);

         }

        if (obj[i].name == "box")//находим все обьекты с именем solid
        {b = new BoxD (0, obj[i].rect.x, obj[i].rect.y, obj[i].rect.w , obj[i].rect.h, obj[i].rotation );}

        if (obj[i].name == "ironbox")//находим все обьекты с именем solid
        {bi = new BoxD (1, obj[i].rect.x, obj[i].rect.y, obj[i].rect.w , obj[i].rect.h, obj[i].rotation );}

        if (obj[i].name == "stone")//находим все обьекты с именем solid
        {st = new Stone (0, obj[i].rect.x, obj[i].rect.y);}

        if (obj[i].name == "swing")//находим все обьекты с именем solid
        {sw = new Swing (0, obj[i].rect.x, obj[i].rect.y, 5, 120);}

}

            SDL_Sprite bulSprite;
            bulSprite.setTextureFile("images/bullet2.png");
            bulSprite.setTextureRect(0,0,18,35);
            bulSprite.setRotation (0);
            bulSprite.setOrigin(9,17);
            bulSprite.setScale(0.3,0.3);

            SDL_Sprite bulSprite2;
            bulSprite2.setTextureFile("images/bullet.png");
            bulSprite2.setTextureRect(0,0,35,35);
            bulSprite2.setRotation (0);
            bulSprite2.setOrigin(16,16);
            bulSprite2.setScale(0.3,0.3);




    // переменные для управление циклом сообщений, мышкой и клавой
    int xx,yy;

    bool quit = false;
    int LBM=0;
    int MBM=0;
    int RBM=0;
    int CLBM,CRBM,CMBM;

    int     KEY_UP   =  0;
    int     KEY_DOWN =  0;
    int     KEY_LEFT =  0;
    int     KEY_RIGHT = 0 ;
    int     KEY_SPACE = 0 ;

    SDL_Event e;
    //////////////////

// Цикл игры
while (!quit)
{   CLBM = CRBM = CMBM = 0;

    SDL_GetMouseState( &xx, &yy );

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_WINDOWEVENT )
        if (e.window.windowID == SDL_GetWindowID(window))
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {GS.width = e.window.data1;GS.height = e.window.data2;}
		if (e.type == SDL_QUIT){quit = true;}
		if (e.type == SDL_MOUSEBUTTONDOWN)
                        {
                            if (e.button.button == SDL_BUTTON_LEFT)     {LBM=1; CLBM =1;}
                            if (e.button.button == SDL_BUTTON_RIGHT)    {RBM=1; CRBM =1;}
                            if (e.button.button == SDL_BUTTON_MIDDLE)   {MBM=1; CMBM =1;}
                        }

       if (e.type == SDL_MOUSEBUTTONUP)
                        {
                            if (e.button.button == SDL_BUTTON_LEFT)     {LBM=0; }
                            if (e.button.button == SDL_BUTTON_RIGHT)    {RBM=0; }
                            if (e.button.button == SDL_BUTTON_MIDDLE)   {MBM=0; }
                        }



                        /*  if (e.type  == SDL_MOUSEMOTION)
                        {
                                xx=e.button.x;
                                yy=e.button.y;
                        }*/

                     if (e.type  == SDL_MOUSEWHEEL)

                         {
                             int x = e.wheel.y;
                         }


                         if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
                                {
                                    //Adjust the velocity
                                    switch( e.key.keysym.sym )
                                    {
                                        case SDLK_w:       KEY_UP =    1 ;break;
                                        case SDLK_s:     KEY_DOWN =  1 ;break;
                                        case SDLK_a:     KEY_LEFT =  1 ;break;
                                        case SDLK_d:    KEY_RIGHT = 1 ;break;
                                        case SDLK_SPACE:    KEY_SPACE = 1 ;break;
                                    }
                                }
                                //If a key was released
                           else
                           if( e.type == SDL_KEYUP && e.key.repeat == 0 )
                                {
                                    //Adjust the velocity
                                    switch( e.key.keysym.sym )
                                    {
                                        case SDLK_w:       KEY_UP =    0 ;break;
                                        case SDLK_s:     KEY_DOWN =  0 ;break;
                                        case SDLK_a:     KEY_LEFT =  0 ;break;
                                        case SDLK_d:    KEY_RIGHT = 0 ;break;
                                        case SDLK_SPACE:    KEY_SPACE = 0 ;break;
                                    }
                                }

    }



    // Рисуем в окне
    SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x10, 0xFF);
    SDL_RenderClear(renderer);

    //рисуем новую карту
    lvl.Draw(renderer);



        // Управление Box2d обьектами
        World.Step(1/60.f, 10, 10);

        bool onGround=0;
        b2Vec2 pos = playerBody->GetPosition();
        b2Vec2 PlayerCentr = pos;
        pos.y+=2;

        setPlayerCoordinateForView(PlayerCentr.x*SCALE, PlayerCentr.y*SCALE);

        if (PlayerCentr.x*SCALE < xx + camera.x  ) sPlayer.setScale (1,1); else sPlayer.setScale (-1,1);

        for (b2Body* it = World.GetBodyList(); it != 0; it = it->GetNext())
        for (b2Fixture *f = it->GetFixtureList(); f!=0; f=f->GetNext())
        if  (f->TestPoint(pos))  {onGround=true;}

        b2Vec2 Skorost = playerBody->GetLinearVelocity();
		float  angVel = playerBody->GetAngularVelocity();

		if (KEY_RIGHT && onGround ) if (Skorost.x < 10) {playerBody->ApplyLinearImpulse( b2Vec2 ( 100,0),  b2Vec2 ( 0, 0),  1);};
		if (KEY_LEFT && onGround)  if (Skorost.x > -10)  {playerBody->ApplyLinearImpulse( b2Vec2 ( -100,0), b2Vec2 ( 0, 0),  1);};
		if (KEY_SPACE && onGround)
                {
                        playerBody->ApplyLinearImpulse(b2Vec2 (0,-100), b2Vec2 ( 0, 0) ,  1);
                };

if (LBM) {

                Mix_PlayChannel( 0, FireWAV, 0 );
                float dx =  xx + camera.x - SCALE * PlayerCentr.x ;
                float dy =  yy + camera.y - SCALE * PlayerCentr.y ;
                float dz =  sqrt(dx*dx+dy*dy);
                float Skorost = 20;
                float Sila = 10;

for (int i = 0; i<100;i++)
                {
                float angle;


                if ( dx >= 0 && dy  >= 0) angle =       asin (dx/dz)*180/3.14;
                if ( dx >= 0 && dy  <  0) angle = 180 - asin (dx/dz)*180/3.14;
                if ( dx  < 0 && dy  <= 0) angle = 180 - asin (dx/dz)*180/3.14;
                if ( dx <  0 && dy  >  0) angle = 360 + asin (dx/dz)*180/3.14;




                float ang = rand()%20-10;
                angle = angle + ang;


                BL p0 = {2,
                         50 * sin  (M_PI / 180 * angle) + PlayerCentr.x * 30,
                         50 * cos  (M_PI / 180 * angle) + PlayerCentr.y * 30,
                         Skorost * sin  (M_PI / 180 * angle),
                         Skorost * cos  (M_PI / 180 * angle),
                         -angle,
                         Sila};
                  oo.push_back(p0);


                          } }

 if (RBM)
       {
                Mix_PlayChannel( 0, FireWAV, 0 );


                float dx =  xx + camera.x - SCALE * PlayerCentr.x ;
                float dy =  yy + camera.y - SCALE * PlayerCentr.y ;
                float dz =  sqrt(dx*dx+dy*dy);
                float Skorost = 20;
                float Sila = 10;

                for (int i = 0; i<100;i++)
                {
                float angle;

                if ( dx >= 0 && dy  >= 0) angle =       asin (dx/dz)*180/3.14;
                if ( dx >= 0 && dy  <  0) angle = 180 - asin (dx/dz)*180/3.14;
                if ( dx  < 0 && dy  <= 0) angle = 180 - asin (dx/dz)*180/3.14;
                if ( dx <  0 && dy  >  0) angle = 360 + asin (dx/dz)*180/3.14;
                angle = angle + rand()% 3 - 1;





                float ang = rand()%20-10;
                angle = angle + ang;

                BL p0 = {1,
                         50 * sin   (M_PI / 180 * angle) + PlayerCentr.x * 30,
                         50 * cos   (M_PI / 180 * angle) + PlayerCentr.y * 30,
                         Skorost * sin (M_PI / 180 * angle),
                         Skorost * cos (M_PI / 180 * angle),
                         -angle,
                         Sila};
                  oo.push_back(p0);
                }
       }

                       // Рисование текстур на box2d обьекты
            for (b2Body* it = World.GetBodyList(); it != 0; it = it -> GetNext())
            {
		    b2Vec2 pos = it->GetPosition();
                			float angle = it->GetAngle();

			if (it->GetUserData()=="box")
			{

			b->setPosition(pos.x*SCALE,pos.y*SCALE);
			b->setRotation(angle*DEG);
            b->Draw (renderer);
			}

			if (it->GetUserData()=="boxiron")
			{
            bi->setPosition(pos.x*SCALE,pos.y*SCALE);
			bi->setRotation(angle*DEG);
			bi->Draw (renderer);
			}

			if (it->GetUserData()=="player")
			{
				sPlayer.setPosition(pos.x*SCALE,pos.y*SCALE);
				sPlayer.setRotation(angle*DEG);
				sPlayer.Draw (renderer);
			}

			if (it->GetUserData()=="swing")
			{
			sw->setPosition(pos.x*SCALE,pos.y*SCALE);
			sw->setRotation(angle*DEG);
			sw->Draw (renderer);
			}

			if (it->GetUserData()=="stone")
			{
			st->setPosition(pos.x*SCALE,pos.y*SCALE);
			st->setRotation(angle*DEG);
			st->Draw (renderer);
			}



          }

     if (!oo.empty())
         {
          for (std::vector<BL>::iterator i = oo.begin(); i != oo.end(); ++i)
                  {
                    if (i->name == 1)
                    {
                    bulSprite.setPosition(i->x,i->y);
                    bulSprite.setRotation(i->ang);
                    bulSprite.Draw (renderer);
                    }
                    if (i->name == 2)
                    {
                    bulSprite2.setPosition(i->x,i->y);
                    bulSprite2.setRotation(i->ang);
                    bulSprite2.Draw (renderer);
                    }

                    i->x = i->x + i->vx;
                    i->y = i->y + i->vy;

                  }
          }

        for (b2Body* it = World.GetBodyList(); it != 0; it = it->GetNext())
        for (b2Fixture *f = it->GetFixtureList(); f!=0; f=f->GetNext())
        if (!oo.empty())
        for (std::vector<BL>::iterator i = oo.begin(); i != oo.end();)
        if  (f->TestPoint(b2Vec2{(i->x + i->vx)/SCALE,(i->y + i->vy)/SCALE}))
        {
         it->ApplyLinearImpulse( b2Vec2( i->massa*i->vx,i->massa*i->vy ), b2Vec2(i->x/SCALE,i->y/SCALE) , 1);
        // it->ApplyForce( b2Vec2( i->massa*i->vx,i->massa*i->vy ), b2Vec2(i->x/SCALE,i->y/SCALE) , 1);
         i = oo.erase(i);
        } else ++i;






        // Музыка
        Mix_PlayChannel( -1, Sel_SB, 0 );

        // Отображаем все
        SDL_RenderPresent(renderer);

   // system ("CLS");


    /** FPS счетчик  **/
   times = 0.1 * timer.GetTimer();
   timer.Reset();
   fps.str("");
   fps <<"FPS"<< timer.GetFPS();


   SDL_SetWindowTitle(window,fps.str().c_str());
}



   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();

return 0;
}
