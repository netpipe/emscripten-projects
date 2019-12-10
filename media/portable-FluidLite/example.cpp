include "fluidlite.h"

fluid_settings_t* settings=new_fluid_settings();
fluid_synth_t* synth=new_fluid_synth(settings);
fluid_synth_sfload(synth, "soundfont.sf3",1);

float* buffer=new float[44100*2];

FILE* file=fopen("float32output.pcm","wb");

fluid_synth_noteon(synth,0,60,127);
fluid_synth_write_float(synth, 44100,buffer, 0, 2, buffer, 1, 2);
fwrite(buffer,sizeof(float),44100*2,file);

fluid_synth_noteoff(synth,0,60);
fluid_synth_write_float(synth, 44100,buffer, 0, 2, buffer, 1, 2);
fwrite(buffer,sizeof(float),44100*2,file);

fclose(file);

delete_fluid_synth(synth);
delete_fluid_settings(settings);
