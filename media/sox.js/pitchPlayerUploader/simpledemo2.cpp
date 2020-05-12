#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Mix_Chunk* chunk;

 bool success = true;



extern "C"
{

int mainSOX(int argc, char **argv);
 }


std::vector<std::string> fileNames;

extern "C" void addFile(const char *name) {
  fileNames.push_back(name);
  std::cout << "Added file " << name << '\n';
}
extern "C" void list_dir(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n",entry->d_name);
    }

    closedir(dir);
}

EM_JS(void, addfiletolist2, (const char *name, const char *type), {
  window.addFileToList(UTF8ToString(name), UTF8ToString(type));
});

extern "C" void processFiles() {
  std::cout << "Processing files\n";
  for (const std::string &name : fileNames) {
    // Do whatever you need to do...
    std::cout << "Processing file " << name << '\n';

    // You can do whatever you want with the file.
    std::ifstream file{name};
    if (file.is_open()) {
      std::cout << "First byte of the file is: " << file.get() << '\n';
    }
    list_dir("/");
    //char name2 = "./"+name.c_str();
    char *cstr = new char[name.length() + 1];
strcpy(cstr, name.c_str());
                    	char *argv1[]={"appname","-v" ,"0.8",cstr,"out.wav","pitch","-800","test"};
	int argc1 = sizeof(argv1) / sizeof(char*) - 1;
mainSOX(argc1,argv1);
    		chunk = Mix_LoadWAV("out.wav");
  		const int channel = Mix_PlayChannel(-1, chunk, -1);

  		addfiletolist2("out.wav", "audio/wav");

     Mix_PlayChannel(-1, chunk, 0);
  }




  std::cout << "Done processing\n";
  fileNames.clear();
}



EM_JS(void, initialize, (), {
  function err(e) {
    throw e;
  }

  // Add the selected files to the emscripten virtual file system.
  function handleFiles() {
    // Wrap the C++ functions as JavaScript so that they can be called
    // https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#interacting-with-code-ccall-cwrap
    let addFile = Module["cwrap"]("addFile", null, ["string"]);
    let processFiles = Module["cwrap"]("processFiles", null, []);

    let completedCount = 0;
    let files = this.files;

    for (let file of files) {
      let reader = new FileReader();
      reader.onerror = err;

      reader.onloadend = () => {
        // Add the file to the list
        window.addFileToList(file.name, file.type);

        // A new file is created on the emscripten virtual file system.
        // This is part of the private file system API
        // https://emscripten.org/docs/api_reference/advanced-apis.html#advanced-file-system-api
        Module["FS_createDataFile"](
          ".", // directory on the virtual file system to place the file in
          file.name, // name of the file
          new Uint8Array(reader.result), // data to file the file with
          true, // Allow C++ to read the file
          true // Allow C++ to write the file
        );

        // Tell C++ that the file is accessible
        addFile(file.name);

        // Tell C++ that all files are accessible after creating all the files
        ++completedCount;
        if (completedCount == files.length) {
          processFiles();
        }
      };

      // reader.result will be an ArrayBuffer
      reader.readAsArrayBuffer(file);
    }
  }

  // Add a file to the list of files.
  // Clicking on a file in the list will download it.
  window.addFileToList = (name, type) => {
    let item = document.createElement("div");
    item.innerHTML = name;

    item.onclick = () => {
      // Reading the file from the emscripten virtual file system.
      // buffer is a Uint8Array
      let buffer = FS.readFile(name);
      // Construct a Blob from the Uint8Array
      let blob = new Blob([buffer], {type: type});
      // Prepare the download button to download the Blob
      let link = document.getElementById("downloadButton");
      link.href = URL.createObjectURL(blob);
      link.download = name;
      // Download the file
      link.click();
    };

    // Very basic styling. You can change this.
    item.style = `
      display: block;
      cursor: pointer;
      padding: 11px 23px;
      margin: 10px;

      font-family: Arial;
      font-size: 12px;
      font-weight: bold;

      color: #FFFFFF;
      background-color: #777;

      border-radius: 5px;
      border: 1px solid #666;
    `;

    let list = document.getElementById("fileList");
    list.appendChild(item);
  };

  // Create the file input element.
  // This is an invisible element that is clicked to upload files.
  function createFileInput() {
    let input = document.createElement("input");
    input.type = "file";
    input.multiple = true;
    input.style.display = "none";
    input.id = "fileInput";
    input.onchange = handleFiles;
    document.body.appendChild(input);
  }

  // Create the upload button.
  // Clicking this button will click on the file input
  function createUploadButton() {
    let button = document.createElement("div");
    button.innerHTML = "Upload File to FileSystem";
    button.id = "uploadButton";

    button.onclick = () => {
      document.getElementById("fileInput").click();
    };

    // Very basic styling. You can change this.
    button.style = `
      display: inline-block;
      cursor: pointer;
      padding: 11px 23px;
      margin: 20px;

      font-family: Arial;
      font-size: 15px;
      font-weight: bold;

      color: #FFF;
      background-color: #007DC1;

      border-radius: 5px;
      border: 1px solid #124D77;
    `;

    document.body.appendChild(button);
  };

  // Create the file list.
  // This is a list of all the file names. Files are added to this list when
  // they're uploaded.
  function createFileList() {
    let list = document.createElement("div");
    list.id = "fileList";

    // Very basic styling. You can change this.
    list.style = `
      display: block;
      margin: 20px;
      width: 400px;

      background-color: #EEE;

      border-radius: 5px;
      border: 1px solid #DDD;
    `;

    document.body.appendChild(list);
  }

  // Create the download button.
  // This is an invisible download button that is used to download files.
  function createDownloadButton() {
    let link = document.createElement("a");
    link.id = "downloadButton";
    link.style.display = "none";
    document.body.appendChild(link);
  }

  createFileInput();
  createUploadButton();
  createFileList();
  createDownloadButton();
});

int main( int arc, char* args[] )
{
initialize();
  int note;
  SDL_Init(SDL_INIT_AUDIO);

                 //Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 6, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }





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

