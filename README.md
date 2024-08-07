# Plugin WardenSystem
## Project Description
The plugin is aimed at replacing the "Significance Manager" module with improved clustering and versatility of using the module without unnecessary callback functions.  
This module does not use unnecessary calculations in the form of numerical float values for their subsequent transmission as a signature to callback calls.  
Also, the module does not use lambda expressions as callbacks, which often led to the crash of the "Significance Manager" module. This module uses only the Object class to guarantee the function call.  
Supported versions of the Unreal Engine: UE5.(0-4)  
## API
- Object registration
- Object unregistration
- Checking on exist object

![Image](https://github.com/markveligod/WardenSystem/blob/main/Assets/API.png)  

## Threads
Since version 1.3, I have added a cluster on a other thread and switching is done in Project Settings -> Game -> WardenSettings.  
Attention!!!  
Unreal Engine works very poorly on other threads with actor spawning, object creation, etc.  Be careful when using the system on a separate thread  
By default, I have set cluster processing to GameThread  


## 📫 Other <a name="Other"></a>
:bangbang: Attention: If you can improve my trash code then make a pull request.  

**:copyright:Authors:**  
  
*[Mark Veligod](https://github.com/markveligod)*  
