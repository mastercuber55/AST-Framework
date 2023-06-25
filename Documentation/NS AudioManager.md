# AST Framework
## Documentation
### namespace AudioManager
###### THIS NAMESPACE IS ONLY AVAIABLE IF `-DAST_AUDIO` WAS PASSED AS A CXX FLAG
#### Variables


- ##### `std::unordered_map<std::string, Mix_Chunk*> AudioManager::Chunks;`

**Description:** Holds all chunks loaded by the `AudioManager`.

**Example:** None

- ##### `std::unordered_map<std::string, Mix_Music*> AudioManager::Musics;`

**Description:** Holds all musics loaded by the `AudioManager`.

**Example:** None

#### Functions

- ##### `void AudioManager::Play(std::string file, bool chunk = true)`

**Description:** 

If chunk was provided as true or wasn't provided: Loads the audio file and saves it to `AudioManager::Chunks` if not loaded and plays the chunk.

else: Loads the audio file and saves it to `AudioManager::Musics` if not loaded and plays the music.

**Example:**` AudioManager::Play("jump.ogg");`

- ##### `void AudioManager::Free();`

**Description:** Frees all memory taken by `Mix_Chunk*`(s) in `AudioManager::Chunks` and by `Mix_Musics*`(s) in `AudioManager::Musics`.

**Example:** `AudioManager::Free();`