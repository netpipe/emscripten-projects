#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Mix_Chunk* chunk;

 bool success = true;

extern "C"
{

int mainSOX(int argc, char **argv);
 }

int main( int arc, char* args[] )
{
  int note;
  SDL_Init(SDL_INIT_AUDIO);

                 //Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 6, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }

                	char *argv1[]={"appname","-v" ,"0.8","/media/bling.wav","out.wav","pitch","-800","test"};
	int argc1 = sizeof(argv1) / sizeof(char*) - 1;

	mainSOX(argc1,argv1);

		chunk = Mix_LoadWAV("/out.wav");
  		const int channel = Mix_PlayChannel(-1, chunk, -1);

     Mix_PlayChannel(-1, chunk, 0);

   return 0;
}


//                            case SDLK_9:
//                            //If there is no music playing
//                            if( Mix_PlayingMusic() == 0 )
//                            {
//                                //Play the music
//                                Mix_PlayMusic( gMusic, -1 );
//                            }
//                            //If music is being played
//                            else
//                            {
//                                //If the music is paused
//                                if( Mix_PausedMusic() == 1 )
//                                {
//                                    //Resume the music
//                                    Mix_ResumeMusic();
//                                }
//                                //If the music is playing
//                                else
//                                {
//                                    //Pause the music
//                                    Mix_PauseMusic();
//                                }
//                            }
//                            break;
//
//                            case SDLK_0:
//                            //Stop the music
//                            Mix_HaltMusic();
//                            break;
//                        }
